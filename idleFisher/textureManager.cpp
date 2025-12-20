#include "textureManager.h"
#include "main.h"
#include "shaderClass.h"
#include "FBO.h"

#include <fstream>
#include <sstream>

#include "debugger.h"

textureStruct::textureStruct(const std::string& path, bool useAlpha) {
	this->useAlpha = useAlpha;

	stbi_set_flip_vertically_on_load(true);
	unsigned char* bytes = stbi_load(path.c_str(), &w, &h, &nChannels, NULL);
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
	}

	stbi_image_free(bytes);
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
			std::getline(colFile, line);
			line = math::toLower(line);
			loadTexture(line);
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

textureStruct* textureManager::loadTexture(std::string path, bool loadSurface) {
	textureMap[path] = std::make_unique<textureStruct>(path, loadSurface);
	return textureMap[path].get();
}

textureStruct* textureManager::getTexture(std::string name) {
	// to lower
	name = math::toLower(name);

	if (textureMap.find(name) != textureMap.end())
		return textureMap[name].get();
	else { // backup
		std::cout << "Image path not in textureMap but loading anyways: " << name << "\n";
		return loadTexture(name);
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
	UploadGPUData();
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