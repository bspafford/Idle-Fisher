#include "textureManager.h"
#include "main.h"

#include <fstream>
#include <sstream>

#include "debugger.h"

textureManager::textureManager() {
	std::ifstream colFile("./data/png_files.txt");
	if (colFile.is_open()) {
		std::string line;
		while (colFile.good()) {
			std::getline(colFile, line);
			line = math::toLower(line);
			loadTexture(line);
		}
	}
	colFile.close();
}

void textureManager::Deconstructor() {
	textureMap.clear();
}

textureStruct* textureManager::loadTexture(std::string path, bool loadSurface) {
	int w, h, nChannels;
	unsigned char* bytes = stbi_load(path.c_str(), &w, &h, &nChannels, NULL);
	std::unique_ptr<textureStruct> texStruct = std::make_unique<textureStruct>(bytes, loadSurface, w, h, nChannels);
	textureMap[path] = std::move(texStruct);
	return textureMap[path].get();
}

textureStruct* textureManager::getTexture(std::string name) {
	// to lower
	name = math::toLower(name);

	if (textureMap.find(name) != textureMap.end())
		return textureMap[name].get();
	else {
		std::cout << "Image path not in textureMap but loading anyways: " << name << "\n";
		return loadTexture(name);
	}
}