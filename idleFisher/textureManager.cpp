#include "textureManager.h"
#include "main.h"
#include "shaderClass.h"
#include "GPULoadCollector.h"
#include "PakReader.h"

#include <fstream>
#include <sstream>

#include "debugger.h"

textureStruct::textureStruct(std::vector<uint8_t>& _bytes, int w, int h, int nChannels, uint16_t keepData) : bytes(std::move(_bytes)) {
	this->w = w;
	this->h = h;
	this->nChannels = nChannels;
	keptData = keepData;

	GPULoadCollector::add(this);
}

textureStruct::textureStruct(uint32_t hashedId) {
	TextureEntry entry = PakReader::GetImgEntry(hashedId);
	bytes = PakReader::LoadTexture(entry);
	w = entry.width;
	h = entry.height;
	nChannels = entry.format;
	keptData = entry.flags;

	GPULoadCollector::add(this);
}

textureStruct::~textureStruct() {
	GPULoadCollector::remove(this);
}

void textureStruct::LoadGPU() {
	glCreateTextures(GL_TEXTURE_2D, 1, &id);

	if (!bytes.empty()) {
		GLenum format = 0;
		if (nChannels == 4)
			format = GL_RGBA;
		else if (nChannels == 3)
			format = GL_RGB;
		else {
			std::cerr << "Warning: the image is not truecolor; this may cause issues." << std::endl;
			abort();
		}

		glTextureStorage2D(id, 1, GL_RGBA8, w, h); // allocate immutable storage
		glTextureSubImage2D(id, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, bytes.data()); // upload texture data

		if (keptData != GL_TEXTURE_BINDING_2D) { // this texture needs to bind, used for water
			handle = glGetTextureSamplerHandleARB(id, textureManager::GetSamplerID());// glGetTextureHandleARB(ID);
			glMakeTextureHandleResidentARB(handle);
		}

		if (keptData == 0 || keptData == GL_TEXTURE_BINDING_2D) { // delete all pixel data
			bytes.clear();
		} else if (keptData == GL_ALPHA) { // only keep alpha
			// make alpha data list
			const size_t pixelCount = w * h;
			const size_t packedSize = (pixelCount + 7) / 8;

			std::vector<uint8_t> alphaBytes(packedSize, 0);
			const uint8_t* src = bytes.data();

			for (size_t i = 0; i < pixelCount; ++i, src += 4)
				alphaBytes[i >> 3] |= (*src != 0) << (i & 7); // read alpha channel

			bytes = std::move(alphaBytes); // replace old RGBA with packed alpha
		} // else don't delete anything
	}

	bytes.shrink_to_fit(); // bytes aren't going to change past this point, so i don't need reserved space
}

bool textureStruct::GetAlphaAtPos(vector pos) {
	// out of bounds check
	pos = pos.floor();
	if (pos.x < 0 || pos.y < 0 || pos.x >= w || pos.y >= h)
		return false;

	if (keptData == GL_ALPHA) { // alpha data kept
		int index = pos.y * w + pos.x;
		int byteIndex = index >> 3;
		int bitIndex = index & 7;
		return !bytes.empty() && (bytes[byteIndex] & (1 << bitIndex)) != 0;
	} else if (keptData == GL_RGBA) { // all data kept
		int index = pos.y * w + pos.x * 4;
		unsigned char a = bytes[index + 3];
		return a > 0;
	}

	return true; // no data kept, assume opaque
}

std::vector<uint8_t> textureStruct::GetFlippedBytes() {
	const uint32_t rowSize = w * nChannels;

	std::vector<uint8_t> cpyBytes = bytes;

	// Basic safety check
	if (cpyBytes.size() < static_cast<size_t>(rowSize) * h)
		return {};

	for (uint32_t y = 0; y < h / 2; ++y) {
		uint8_t* topRow = cpyBytes.data() + y * rowSize;
		uint8_t* bottomRow = cpyBytes.data() + (h - 1 - y) * rowSize;

		std::swap_ranges(topRow, topRow + rowSize, bottomRow);
	}

	return cpyBytes;
}

textureManager::textureManager() {
	std::lock_guard<std::recursive_mutex> lock(mutex);

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

	PakReader::ParseImages("data/images.pak");
}

void textureManager::LoadTextures() {
	std::lock_guard<std::recursive_mutex> lock(mutex);
	PakReader::LoadTextures(textureMap);
	areTexturesLoaded = true;
}

void textureManager::Deconstructor() {
	std::lock_guard<std::recursive_mutex> lock(mutex);

	for (auto& texture : textureMap) {
		glDeleteTextures(1, &texture.second->id);
		texture.second->bytes.clear();
	}

	textureMap.clear();
}

textureStruct* textureManager::loadTexture(uint32_t hashedId) {
	std::lock_guard<std::recursive_mutex> lock(mutex);

	if (Main::IsRunning()) { // wasn't in map, need to add it
		auto [it, inserted] = textureMap.emplace(hashedId, std::make_unique<textureStruct>(hashedId));
		return it->second.get();
	}
	
	return nullptr;
}

textureStruct* textureManager::getTexture(const std::string& name) {
	std::lock_guard<std::recursive_mutex> lock(mutex);

	if (name == "")
		return nullptr;

	// to lower
	std::string lower = math::toLower(name);

	uint32_t hashedId = PakReader::Hash(lower);
	auto it = textureMap.find(hashedId);
	if (it != textureMap.end())
		return it->second.get();
	else if (!areTexturesLoaded) {
		return loadTexture(hashedId);
	} else {
		std::cout << "\"" << name << "\" is not inside of textureMap\n";
		abort();
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