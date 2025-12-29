#include "textureManager.h"
#include "main.h"
#include "shaderClass.h"
#include "FBO.h"

#include <fstream>
#include <sstream>

#include "debugger.h"

textureStruct::textureStruct(const std::string& path, GLuint keepData) {
	this->useAlpha = useAlpha;

	stbi_set_flip_vertically_on_load(true);
	bytes = stbi_load(path.c_str(), &w, &h, &nChannels, NULL);
	if (!bytes) {
		std::cout << "Filepath NOT Found: " << path << std::endl;
		stbi_image_free(bytes);
		return;
	}

	GLuint ID;
	glCreateTextures(GL_TEXTURE_2D, 1, &ID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ID);

	if (bytes) {
		GLenum format = 0;
		if (nChannels == 4)
			format = GL_RGBA;
		else if (nChannels == 3)
			format = GL_RGB;
		else
			std::cerr << "Warning: the image is not truecolor; this may cause issues." << std::endl;

		glTextureStorage2D(ID, 1, GL_RGBA8, w, h); // allocate immutable storage
		glTextureSubImage2D(ID, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, bytes); // upload texture data

		handle = glGetTextureSamplerHandleARB(ID, textureManager::GetSamplerID());// glGetTextureHandleARB(ID);
		glMakeTextureHandleResidentARB(handle);

		keptData = keepData;
		if (keptData == 0) { // delete all pixel data
			stbi_image_free(bytes);
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
		} // else don't delete anything
	}
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
		return (alphaBits[byteIndex] & (1 << bitIndex)) != 0;
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
	glSamplerParameteri(samplerID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glSamplerParameteri(samplerID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	std::ifstream colFile("./data/png_files.txt");
	if (colFile.is_open()) {
		std::string line;
		while (colFile.good()) {
			// get the line
			std::getline(colFile, line);
			if (line == "")
				continue;

			if (line == "./images/cursor0.png|rgba")
				std::cout << "puase\n";

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

			loadTexture(delimLine[0], keepData);
		}
	}
	colFile.close();

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
}

void textureManager::Deconstructor() {
	textureMap.clear();
}

textureStruct* textureManager::loadTexture(std::string path, GLuint keptData) {
	textureMap[path] = std::make_unique<textureStruct>(path, keptData);
	return textureMap[path].get();
}

textureStruct* textureManager::getTexture(std::string name) {
	// to lower
	name = math::toLower(name);

	if (textureMap.find(name) != textureMap.end())
		return textureMap[name].get();
	else { // backup
		std::cout << "Image path not in textureMap but loading anyways: " << name << "\n";
		return loadTexture(name, 0);
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