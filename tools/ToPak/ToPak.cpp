#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include <filesystem>
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include<stb/stb_image.h>

#include "vector.h"

struct PakHeader {
	uint32_t version;
	uint32_t dirOffset;
	uint32_t dirCount;

	PakHeader() {}
	PakHeader(uint32_t _version, uint32_t _dirOffset, uint32_t _dirCount) : version(_version), dirOffset(_dirOffset), dirCount(_dirCount) {}

	friend std::ostream& operator<<(std::ostream& os, const PakHeader& h) {
		os << "version: " << h.version << ", offset: " << h.dirOffset << ", count: " << h.dirCount;
		return os; 
	}
};

struct TextureEntry {
	uint32_t hashId;
	uint32_t offset;
	uint32_t size;
	uint32_t width;
	uint32_t height;
	uint16_t format; // RGBA8, indexed, etc
	uint16_t flags; // keep CPU copy, alpha mask, etc, a binding texture

	TextureEntry() {}
	TextureEntry(uint32_t _hashId, uint32_t _offset, uint32_t _size, uint32_t _width, uint32_t _height, uint16_t _format, uint16_t _flags)
		: hashId(_hashId), offset(_offset), size(_size), width(_width), height(_height), format(_format), flags(_flags) {}

	friend std::ostream& operator<<(std::ostream& os, const TextureEntry& e) {
		os << "hashId: " << e.hashId << ", offset: " << e.offset << ", size: " << e.size << ", width: " << e.width << ", height: " << e.height << ", format: " << e.format << ", flags: " << e.flags;
		return os;
	}
};

struct Entry { // shaders and text
	uint32_t hashId;
	uint32_t offset;
	uint32_t size;
	
	Entry() {}
	Entry(uint32_t _hashId, uint32_t _offset, uint32_t _size) : hashId(_hashId), offset(_offset), size(_size) {}

	friend std::ostream& operator<<(std::ostream& os, const Entry& e) {
		os << "hashId: " << e.hashId << ", offset: " << e.offset << ", size: " << e.size;
		return os;
	}
};

struct Rect {
	float x = 0;
	float y = 0;
	float w = 0;
	float h = 0;

	Rect() {}
	template<typename T>
	Rect(T _x, T _y, T _w, T _h) : x(static_cast<float>(_x)), y(static_cast<float>(_y)), w(static_cast<float>(_w)), h(static_cast<float>(_h)) {}

	friend std::ostream& operator<<(std::ostream& os, const Rect& r) {
		os << "(" << r.x << ", " << r.y << ", " << r.w << ", " << r.h << ")";
		return os;
	}
};

struct FfontInfo {
	uint16_t textHeight = 0;
	uint16_t dropHeight = 0;
	std::vector<Rect> letterRect = std::vector<Rect>(126);
};

struct Fcollision {
	vector points[4];
	char identifier = ' ';
	bool isCircle = false;
	float radius = 0.f;

	float minX = 0.f;
	float maxX = 0.f;
	float minY = 0.f;
	float maxY = 0.f;

	// for polygon collision
	Fcollision() {}
	Fcollision(std::vector<vector> worldPoints, char identifier) {
		isCircle = false;
		this->points[0] = worldPoints[0];
		this->points[1] = worldPoints[1];
		this->points[2] = worldPoints[2];
		this->points[3] = worldPoints[3];
		this->identifier = identifier;

		minX = INFINITY, maxX = -INFINITY, minY = INFINITY, maxY = -INFINITY;

		// world loc
		for (int i = 0; i < worldPoints.size(); i++) {
			if (points[i].x < minX)
				minX = points[i].x;
			else if (points[i].x > maxX)
				maxX = points[i].x;

			if (points[i].y < minY)
				minY = points[i].y;
			else if (points[i].y > maxY)
				maxY = points[i].y;
		}
	}
};

void ParseImageFile(const char* path);
void Input(const char* pakPath);
void Output(const char* pakPath, const char* rootDir, const std::unordered_map<std::string, unsigned int>& paths);
void OutputShaders(const std::string& pakPath, const std::string& folderName, std::vector<std::string>& shaderPaths);
void InputShaders(const std::string& pakPath);
void OutputText(const std::string& pakPath, std::vector<std::string>& textPaths);
void InputText(const std::string& pakPath);
void OutputCol(const std::string& pakPath, const std::string& colPath);
void InputCol(const std::string& pakPath);
std::string ReadFile(const std::string& path);
std::vector<std::string> getFilesInPath(const std::string& folderPath, std::vector<std::string> extensions, bool recursive, bool removeInputFolderPath);
uint32_t Hash(const std::string& str);
std::string toLower(std::string str);
std::pair<uint16_t, uint16_t> parseTextFile(std::string path, std::vector<Rect>& rects);
std::unordered_map<uint32_t, std::vector<Fcollision>> getCollisionObjects(const std::string& colPath);

std::unordered_map<uint32_t, std::string> hashMap; // see if there are duplicates
std::unordered_map<std::string, unsigned int> imageData;

int main() {
	std::string rootDir("../../idlefisher/");
	std::string base = "../../idleFisher/data/";
	std::string imagePak = base + "images.pak";
	std::string shaderPak = base + "shaders.pak";
	std::string textPak = base + "fonts.pak";
	std::string audioPak = base + "audio.pak";
	std::string colPak = base + "col.pak";

	bool outputImagePak = false;
	bool outputShaderPak = true;
	bool outputTextPak = true;
	bool outputAudioPak = true;
	bool outputColPak = true;

	if (outputImagePak) {
		ParseImageFile((base + "debug/png_files.txt").c_str());
		Output(imagePak.c_str(), rootDir.c_str(), imageData);
		//Input(pakPath.c_str());
	}

	if (outputShaderPak) {
		std::string shaderFolder = "../../idleFisher";
		std::vector<std::string> shaderPaths = getFilesInPath(shaderFolder, { ".vert", ".frag" }, false, true);
		OutputShaders(shaderPak, shaderFolder, shaderPaths);
		InputShaders(shaderPak);
	}

	if (outputTextPak) {
		std::string textFolder = "../../idleFisher/fonts";
		std::vector<std::string> textPaths = getFilesInPath(textFolder, { ".txt" }, true, true);
		OutputText(textPak, textPaths);
		//InputText(textPak);
	}

	if (outputAudioPak) {
		std::string audioFolder = "../../idleFisher/audio";
		std::vector<std::string> audioPaths = getFilesInPath(audioFolder, {}, true, true);
		// same things, loading in the header, directory, and loading in files
		OutputShaders(audioPak, audioFolder, audioPaths);
		//InputShaders(audioPak);
	}

	if (outputColPak) {
		OutputCol(colPak, "../../idleFisher/data/debug/collision.col");
		//InputCol(colPak);
	}

	return 0;
}

void OutputCol(const std::string& pakPath, const std::string& colPath) {

	std::vector<Entry> directory;
	std::vector<unsigned char> bytesList;

	auto colMap = getCollisionObjects(colPath);

	for (auto& col : colMap) {
		// add to directory
		directory.push_back(Entry(
			col.first,
			bytesList.size(),
			col.second.size() * sizeof(col.second[0])
		));

		// add bytes to list
		unsigned char* bytes = reinterpret_cast<unsigned char*>(col.second.data());
		bytesList.insert(bytesList.end(), bytes, bytes + col.second.size() * sizeof(col.second[0]));
	}

	// reupdate the offsets of the image data to account for the offset and size of the directory
	uint32_t offset = sizeof(PakHeader) + directory.size() * sizeof(Entry);
	for (Entry& entry : directory)
		entry.offset += offset;

	// store to file
	std::ofstream output(pakPath, std::ios::binary);
	if (!output.is_open()) {
		std::cout << "Failed to open: " << pakPath << "\n";
		return;
	}

	// output header
	PakHeader header = PakHeader(0, sizeof(PakHeader), directory.size());
	unsigned char* headerBytes = reinterpret_cast<unsigned char*>(&header);
	size_t headerSize = sizeof(PakHeader);
	for (size_t i = 0; i < headerSize; i++)
		output << headerBytes[i];

	// output directory
	unsigned char* directoryBytes = reinterpret_cast<unsigned char*>(directory.data());
	size_t directorySize = directory.size() * sizeof(Entry);
	for (size_t i = 0; i < directorySize; i++)
		output << directoryBytes[i];

	output.write(reinterpret_cast<const char*>(bytesList.data()), bytesList.size());

	output.close();

	// debug
	std::cout << "output header: " << header << "\n";
	for (int i = 0; i < directory.size(); i++)
		std::cout << "output directory[" << i << "]: " << directory[i] << "\n";
}

void InputCol(const std::string& pakPath) {
	std::ifstream colInput(pakPath, std::ios::binary);
	if (!colInput.is_open()) {
		std::cout << "failed to open: " << pakPath << "\n";
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
	}
}

void OutputText(const std::string& pakPath, std::vector<std::string>& textPaths) {
	std::vector<Entry> directory;
	std::vector<unsigned char> bytesList;

	for (auto textPath : textPaths) {

		// need to convert all text letters into Rects, then add that list to directory
		std::vector<Rect> rectsList;
		std::filesystem::path p("../../idleFisher/fonts/" + textPath);
		std::pair<uint16_t, uint16_t> textData = parseTextFile(p.string(), rectsList);

		// add to directory
		directory.push_back(Entry(
			Hash(p.stem().string()), // removes the file extension
			bytesList.size(),
			rectsList.size() * sizeof(rectsList[0]) + sizeof(uint16_t) * 2 // 2 aditional for textHeight and dropHeight
		));

		// add bytes to list
		// first insert the textHeight and dropHeight
		unsigned char* textHeight = reinterpret_cast<unsigned char*>(&textData.first);
		unsigned char* dropHeight = reinterpret_cast<unsigned char*>(&textData.second);
		bytesList.insert(bytesList.end(), textHeight, textHeight + sizeof(unsigned char) * 2);
		bytesList.insert(bytesList.end(), dropHeight, dropHeight + sizeof(unsigned char) * 2);

		// insert body contents
		unsigned char* bytes = reinterpret_cast<unsigned char*>(rectsList.data());
		bytesList.insert(bytesList.end(), bytes, bytes + rectsList.size() * sizeof(rectsList[0]));
	}

	// reupdate the offsets of the image data to account for the offset and size of the directory
	uint32_t offset = sizeof(PakHeader) + directory.size() * sizeof(Entry);
	for (Entry& entry : directory)
		entry.offset += offset;

	// store to file
	std::ofstream output(pakPath, std::ios::binary);
	if (!output.is_open()) {
		std::cout << "Failed to open: " << pakPath << "\n";
		return;
	}

	// output header
	PakHeader header = PakHeader(0, sizeof(PakHeader), directory.size());
	unsigned char* headerBytes = reinterpret_cast<unsigned char*>(&header);
	size_t headerSize = sizeof(PakHeader);
	for (size_t i = 0; i < headerSize; i++)
		output << headerBytes[i];

	// output directory
	unsigned char* directoryBytes = reinterpret_cast<unsigned char*>(directory.data());
	size_t directorySize = directory.size() * sizeof(Entry);
	for (size_t i = 0; i < directorySize; i++)
		output << directoryBytes[i];

	// output image data
	//for (size_t i = 0; i < bytesList.size(); i++)
		//output << bytesList[i];
	output.write(reinterpret_cast<const char*>(bytesList.data()), bytesList.size());

	output.close();

	// debug
	std::cout << "output header: " << header << "\n";
	for (int i = 0; i < directory.size(); i++)
		std::cout << "output directory[" << i << "]: " << directory[i] << "\n";
}

void InputText(const std::string& pakPath) {
	std::ifstream textInput(pakPath, std::ios::binary);
	if (!textInput.is_open()) {
		std::cout << "failed to open: " << pakPath << "\n";
		return;
	}

	// load header
	uint32_t headerSize = sizeof(PakHeader);
	std::vector<unsigned char*> headerBuffer(headerSize);
	textInput.seekg(0);
	textInput.read(reinterpret_cast<char*>(headerBuffer.data()), headerSize);

	PakHeader* textHeader = new PakHeader();
	memcpy(textHeader, reinterpret_cast<PakHeader*>(headerBuffer.data()), sizeof(PakHeader));

	// load directory
	uint32_t dirSize = textHeader->dirCount * sizeof(Entry);
	std::vector<unsigned char*> dirBuffer(dirSize);
	textInput.seekg(sizeof(PakHeader));
	textInput.read(reinterpret_cast<char*>(dirBuffer.data()), dirSize);

	std::vector<Entry> textEntries(textHeader->dirCount);
	memcpy(textEntries.data(), dirBuffer.data(), dirBuffer.size());

	std::unordered_map<uint32_t, FfontInfo*> textMap(textEntries.size());
	for (Entry entry : textEntries) {
		uint32_t size = entry.size;
		std::vector<uint8_t> buffer(size);
		textInput.seekg(entry.offset);
		textInput.read(reinterpret_cast<char*>(buffer.data()), size);


		FfontInfo* fontInfo = new FfontInfo();

		// first 2 bytes are text height, second 2 are dropHeight
		std::memcpy(&fontInfo->textHeight, buffer.data(), sizeof(uint16_t));
		std::memcpy(&fontInfo->dropHeight, buffer.data() + 2, sizeof(uint16_t));

		// then the list of rects
		size_t remainingSize = (buffer.size() - sizeof(uint16_t) * 2) / sizeof(Rect);
		fontInfo->letterRect.resize(remainingSize);
		std::memcpy(fontInfo->letterRect.data(), buffer.data() + sizeof(uint16_t) * 2, remainingSize * sizeof(Rect));

		std::cout << "textHeight: " << fontInfo->textHeight << ", dropHeight: " << fontInfo->dropHeight << "\n";
		for (int i = 0; i < fontInfo->letterRect.size(); i++)
			std::cout << char(i) << ": " << fontInfo->letterRect[i] << "\n";

		textMap.insert({ entry.hashId, fontInfo });
	}
}

std::string ReadFile(const std::string& path) {
	std::ifstream input(path, std::ios::binary);
	if (!input.is_open()) {
		std::cout << "Failed to open: " << path << "\n";
		return "";
	}

	std::stringstream buffer;
	buffer << input.rdbuf();
	return buffer.str();
}

void InputShaders(const std::string& pakPath) {
	std::ifstream shaderInput(pakPath, std::ios::binary);
	if (!shaderInput.is_open()) {
		std::cout << "failed to open: " << pakPath << "\n";
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

	std::unordered_map<uint32_t, std::string> shaderMap(shaderEntries.size());
	for (Entry entry : shaderEntries) {
		uint32_t size = entry.size;
		std::vector<uint8_t> buffer(size);
		shaderInput.seekg(entry.offset);
		shaderInput.read(reinterpret_cast<char*>(buffer.data()), size);

		std::string shaderData(buffer.begin(), buffer.end());
		shaderMap.insert({ entry.hashId, shaderData });
	}
}

void OutputShaders(const std::string& pakPath, const std::string& folderPath, std::vector<std::string>& shaderPaths) {
	std::vector<Entry> directory;
	std::vector<unsigned char> bytesList;

	for (auto shaderPath : shaderPaths) {
		// load the shader
		std::string shader = ReadFile(folderPath + "/" + shaderPath);

		// add to directory
		directory.push_back(Entry(
			Hash(shaderPath),
			bytesList.size(),
			shader.size()
		));

		// add bytes to list
		bytesList.insert(bytesList.end(), shader.begin(), shader.end());
	}

	// reupdate the offsets of the image data to account for the offset and size of the directory
	uint32_t offset = sizeof(PakHeader) + directory.size() * sizeof(Entry);
	for (Entry& entry : directory)
		entry.offset += offset;

	// store to file
	std::ofstream output(pakPath, std::ios::binary);
	if (!output.is_open()) {
		std::cout << "Failed to open: " << pakPath << "\n";
		return;
	}

	// output header
	PakHeader header = PakHeader(0, sizeof(PakHeader), directory.size());
	unsigned char* headerBytes = reinterpret_cast<unsigned char*>(&header);
	size_t headerSize = sizeof(PakHeader);
	for (size_t i = 0; i < headerSize; i++)
		output << headerBytes[i];

	// output directory
	unsigned char* directoryBytes = reinterpret_cast<unsigned char*>(directory.data());
	size_t directorySize = directory.size() * sizeof(Entry);
	for (size_t i = 0; i < directorySize; i++)
		output << directoryBytes[i];

	// output image data
	//for (size_t i = 0; i < bytesList.size(); i++)
		//output << bytesList[i];
	output.write(reinterpret_cast<const char*>(bytesList.data()), bytesList.size());

	output.close();

	// debug
	std::cout << "output header: " << header << "\n";
	for (int i = 0; i < directory.size(); i++)
		std::cout << "output directory[" << i << "]: " << directory[i] << "\n";
}

void ParseImageFile(const char* path) {
	std::ifstream colFile(path);
	if (colFile.is_open()) {
		std::string line;
		while (colFile.good()) {
			// get the line
			std::getline(colFile, line);
			if (line == "")
				continue;

			line = toLower(line);

			// delimiter it
			std::vector<std::string> delimLine;
			std::string subData;
			std::istringstream stream(line);
			while (std::getline(stream, subData, '|'))
				delimLine.push_back(subData);


			std::string path = delimLine[0];
			unsigned int keepData = 0;
			if (delimLine.size() > 1) {
				if (delimLine[1] == "a")
					keepData = GL_ALPHA;
				else if (delimLine[1] == "rgba")
					keepData = GL_RGBA;
				else if (delimLine[1] == "b") // a binding texture (not bindless)
					keepData = GL_TEXTURE_BINDING_2D;
			}

			imageData.insert({ delimLine[0], keepData });
		}
	}
	colFile.close();
}

void Input(const char* pakPath) {
	std::ifstream input(pakPath, std::ios::binary);
	if (!input.is_open()) {
		std::cout << "failed to open: " << pakPath << "\n";
		return;
	}

	std::cout << "size of entry: " << sizeof(TextureEntry) << "\n";
		
	std::stringstream buffer;
	buffer << input.rdbuf();

	// load header
	std::string headerSubstr = buffer.str().substr(0, sizeof(PakHeader));
	PakHeader* header = reinterpret_cast<PakHeader*>(headerSubstr.data());
	std::cout << "input header: " << *header << "\n";

	// load directory
	std::string entrySubStr = buffer.str().substr(sizeof(PakHeader), header->dirCount * sizeof(TextureEntry));
	std::vector<TextureEntry> textureEntries(header->dirCount);
	memcpy(textureEntries.data(), entrySubStr.c_str(), entrySubStr.size());

	for (int i = 0; i < textureEntries.size(); i++) {
		std::cout << "input directory[" << i << "]: " << textureEntries[i] << "\n";

		std::string imgData = buffer.str().substr(textureEntries[i].offset, textureEntries[i].size);
		unsigned char* bytes = reinterpret_cast<unsigned char*>(imgData.data());
	}
}

void Output(const char* pakPath, const char* rootDir, const std::unordered_map<std::string, unsigned int>& paths) {
	std::vector<TextureEntry> directory;
	std::vector<unsigned char> bytesList;

	for (auto data : paths) {
		// load the image
		int w, h, nChannels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* bytes = stbi_load((rootDir + data.first).c_str(), &w, &h, &nChannels, NULL);
		if (!bytes) {
			std::cout << "Bytes null for \"" << data.first << "\"\n";
			abort();
		}

		size_t bytesSize = w * h * nChannels;

		// add to directory
		directory.push_back(TextureEntry(
			Hash(data.first),
			bytesList.size(),
			bytesSize,
			w,
			h,
			nChannels,
			data.second
		));

		// add bytes to list
		bytesList.insert(bytesList.end(), bytes, bytes + bytesSize);

		// bytes copied to list, free bytes
		stbi_image_free(bytes);
	}

	// reupdate the offsets of the image data to account for the offset and size of the directory
	uint32_t offset = sizeof(PakHeader) + directory.size() * sizeof(TextureEntry);
	for (TextureEntry& entry : directory)
		entry.offset += offset;

	// store to file
	std::ofstream output(pakPath, std::ios::binary);
	if (!output.is_open()) {
		std::cout << "failed to open: " << pakPath << "\n";
		return;
	}

	// output header
	PakHeader header = PakHeader(0, sizeof(PakHeader), directory.size());
	unsigned char* headerBytes = reinterpret_cast<unsigned char*>(&header);
	size_t headerSize = sizeof(PakHeader);
	for (size_t i = 0; i < headerSize; i++)
		output << headerBytes[i];

	// output directory
	unsigned char* directoryBytes = reinterpret_cast<unsigned char*>(directory.data());
	size_t directorySize = directory.size() * sizeof(TextureEntry);
	for (size_t i = 0; i < directorySize; i++)
		output << directoryBytes[i];

	// output image data
	//for (size_t i = 0; i < bytesList.size(); i++)
		//output << bytesList[i];
	output.write(reinterpret_cast<const char*>(bytesList.data()), bytesList.size());

	output.close();

	return;

	// debug
	std::cout << "output header: " << header << "\n";
	//for (int i = 0; i < directory.size(); i++) {
		//auto it = imageData.begin();
		//std::cout << "output directory[" << i << "] (\"" <<  << "\"): " << directory[i] << "\n";
	//}
	uint32_t index = 0;
	for (auto it = imageData.begin(); it != imageData.end(); ++it) {
		std::cout << "output directory[" << index << "] (\"" << it->first << "\"): " << directory[index] << "\n";
		++index;
	}
}

void processEntry(const std::string& folderPath, const std::filesystem::directory_entry& entry, std::vector<std::string>& paths, std::vector<std::string> extensions, bool recursive, bool removeInputFolderPath) {
	if (!entry.is_regular_file())
		return;

	const std::filesystem::path& filePath = entry.path();
	std::string ext = filePath.extension().string();

	bool hasExtension = false;
	for (std::string& extension : extensions) {
		if (ext == extension) {
			hasExtension = true;
			break;
		}
	}

	if (hasExtension || extensions.empty()) {
		if (removeInputFolderPath) {
			std::string newPath(filePath.string());
			newPath.erase(0, folderPath.size() + 1); // folderPath + '\\'
			paths.push_back(newPath);
		} else
			paths.push_back(filePath.string());
	}
}

std::vector<std::string> getFilesInPath(const std::string& folderPath, std::vector<std::string> extensions, bool recursive, bool removeInputFolderPath) {
	if (!std::filesystem::exists(folderPath) || !std::filesystem::is_directory(folderPath)) {
		std::cerr << "Invalid directory\n";
		return {};
	}

	std::vector<std::string> paths;

	if (recursive) {
		for (const auto& entry : std::filesystem::recursive_directory_iterator(folderPath)) {
			processEntry(folderPath, entry, paths, extensions, recursive, removeInputFolderPath);
		}
	} else {
		for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
			processEntry(folderPath, entry, paths, extensions, recursive, removeInputFolderPath);
		}
	}

	return paths;
}

uint32_t Hash(const std::string& str) {
	const char* c = str.c_str();

	uint32_t hash = 2166136261u; // offset basis
	while (*c)
	{
		hash ^= (uint8_t)(*c);
		hash *= 16777619u;       // prime
		++c;
	}

	auto it = hashMap.find(hash);
	if (it != hashMap.end() && it->second != str) { // make sure its not the same string value
		std::cout << "Hash value (" << hash << ") already used by \"" << it->second << "\"\n";
		std::cout << "stopping\n";
	} else
		hashMap.insert({ hash, str });

	return hash;
}

std::string toLower(std::string str) {
	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
		return std::tolower(c);
		});
	return str;
}

std::pair<uint16_t, uint16_t> parseTextFile(std::string path, std::vector<Rect>& rects) {
	std::ifstream file(path);
	if (!file) {
		std::cout << "no font file\n";
		return { 0, 0 };
	}

	rects.resize(126);

	uint16_t textHeight = 0;
	uint16_t dropHeight = 0;

	std::string line;
	std::string delimiter = " ";
	std::string delimiter1 = ":";

	int lineNum = 0;
	int xOffset = 0;

	// parse header row
	std::string headerLine;
	std::getline(file, headerLine);
	std::vector<std::string> delimLine;
	std::istringstream stream(headerLine);
	while (std::getline(stream, line, ' '))
		delimLine.push_back(line);

	if (delimLine.size() > 0)
		textHeight = std::stoi(delimLine[0]);
	if (delimLine.size() > 1)
		dropHeight = std::stoi(delimLine[1]);

	// parse body
	while (std::getline(file, line)) {
		// get each word in line
		size_t pos = 0;
		std::string token;
		while ((pos = line.find(delimiter)) != std::string::npos || line != "") {
			// includes the last item in list
			if (line.find(delimiter) == std::string::npos)
				pos = line.size();

			char token1;
			size_t pos1 = 0;
			token = line.substr(0, pos);
			if (token.find("32:") != std::string::npos) {
				line.erase(0, pos + delimiter.length());
				if ((pos1 = token.find(":")) != std::string::npos) {
					token.erase(0, pos1 + delimiter1.length());
					rects[32].x = float(xOffset);
					rects[32].y = float(lineNum);
				}

				token1 = 32;
			} else if (token.find("58:") != std::string::npos) {
				line.erase(0, pos + delimiter.length());
				if ((pos1 = token.find(":")) != std::string::npos) {
					token.erase(0, pos1 + delimiter1.length());
					rects[58].x = float(xOffset);
					rects[58].y = float(lineNum);
				}

				token1 = 58;
			} else {
				line.erase(0, pos + delimiter.length());
				while ((pos1 = token.find(":")) != std::string::npos) {
					token1 = *token.substr(0, pos1).c_str();
					token.erase(0, pos1 + delimiter1.length());
					rects[token1].x = float(xOffset);
					rects[token1].y = float(lineNum);
				}
			}

			xOffset += std::stoi(token);
			rects[token1].w = std::stof(token);
			rects[token1].h = textHeight;
		}

		xOffset = 0;
		lineNum += textHeight;
	}

	return { textHeight, dropHeight };
}

void removeSpaces(std::string& str) {
	std::string newStr;
	for (int i = 0; i < str.size(); i++) {
		if (str[i] != ' ') {
			std::string stringChar(1, str[i]);
			newStr.append(stringChar);
		}
	}
	str = newStr;
}

std::string getIdentifier(std::string str) {
	std::string identifier;
	for (int i = 0; i < str.size(); i++) {
		if (str[i] != '(') {
			std::string stringChar(1, str[i]);
			identifier.append(stringChar);
		} else {
			return identifier;
		}
	}

	return identifier;
}

std::unordered_map<uint32_t, std::vector<Fcollision>> getCollisionObjects(const std::string& colPath) {
	std::unordered_map<uint32_t, std::vector<Fcollision>> collisionStorage;
	std::vector<std::vector<vector>> coords;

	std::ifstream colFile(colPath);

	int lineNum = 0;
	std::string currWorldName = "";
	if (colFile.is_open()) {
		while (colFile.good()) {
			std::string line;
			std::getline(colFile, line);

			removeSpaces(line);

			if (line[0] == '!') {
				if (currWorldName != "") // returns if passed the world
					return {};

				// then set the world name parameter
				line.erase(line.begin());
				currWorldName = line;

				// only puts collision in the list if its part of that world
			} else if (line[0] != '/' && line != "" && currWorldName != "") {

				std::string identifier = getIdentifier(line);

				line.erase(0, identifier.size());

				std::vector<std::string> stringList;
				for (int i = 0; i < line.size(); i++) {
					// if char == "(" then break into a string
					if (line[i] == '(') {
						std::string stringChar(1, line[i]);
						stringList.push_back(stringChar);
					} else {
						std::string stringChar(1, line[i]);
						stringList[stringList.size() - 1].append(stringChar);
					}
				}

				if ((int)coords.size() - 1 < lineNum) {
					std::vector<vector> charString(stringList.size());
					coords.push_back(charString);
				}

				for (int i = 0; i < stringList.size(); i++) {
					sscanf_s(stringList[i].c_str(), "(%f, %f)", &coords[lineNum][i].x, &coords[lineNum][i].y);
				}

				vector multiplier = vector{ 1, -1 };
				//vector offset = { 500, 500 };
				for (int i = 0; i < coords[lineNum].size(); i++)
					coords[lineNum][i] = coords[lineNum][i] * multiplier;// +offset;

				collisionStorage[Hash(currWorldName)].push_back(Fcollision(coords[lineNum], identifier[0]));

				lineNum++;
			}
		}
	}

	return collisionStorage;
}