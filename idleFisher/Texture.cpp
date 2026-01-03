#include"Texture.h"
#include "main.h"
#include "textureManager.h"
#include "GPULoadCollector.h"

#include "debugger.h"

Texture::Texture(const char* imgPath) {
	instances.push_back(this);

	if (imgPath == "")
		return;

	bytes = stbi_load(imgPath, &widthImg, &heightImg, &numColCh, 0);
	if (!bytes)
		return;

	functionIdx = 0;
	size = { static_cast<float>(widthImg), static_cast<float>(heightImg) };
	this->imgPath = imgPath;
	GPULoadCollector::add(this);
}

Texture::Texture(vector size) {
	instances.push_back(this);

	if (size == vector{ 0.f, 0.f })
		size = stuff::screenSize;
	else
		this->size = size;

	functionIdx = 1;
	GPULoadCollector::add(this);
}

Texture::Texture(const char* imgPath, bool binding) {
	instances.push_back(this);

	functionIdx = 2;
	this->imgPath = imgPath;
	GPULoadCollector::add(this);
}

void Texture::LoadGPU() {
	if (functionIdx == 0) {
		glCreateTextures(GL_TEXTURE_2D, 1, &ID);

		glBindTexture(GL_TEXTURE_2D, ID);

		if (numColCh == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
		else if (numColCh == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes);
		else if (numColCh == 1)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, GL_RED, GL_UNSIGNED_BYTE, bytes);
		else
			throw std::invalid_argument("Automatic Texture type recognition failed");

		stbi_image_free(bytes);
		glBindTexture(GL_TEXTURE_2D, 0);
	} else if (functionIdx == 1) {
		glCreateTextures(GL_TEXTURE_2D, 1, &ID);
		glBindTexture(GL_TEXTURE_2D, ID);

		vector tempSize = size;
		if (size == vector{ 0.f, 0.f })
			tempSize = stuff::screenSize;

		glTextureStorage2D(ID, 1, GL_RGBA8, tempSize.x, tempSize.y);
		handle = glGetTextureSamplerHandleARB(ID, textureManager::GetSamplerID());
		glMakeTextureHandleResidentARB(handle);
		glBindTexture(GL_TEXTURE_2D, 0);
	} else if (functionIdx == 2) {
		if (usedSlots.size() == 0) {
			GLint maxCombined;
			glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxCombined);
			usedSlots.resize(maxCombined, false);
			usedSlots[0] = true;
		}

		if (imgPath == "")
			return;

		unsigned char* bytes = stbi_load(imgPath, &widthImg, &heightImg, &numColCh, 0);
		if (!bytes)
			return;

		size = { static_cast<float>(widthImg), static_cast<float>(heightImg) };

		glCreateTextures(GL_TEXTURE_2D, 1, &ID);
		GLuint slot = takeOpenSlot();
		if (slot == -1) {
			std::cout << "slots are full!\n";
			abort();
		}

		unit = slot;
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, ID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		if (numColCh == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
		else if (numColCh == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes);
		else if (numColCh == 1)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, GL_RED, GL_UNSIGNED_BYTE, bytes);
		else
			throw std::invalid_argument("Automatic Texture type recognition failed");

		stbi_image_free(bytes);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

Texture::~Texture() {
	auto it = std::find(instances.begin(), instances.end(), this);
	if (it != instances.end())
		instances.erase(it);

	Unbind();
	Delete();

	GPULoadCollector::remove(this);
}

void Texture::Resize(vector size) {
	this->size = size;

	Delete();

	glCreateTextures(GL_TEXTURE_2D, 1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);

	glTextureStorage2D(ID, 1, GL_RGBA8, size.x, size.y);
	handle = glGetTextureSamplerHandleARB(ID, textureManager::GetSamplerID());
	glMakeTextureHandleResidentARB(handle);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::texUnit(Shader* shader, const char* uniform) {
	// Shader needs to be activated before changing the value of a uniform
	shader->Activate();
	shader->setInt(uniform, unit);
}

void Texture::Bind() {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::Unbind() {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, 0);
	releaseSlot(unit);
}

void Texture::Delete() {
	glMakeTextureHandleNonResidentARB(handle);
	glDeleteTextures(1, &ID);
}

GLuint64 Texture::GetID() {
	return ID;
}

GLuint64 Texture::GetHandle() {
	return handle;
}

void Texture::bindTextureToShader(Shader* shaderProgram, const char* path, const char* uniform) {
	bindTextureToShader(std::vector<Shader*>{ shaderProgram }, path, uniform);
}

void Texture::bindTextureToShader(std::vector<Shader*> shaderPrograms, const char* path, const char* uniform) {
	if (path == nullptr || path == "") {
		for (Shader* shaderProgram : shaderPrograms) {
			shaderProgram->Activate();
			shaderProgram->setInt(uniform, 0);
			glActiveTexture(GL_TEXTURE0);
		}
	} else {
		std::unique_ptr<Texture> texture = std::make_unique<Texture>(path, true);
		for (Shader* shaderProgram : shaderPrograms) {
			texture->texUnit(shaderProgram, uniform);
			texture->Bind();
		}
		textureCache.push_back(std::move(texture));
	}
}

void Texture::deleteCache() {
	for (std::unique_ptr<Texture>& texture : textureCache)
		texture->Delete();
	textureCache.clear();
}

void Texture::ResizeAllTextures() {
	for (Texture* texture : instances) {
		if (texture->size == vector{ 0.f, 0.f })
			texture->Resize(stuff::screenSize);
	}
}

GLuint Texture::takeOpenSlot() {
	for (int i = 0; i < usedSlots.size(); i++) {
		if (!usedSlots[i]) {
			usedSlots[i] = true;
			return i;
		}
	}
	return -1;
}

void Texture::releaseSlot(GLuint slot) {
	if (slot == 0 || usedSlots.size() <= slot)
		return;

	if (Main::IsRunning())
		usedSlots[slot] = false;
}