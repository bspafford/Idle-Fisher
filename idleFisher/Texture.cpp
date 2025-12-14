#include"Texture.h"
#include "main.h"

#include "debugger.h"

Texture::Texture(const char* image) {
	if (usedSlots.size() == 0) {
		GLint maxCombined;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxCombined);
		usedSlots.resize(maxCombined, false);
		usedSlots[0] = true;
	}

	if (image == "")
		return;

	GLint maxFragmentTexUnits;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxFragmentTexUnits);

	// Stores the width, height, and the number of color channels of the image
	int widthImg, heightImg, numColCh;

	// Reads the image from a file and stores it in bytes
	unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);
	if (!bytes)
		return;

	// Generates an OpenGL texture object
	glGenTextures(1, &ID);
	// Assigns the texture to a Texture Unit
	GLuint slot = takeOpenSlot();
	if (slot == -1) {
		std::cout << "slots are full!\n";
		abort();
	}

	glActiveTexture(GL_TEXTURE0 + slot);
	unit = slot;
	glBindTexture(GL_TEXTURE_2D, ID);

	// Configures the type of algorithm that is used to make the image smaller or bigger
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Configures the way the texture repeats (if it does at all)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	// Check what type of color channels the texture has and load it accordingly

	if (numColCh == 4)
		glTexImage2D
		(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			widthImg,
			heightImg,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			bytes
		);
	else if (numColCh == 3)
		glTexImage2D
		(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			widthImg,
			heightImg,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			bytes
		);
	else if (numColCh == 1)
		glTexImage2D
		(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			widthImg,
			heightImg,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			bytes
		);
	else
		throw std::invalid_argument("Automatic Texture type recognition failed");

	// Generates MipMaps
	glGenerateMipmap(GL_TEXTURE_2D);

	// Deletes the image data as it is already in the OpenGL Texture object
	stbi_image_free(bytes);

	// Unbinds the OpenGL Texture object so that it can't accidentally be modified
	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture() {
	Unbind();
	Delete();
}

void Texture::texUnit(Shader* shader, const char* uniform) {
	// Shader needs to be activated before changing the value of a uniform
	shader->Activate();
	// Gets the location of the uniform
	GLuint texUni = glGetUniformLocation(shader->ID, uniform);
	// Sets the value of the uniform
	glUniform1i(texUni, unit);
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

void Texture::bindTextureToShader(Shader* shaderProgram, const char* path, const char* uniform) {
	bindTextureToShader(std::vector<Shader*>{ shaderProgram }, path, uniform);
}

void Texture::bindTextureToShader(std::vector<Shader*> shaderPrograms, const char* path, const char* uniform) {
	if (path == nullptr || path == "") {
		for (Shader* shaderProgram : shaderPrograms) {
			shaderProgram->Activate();
			GLuint texUni = glGetUniformLocation(shaderProgram->ID, uniform);
			glUniform1i(texUni, 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
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
	if (Main::IsRunning())
		usedSlots[slot] = false;
}