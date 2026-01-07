#include "PakReader.h"
#include "textureManager.h"

void PakReader::ParseImages(const std::string& _path) {
	std::lock_guard lock(mutex);

	imgPath = _path;

	imgInput = std::ifstream(imgPath, std::ios::binary);
	if (!imgInput.is_open()) {
		std::cout << "failed to open: " << imgPath << "\n";
		return;
	}

	// load header
	uint32_t headerSize = sizeof(PakHeader);
	std::vector<unsigned char*> headerBuffer(headerSize);
	imgInput.seekg(0);
	imgInput.read(reinterpret_cast<char*>(headerBuffer.data()), headerSize);

	imgHeader = new PakHeader();
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
	imgPath.clear();
	delete imgHeader;
	imgDirectory.clear();
	imgInput.close();
}

TextureEntry PakReader::GetImgEntry(uint32_t hashedId) {
	std::lock_guard lock(mutex);
	return imgDirectory[hashedId];
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

	shaderHeader = new PakHeader();
	memcpy(shaderHeader, reinterpret_cast<PakHeader*>(headerBuffer.data()), sizeof(PakHeader));

	// load directory
	uint32_t dirSize = shaderHeader->dirCount * sizeof(ShaderEntry);
	std::vector<unsigned char*> dirBuffer(dirSize);
	shaderInput.seekg(sizeof(PakHeader));
	shaderInput.read(reinterpret_cast<char*>(dirBuffer.data()), dirSize);

	std::vector<ShaderEntry> shaderEntries(shaderHeader->dirCount);
	memcpy(shaderEntries.data(), dirBuffer.data(), dirBuffer.size());

	shaderMap.reserve(shaderEntries.size());
	for (ShaderEntry entry : shaderEntries) {
		uint32_t size = entry.size;
		std::vector<uint8_t> buffer(size);
		shaderInput.seekg(entry.offset);
		shaderInput.read(reinterpret_cast<char*>(buffer.data()), size);

		std::string shaderData(buffer.begin(), buffer.end());
		shaderMap.insert({ entry.hashId, shaderData });
	}
}

void PakReader::ClearShaderData() {
	shaderPath.clear();
	shaderInput.close();
	delete shaderHeader;
	shaderMap.clear();
}

std::string PakReader::GetShader(const std::string& path) {
	auto it = shaderMap.find(Hash(path));
	if (it != shaderMap.end())
		return it->second;
	
	std::cout << "\"" << path << "\" was not in shaderMap!\n";
	return "";
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