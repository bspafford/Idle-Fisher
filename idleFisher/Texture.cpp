#include"Texture.h"
#include "main.h"
#include "textureManager.h"

#include "debugger.h"

Texture::Texture(const char* imgPath) {
	if (imgPath == "")
		return;

	int widthImg, heightImg, numColCh;
	unsigned char* bytes = stbi_load(imgPath, &widthImg, &heightImg, &numColCh, 0);
	if (!bytes)
		return;

	glCreateTextures(GL_TEXTURE_2D, 1, &ID);

	glActiveTexture(GL_TEXTURE0);
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
}

Texture::Texture(vector size) {
	glCreateTextures(GL_TEXTURE_2D, 1, &ID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ID);

	glTextureStorage2D(ID, 1, GL_RGBA8, size.x, size.y);
	handle = glGetTextureSamplerHandleARB(ID, textureManager::GetSamplerID());
	glMakeTextureHandleResidentARB(handle);
}

Texture::Texture(const char* imgPath, bool binding) {
	if (usedSlots.size() == 0) {
		GLint maxCombined;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxCombined);
		usedSlots.resize(maxCombined, false);
		usedSlots[0] = true;
	}

	if (imgPath == "")
		return;

	int widthImg, heightImg, numColCh;
	unsigned char* bytes = stbi_load(imgPath, &widthImg, &heightImg, &numColCh, 0);
	if (!bytes)
		return;

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

Texture::~Texture() {
	if (handle)
		glMakeTextureHandleNonResidentARB(handle);
	if (ID)
		glDeleteTextures(1, &ID);

	Unbind();
	Delete();
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