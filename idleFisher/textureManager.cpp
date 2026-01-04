#include "textureManager.h"
#include "main.h"
#include "shaderClass.h"
#include "FBO.h"
#include "GPULoadCollector.h"

#include <fstream>
#include <sstream>

#include "debugger.h"

textureStruct::textureStruct(const std::string& path, GLuint keepData) {
	this->useAlpha = useAlpha;
	keptData = keepData;

	stbi_set_flip_vertically_on_load(true);
	bytes = stbi_load(path.c_str(), &w, &h, &nChannels, NULL);
	if (!bytes) {
		std::cout << "Filepath NOT Found: " << path << std::endl;
		return;
	}

	GPULoadCollector::add(this);
}

textureStruct::~textureStruct() {
	GPULoadCollector::remove(this);
}

void textureStruct::LoadGPU() {
	glCreateTextures(GL_TEXTURE_2D, 1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	if (bytes) {
		GLenum format = 0;
		if (nChannels == 4)
			format = GL_RGBA;
		else if (nChannels == 3)
			format = GL_RGB;
		else
			std::cerr << "Warning: the image is not truecolor; this may cause issues." << std::endl;

		glTextureStorage2D(id, 1, GL_RGBA8, w, h); // allocate immutable storage
		glTextureSubImage2D(id, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, bytes); // upload texture data

		handle = glGetTextureSamplerHandleARB(id, textureManager::GetSamplerID());// glGetTextureHandleARB(ID);
		glMakeTextureHandleResidentARB(handle);

		if (keptData == 0) { // delete all pixel data
			stbi_image_free(bytes);
			bytes = NULL;
		} else if (keptData == GL_R) { // only keep alpha
			// make alpha data list
			alphaBits.resize((w * h + 7) / 8, 0);
			const int pixelCount = w * h;
			if (nChannels == 4) {
				for (int i = 0; i < pixelCount; ++i) {
					uint8_t a = bytes[i * 4 + 3];
					alphaBits[i >> 3] |= (a > 0) << (i & 7);
				}
			}

			stbi_image_free(bytes);
			bytes = NULL;
		} // else don't delete anything
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

bool textureStruct::GetAlphaAtPos(vector pos) {
	// out of bounds check
	pos = pos.floor();
	if (pos.x < 0 || pos.y < 0 || pos.x >= w || pos.y >= h)
		return false;

	if (keptData == GL_R) { // alpha data kept
		int index = pos.y * w + pos.x;
		int byteIndex = index >> 3;
		int bitIndex = index & 7;
		return !alphaBits.empty() && (alphaBits[byteIndex] & (1 << bitIndex)) != 0;
	} else if (keptData == GL_RGBA) { // all data kept
		int index = pos.y * w + pos.x * 4;
		unsigned char* pixels = bytes;
		unsigned char a = pixels[index + 3];
		return a > 0;
	}

	return true; // no data kept, assume opaque
}

unsigned char* textureStruct::FlipBytesVertically() {
	int size = w * h * nChannels;
	unsigned char* newBytes = new unsigned char[size];
	std::memcpy(newBytes, bytes, size);
	int rowSize = w * nChannels;
	std::vector<unsigned char> tempRow(rowSize);
	for (int y = 0; y < h / 2; ++y) {
		unsigned char* top = newBytes + y * rowSize;
		unsigned char* bottom = newBytes + (h - 1 - y) * rowSize;
		// swap top and bottom
		std::memcpy(tempRow.data(), top, rowSize);
		std::memcpy(top, bottom, rowSize);
		std::memcpy(bottom, tempRow.data(), rowSize);
	}
	return newBytes;
}

textureManager::textureManager() {
	glCreateSamplers(1, &samplerID);
	glSamplerParameteri(samplerID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glSamplerParameteri(samplerID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameteri(samplerID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(samplerID, GL_TEXTURE_WRAP_T, GL_REPEAT);

	float vertices[] = {
		// positions       // texture coordinates
		0.0f, 1.0f,      0.0f, 0.0f,  // top-left
		0.0f, 0.0f,      0.0f, 1.0f,  // bottom-left
		1.0f, 0.0f,      1.0f, 1.0f,  // bottom-right
		1.0f, 1.0f,      1.0f, 0.0f   // top-right
	};

	unsigned int indices[] = {
		0, 1, 2,
		0, 2, 3
	};

	vao = std::make_unique<VAO>();
	vao->Bind();
	vbo = std::make_unique<VBO>(vertices, sizeof(vertices));
	ebo = std::make_unique<EBO>(indices, sizeof(indices));
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribDivisor(0, 0);
	glVertexAttribDivisor(1, 0);

	ParseImageFile();
}

void textureManager::ParseImageFile() {
	std::ifstream colFile("./data/png_files.txt");
	if (colFile.is_open()) {
		std::string line;
		while (colFile.good()) {
			// get the line
			std::getline(colFile, line);
			if (line == "")
				continue;

			line = math::toLower(line);

			// delimiter it
			std::vector<std::string> delimLine;
			std::string subData;
			std::istringstream stream(line);
			while (std::getline(stream, subData, '|'))
				delimLine.push_back(subData);


			std::string path = delimLine[0];
			GLuint keepData = 0;
			if (delimLine.size() > 1) {
				if (delimLine[1] == "a")
					keepData = GL_R;
				else if (delimLine[1] == "rgba")
					keepData = GL_RGBA;
			}

			imageData.insert({ delimLine[0], keepData });
		}
	}
	colFile.close();
}

void textureManager::LoadTextures() {
	for (auto& data : imageData)
		loadTexture(data.first, data.second);
	imageData.clear();
	areTexturesLoaded = true;
}

void textureManager::Deconstructor() {
	for (auto& texture : textureMap) {
		glDeleteTextures(1, &texture.second->id);
		if (texture.second->bytes)
			stbi_image_free(texture.second->bytes);
	}

	textureMap.clear();
}

textureStruct* textureManager::loadTexture(std::string path, GLuint keptData) {
	std::lock_guard<std::recursive_mutex> lock(mutex);
	auto it = textureMap.find(path);
	if (it != textureMap.end()) // already in map don't add again
		return it->second.get();

	if (Main::IsRunning()) {
		textureMap[path] = std::make_unique<textureStruct>(path, keptData);
		return textureMap[path].get();
	} else
		return nullptr;
}

textureStruct* textureManager::getTexture(std::string name) {
	std::lock_guard<std::recursive_mutex> lock(mutex);
	if (name == "")
		return nullptr;

	// to lower
	name = math::toLower(name);

	auto it = textureMap.find(name);
	if (it != textureMap.end())
		return it->second.get();
	else { // backup
		std::cout << "Image path not in textureMap but loading anyways: " << name << "\n";
		return loadTexture(name, imageData[name]);
	}
}

void textureManager::StartFrame() {
	gpuData.clear();
	prevShader = NULL;
	currShader = NULL;
}

void textureManager::DrawImage(Shader* shader, const vector& position, const vector& size, const Rect& source, const bool& useWorldPos, const glm::vec4& color, const uint64_t& texHandle) {
	currShader = shader;
	CheckShaders();
	gpuData.push_back(InstanceData(glm::vec2(position.x, position.y), glm::vec2(size.x, size.y), glm::vec4(source.x, source.y, source.w, source.h), useWorldPos, color, texHandle));
}

void textureManager::DrawRect(Shader* shader, const vector& position, const vector& size, const bool& useWorldPos, const glm::vec4& color) {
	currShader = shader;
	CheckShaders();
	gpuData.push_back(InstanceData(glm::vec2(position.x, position.y), glm::vec2(size.x, size.y), glm::vec4(0), useWorldPos, color, 0));
}

void textureManager::EndFrame() {
	ForceGPUUpload();
}

void textureManager::ForceGPUUpload() {
	prevShader = currShader;
	UploadGPUData();
}

void textureManager::CheckShaders() {
	if (prevShader != currShader) {
		UploadGPUData();
		prevShader = currShader;
	}
}

void textureManager::UploadGPUData() {
	if (gpuData.size() == 0)
		return;

	if (prevShader) {
		glCreateBuffers(1, &ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, gpuData.size() * sizeof(InstanceData), gpuData.data(), GL_DYNAMIC_DRAW);

		prevShader->Activate();
		vao->Bind();
		
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, gpuData.size());

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		glDeleteBuffers(1, &ssbo);

		gpuData.clear();

	}
	prevShader = currShader;
}

GLuint textureManager::GetSamplerID() {
	return samplerID;
}

bool textureManager::GetTexturesLoaded() {
	return areTexturesLoaded;
}

void textureManager::SetInterpMethod(int method) {
	// Step 0: Make old handles non-resident
	for (auto& texture : textureMap) {
		if (texture.second->handle != 0) {
			glBindTexture(GL_TEXTURE_2D, texture.second->id);
			glMakeTextureHandleNonResidentARB(texture.second->handle);
			texture.second->handle = 0;
		}
	}

	// Step 1: Delete old sampler (optional if you want to reuse)
	if (samplerID != 0) {
		glDeleteSamplers(1, &samplerID);
		samplerID = 0;
	}

	// Step 2: Create new sampler
	glGenSamplers(1, &samplerID);
	glSamplerParameteri(samplerID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(samplerID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(samplerID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(samplerID, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Step 3: Create new handles
	for (auto& texture : textureMap) {
		if (texture.second->id == 0) continue; // skip uninitialized textures

		// Make sure texture has storage
		GLint width, height;
		glBindTexture(GL_TEXTURE_2D, texture.second->id);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
		if (width == 0) continue; // incomplete texture

		texture.second->handle = glGetTextureSamplerHandleARB(texture.second->id, samplerID);

		if (texture.second->handle != 0)
			glMakeTextureHandleResidentARB(texture.second->handle);
	}
}