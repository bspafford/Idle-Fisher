#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <mutex>

#include "textureManager.h"

struct PakHeader {
	uint32_t version;
	uint32_t dirOffset;
	uint32_t dirCount;

	PakHeader() {}
	PakHeader(uint32_t _version, uint32_t _dirOffset, uint32_t _dirCount) : version(_version), dirOffset(_dirOffset), dirCount(_dirCount) {}
};

struct TextureEntry {
	uint32_t hashId;
	uint32_t offset;
	uint32_t size;
	uint32_t width;
	uint32_t height;
	uint16_t format; // RGBA8, indexed, etc
	uint16_t flags; // keep CPU copy, alpha mask, etc

	TextureEntry() {}
	TextureEntry(uint32_t _hashId, uint32_t _offset, uint32_t _size, uint32_t _width, uint32_t _height, uint16_t _format, uint16_t _flags)
		: hashId(_hashId), offset(_offset), size(_size), width(_width), height(_height), format(_format), flags(_flags) {}
};

struct ShaderEntry {
	uint32_t hashId;
	uint32_t offset;
	uint32_t size;

	ShaderEntry() {}
	ShaderEntry(uint32_t _hashId, uint32_t _offset, uint32_t _size) : hashId(_hashId), offset(_offset), size(_size) {}
};

class PakReader {
public:
	// Images
	// parses just the header and directory of the .pak file
	static void ParseImages(const std::string& path);
	// loads a single texture
	static std::vector<uint8_t> LoadTexture(const TextureEntry& entry);
	// loads all the image data to the given map
	static void LoadTextures(std::unordered_map<uint32_t, std::unique_ptr<textureStruct>>& textureMap);
	static TextureEntry GetImgEntry(uint32_t hashedId);

	// Shaders
	static void ParseShaders(const std::string& path);
	static void ClearShaderData();
	static std::string GetShader(const std::string& path);

	// Hash FNV1a
	static uint32_t Hash(const std::string& str);

private:
	static inline std::mutex mutex;
	
	// Images
	static inline std::string imgPath;
	static inline std::ifstream imgInput;
	static inline PakHeader* imgHeader;
	static inline std::unordered_map<uint32_t, TextureEntry> imgDirectory;

	// Shaders
	static inline std::string shaderPath;
	static inline std::ifstream shaderInput;
	static inline PakHeader* shaderHeader;
	static inline std::unordered_map<uint32_t, std::string> shaderMap;
};