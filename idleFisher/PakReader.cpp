#include "PakReader.h"
#include "textureManager.h"

void PakReader::Parse(const std::string& _path) {
	std::lock_guard lock(mutex);

	path = _path;

	input = std::ifstream(path, std::ios::binary);
	if (!input.is_open()) {
		std::cout << "failed to open: " << path << "\n";
		return;
	}

	// load header
	uint32_t headerSize = sizeof(PakHeader);
	std::vector<unsigned char*> headerBuffer(headerSize);
	input.seekg(0);
	input.read(reinterpret_cast<char*>(headerBuffer.data()), headerSize);

	header = new PakHeader();
	memcpy(header, reinterpret_cast<PakHeader*>(headerBuffer.data()), sizeof(PakHeader));

	// load directory
	uint32_t dirSize = header->dirCount * sizeof(TextureEntry);
	std::vector<unsigned char*> dirBuffer(dirSize);
	input.seekg(sizeof(PakHeader));
	input.read(reinterpret_cast<char*>(dirBuffer.data()), dirSize);

	std::vector<TextureEntry> textureEntries(header->dirCount);
	memcpy(textureEntries.data(), dirBuffer.data(), dirBuffer.size());

	directory.reserve(textureEntries.size());
	for (TextureEntry entry : textureEntries) {
		directory.insert({ entry.hashId, std::move(entry) });
	}
}

std::vector<uint8_t> PakReader::LoadTexture(const TextureEntry& entry) {
	std::lock_guard lock(mutex);

	uint32_t size = entry.size;
	std::vector<uint8_t> buffer(size);
	input.seekg(entry.offset);
	input.read(reinterpret_cast<char*>(buffer.data()), size);
	return buffer;
}

void PakReader::LoadTextures(std::unordered_map<uint32_t, std::unique_ptr<textureStruct>>& textureMap) {
	std::lock_guard lock(mutex);
	
	for (auto& entry : directory) {
		auto it = textureMap.find(entry.second.hashId);
		if (it != textureMap.end())
			continue; // already in map

		uint32_t size = entry.second.size;
		std::vector<uint8_t> buffer(size);
		input.seekg(entry.second.offset);
		input.read(reinterpret_cast<char*>(buffer.data()), size);

		textureMap[entry.second.hashId] = std::make_unique<textureStruct>(
			buffer, entry.second.width, entry.second.height, entry.second.format, entry.second.flags
		);
	}

	// clean up
	path.clear();
	delete header;
	directory.clear();
	input.close();
}

TextureEntry PakReader::GetEntry(uint32_t hashedId) {
	std::lock_guard lock(mutex);
	return directory[hashedId];
}