#include"Texture.h"
#include "main.h"
#include "textureManager.h"
#include "GPULoadCollector.h"

#include "debugger.h"

Texture::Texture(vector size) {
	this->size = size;

	functionIdx = 0;
	GPULoadCollector::add(this);
}

Texture::Texture(const std::string& imgPath) {
	if (imgPath == "")
		return;

	texData = textureManager::getTexture(imgPath);
	if (!texData)
		return;
	if (texData->handle) {
		std::cerr << "Trying to make bindable texture that has a bindless handle: \"" << imgPath << "\"\n";
		abort();
	}

	size = vector(texData->w, texData->h);

	functionIdx = 1;
	GPULoadCollector::add(this);
}

void Texture::LoadGPU() {
	if (functionIdx == 0) {
		glCreateTextures(GL_TEXTURE_2D, 1, &id);
		glTextureStorage2D(id, 1, GL_RGBA8, size.x, size.y);
		handle = glGetTextureSamplerHandleARB(id, textureManager::GetSamplerID());
		glMakeTextureHandleResidentARB(handle);
	} else if (functionIdx == 1) {
		if (usedSlots.size() == 0) {
			GLint maxCombined;
			glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxCombined);
			usedSlots.resize(maxCombined, false);
			usedSlots[0] = true;
		}

		unit = takeOpenSlot();
		if (unit == -1) {
			std::cout << "slots are full!\n";
			abort();
		}

		id = texData->id;
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, id);

		glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

Texture::~Texture() {
	Unbind();
	Delete();

	GPULoadCollector::remove(this);
}

void Texture::Resize(vector size) {
	if (this->size == size) // if same size, return
		return;

	this->size = size;

	Delete();

	glCreateTextures(GL_TEXTURE_2D, 1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTextureStorage2D(id, 1, GL_RGBA8, size.x, size.y);

	handle = glGetTextureSamplerHandleARB(id, textureManager::GetSamplerID());
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
	glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::Unbind() {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, 0);
	releaseSlot(unit);
}

void Texture::Delete() {
	glMakeTextureHandleNonResidentARB(handle);
	glDeleteTextures(1, &id);
}

GLuint64 Texture::GetID() {
	return id;
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
		std::unique_ptr<Texture> texture = std::make_unique<Texture>(path);
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