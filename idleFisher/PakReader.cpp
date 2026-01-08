#include "PakReader.h"
#include "textureManager.h"
#include "text.h"

#include "debugger.h"

void PakReader::ParseImages(const std::string& path) {
	std::lock_guard lock(mutex);

	imgInput = std::ifstream(path, std::ios::binary);
	if (!imgInput.is_open()) {
		std::cout << "failed to open: " << path << "\n";
		return;
	}

	// load header
	uint32_t headerSize = sizeof(PakHeader);
	std::vector<unsigned char*> headerBuffer(headerSize);
	imgInput.seekg(0);
	imgInput.read(reinterpret_cast<char*>(headerBuffer.data()), headerSize);

	PakHeader* imgHeader = new PakHeader();
	memcpy(imgHeader, reinterpret_cast<PakHeader*>(headerBuffer.data()), sizeof(PakHeader));

	// load directory
	uint32_t dirSize = imgHeader->dirCount * sizeof(TextureEntry);
	std::vector<unsigned char*> dirBuffer(dirSize);
	imgInput.seekg(sizeof(PakHeader));
	imgInput.read(reinterpret_cast<char*>(dirBuffer.data()), dirSize);

	std::vector<TextureEntry> textureEntries(imgHeader->dirCount);
	memcpy(textureEntries.data(), dirBuffer.data(), dirBuffer.size());

	imgDirectory.reserve(textureEntries.size());
	for (TextureEntry entry : textureEntries)
		imgDirectory.insert({ entry.hashId, std::move(entry) });

	delete imgHeader;
}

std::vector<uint8_t> PakReader::LoadTexture(const TextureEntry& entry) {
	std::lock_guard lock(mutex);

	uint32_t size = entry.size;
	std::vector<uint8_t> buffer(size);
	imgInput.seekg(entry.offset);
	imgInput.read(reinterpret_cast<char*>(buffer.data()), size);
	return buffer;
}

void PakReader::LoadTextures(std::unordered_map<uint32_t, std::unique_ptr<textureStruct>>& textureMap) {
	std::lock_guard lock(mutex);
	
	for (auto& entry : imgDirectory) {
		auto it = textureMap.find(entry.second.hashId);
		if (it != textureMap.end())
			continue; // already in map

		uint32_t size = entry.second.size;
		std::vector<uint8_t> buffer(size);
		imgInput.seekg(entry.second.offset);
		imgInput.read(reinterpret_cast<char*>(buffer.data()), size);

		textureMap[entry.second.hashId] = std::make_unique<textureStruct>(
			buffer, entry.second.width, entry.second.height, entry.second.format, entry.second.flags
		);
	}

	// clean up
	imgDirectory.clear();
	imgInput.close();
}

TextureEntry PakReader::GetImgEntry(uint32_t hashedId) {
	std::lock_guard lock(mutex);

	auto it = imgDirectory.find(hashedId);
	if (it != imgDirectory.end())
		return imgDirectory[hashedId];

	std::cout << "No Image with id '" << hashedId << "' was found in imgDirectory\n";
	abort();
}

void PakReader::ParseShaders(const std::string& path) {
	shaderInput = std::ifstream(path, std::ios::binary);
	if (!shaderInput.is_open()) {
		std::cout << "failed to open: " << path << "\n";
		return;
	}

	// load header
	uint32_t headerSize = sizeof(PakHeader);
	std::vector<unsigned char*> headerBuffer(headerSize);
	shaderInput.seekg(0);
	shaderInput.read(reinterpret_cast<char*>(headerBuffer.data()), headerSize);

	PakHeader* shaderHeader = new PakHeader();
	memcpy(shaderHeader, reinterpret_cast<PakHeader*>(headerBuffer.data()), sizeof(PakHeader));

	// load directory
	uint32_t dirSize = shaderHeader->dirCount * sizeof(Entry);
	std::vector<unsigned char*> dirBuffer(dirSize);
	shaderInput.seekg(sizeof(PakHeader));
	shaderInput.read(reinterpret_cast<char*>(dirBuffer.data()), dirSize);

	std::vector<Entry> shaderEntries(shaderHeader->dirCount);
	memcpy(shaderEntries.data(), dirBuffer.data(), dirBuffer.size());

	shaderMap.reserve(shaderEntries.size());
	for (Entry entry : shaderEntries) {
		uint32_t size = entry.size;
		std::vector<uint8_t> buffer(size);
		shaderInput.seekg(entry.offset);
		shaderInput.read(reinterpret_cast<char*>(buffer.data()), size);

		std::string shaderData(buffer.begin(), buffer.end());
		shaderMap.insert({ entry.hashId, shaderData });
	}

	delete shaderHeader;
}

void PakReader::ClearShaderData() {
	shaderInput.close();
	shaderMap.clear();
}

std::string PakReader::GetShader(const std::string& path) {
	auto it = shaderMap.find(Hash(path));
	if (it != shaderMap.end())
		return it->second;
	
	std::cout << "\"" << path << "\" was not in shaderMap!\n";
	return "";
}

void PakReader::ParseFonts(const std::string& path) {
	std::ifstream fontInput(path, std::ios::binary);
	if (!fontInput.is_open()) {
		std::cout << "failed to open: " << path << "\n";
		return;
	}

	// load header
	uint32_t headerSize = sizeof(PakHeader);
	std::vector<unsigned char*> headerBuffer(headerSize);
	fontInput.seekg(0);
	fontInput.read(reinterpret_cast<char*>(headerBuffer.data()), headerSize);

	PakHeader* fontHeader = new PakHeader();
	memcpy(fontHeader, reinterpret_cast<PakHeader*>(headerBuffer.data()), sizeof(PakHeader));

	// load directory
	uint32_t dirSize = fontHeader->dirCount * sizeof(Entry);
	std::vector<unsigned char*> dirBuffer(dirSize);
	fontInput.seekg(sizeof(PakHeader));
	fontInput.read(reinterpret_cast<char*>(dirBuffer.data()), dirSize);

	std::vector<Entry> fontEntries(fontHeader->dirCount);
	memcpy(fontEntries.data(), dirBuffer.data(), dirBuffer.size());

	fontMap.reserve(fontEntries.size());
	for (Entry entry : fontEntries) {
		uint32_t size = entry.size;
		std::vector<uint8_t> buffer(size);
		fontInput.seekg(entry.offset);
		fontInput.read(reinterpret_cast<char*>(buffer.data()), size);

		FfontInfo* fontInfo = new FfontInfo();

		// first 2 bytes are font height, second 2 are drop height
		std::memcpy(&fontInfo->height, buffer.data(), sizeof(uint16_t));
		std::memcpy(&fontInfo->dropHeight, buffer.data() + 2, sizeof(uint16_t));

		// then the list of rects
		size_t remainingSize = (buffer.size() - sizeof(uint16_t) * 2) / sizeof(Rect);
		fontInfo->letterRect.resize(remainingSize);
		std::memcpy(fontInfo->letterRect.data(), buffer.data() + sizeof(uint16_t) * 2, remainingSize * sizeof(Rect));

		fontMap.insert({ entry.hashId, fontInfo });
	}

	fontInput.close();
	delete fontHeader;
}

void PakReader::ClearTextData() {
	for (auto& texture : fontMap)
		delete texture.second;
	fontMap.clear();
}

FfontInfo* PakReader::GetFontData(const std::string& path) {
	auto it = fontMap.find(Hash(path));
	if (it != fontMap.end())
		return it->second;

	std::cout << "\"" << path << "\" was not in fontMap!\n";
	return nullptr;
}

void PakReader::ParseAudio(const std::string& path) {
	audioInput = std::ifstream(path, std::ios::binary);
	if (!audioInput.is_open()) {
		std::cout << "failed to open: " << path << "\n";
		return;
	}

	// load header
	uint32_t headerSize = sizeof(PakHeader);
	std::vector<unsigned char*> headerBuffer(headerSize);
	audioInput.seekg(0);
	audioInput.read(reinterpret_cast<char*>(headerBuffer.data()), headerSize);

	PakHeader* audioHeader = new PakHeader();
	memcpy(audioHeader, reinterpret_cast<PakHeader*>(headerBuffer.data()), sizeof(PakHeader));

	// load directory
	uint32_t dirSize = audioHeader->dirCount * sizeof(Entry);
	std::vector<unsigned char*> dirBuffer(dirSize);
	audioInput.seekg(sizeof(PakHeader));
	audioInput.read(reinterpret_cast<char*>(dirBuffer.data()), dirSize);

	std::vector<Entry> audioEntries(audioHeader->dirCount);
	memcpy(audioEntries.data(), dirBuffer.data(), dirBuffer.size());

	audioDirectory.reserve(audioEntries.size());
	for (Entry entry : audioEntries)
		audioDirectory.insert({ entry.hashId, std::move(entry) });

	delete audioHeader;
}

void PakReader::LoadAllAudio(std::unordered_map<uint32_t, std::unique_ptr<std::vector<uint8_t>>>& audioMap) {
	for (auto& entry : audioDirectory) {
		auto it = audioMap.find(entry.second.hashId);
		if (it != audioMap.end())
			continue; // already in map

		uint32_t size = entry.second.size;
		std::unique_ptr<std::vector<uint8_t>> buffer = std::make_unique<std::vector<uint8_t>>(size);
		audioInput.seekg(entry.second.offset);
		audioInput.read(reinterpret_cast<char*>(buffer->data()), size);

		audioMap.insert({ entry.second.hashId, std::move(buffer) });
	}

	// clean up
	audioDirectory.clear();
	audioInput.close();
}

std::vector<uint8_t> PakReader::LoadAudio(const std::string& path) {
	auto it = audioDirectory.find(Hash(path));
	if (it == audioDirectory.end()) {
		std::cerr << "Audio file not in audioDirectory: \"" << path << "\"\n";
		abort();
	}

	uint32_t size = it->second.size;
	std::vector<uint8_t> buffer(size);
	audioInput.seekg(it->second.offset);
	audioInput.read(reinterpret_cast<char*>(buffer.data()), size);

	return buffer;
}

uint32_t PakReader::Hash(const std::string& str) {
	const char* c = str.c_str();

	uint32_t hash = 2166136261u; // offset basis
	while (*c)
	{
		hash ^= (uint8_t)(*c);
		hash *= 16777619u;       // prime
		++c;
	}

	return hash;
}

void PakReader::ParseCollision(const std::string& path, std::unordered_map<uint32_t, std::unique_ptr<std::vector<Fcollision>>>& colMap) {
	std::ifstream colInput(path, std::ios::binary);
	if (!colInput.is_open()) {
		std::cout << "failed to open: " << path << "\n";
		return;
	}

	// load header
	uint32_t headerSize = sizeof(PakHeader);
	std::vector<unsigned char*> headerBuffer(headerSize);
	colInput.seekg(0);
	colInput.read(reinterpret_cast<char*>(headerBuffer.data()), headerSize);

	PakHeader* colHeader = new PakHeader();
	memcpy(colHeader, reinterpret_cast<PakHeader*>(headerBuffer.data()), sizeof(PakHeader));

	// load directory
	uint32_t dirSize = colHeader->dirCount * sizeof(Entry);
	std::vector<unsigned char*> dirBuffer(dirSize);
	colInput.seekg(sizeof(PakHeader));
	colInput.read(reinterpret_cast<char*>(dirBuffer.data()), dirSize);

	std::vector<Entry> colEntries(colHeader->dirCount);
	memcpy(colEntries.data(), dirBuffer.data(), dirBuffer.size());

	for (Entry entry : colEntries) {
		uint32_t size = entry.size;
		std::vector<uint8_t> buffer(size);
		colInput.seekg(entry.offset);
		colInput.read(reinterpret_cast<char*>(buffer.data()), size);

		std::vector<Fcollision> colList(entry.size / sizeof(Fcollision));
		std::memcpy(colList.data(), buffer.data(), entry.size);

		colMap.insert({ entry.hashId, std::make_unique<std::vector<Fcollision>>(colList) });
	}
}