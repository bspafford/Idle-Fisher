#include "Image.h"
#include "Input.h"
#include "stuff.h"
#include "textureManager.h"
#include <glm/gtx/rotate_vector.hpp>
#include "GPULoadCollector.h"

#include "debugger.h"

Image::Image(std::shared_ptr<Image> image, std::shared_ptr<Rect> source, vector loc, bool useWorldPos) {
	this->loc = loc;

	path = image->path;
	this->source = source;
	this->useWorldPos = useWorldPos;

	texType = GL_TEXTURE_2D;
	pixelType = GL_UNSIGNED_BYTE;

	texture = image->texture;

	if (!texture) {
		std::cout << "Filepath NOT Found: " << image->path << std::endl;
		return;
	}

	positions = getPositionsList();

	ogW = image->ogW;
	ogH = image->ogH;
	w = source->w;
	h = source->h;

	// Assigns the type of the texture ot the texture object
	type = texType;

	textureFormat = image->textureFormat;

	GPULoadCollector::add(this);
}

Image::Image(std::string image, vector loc, bool useWorldPos) {
	this->loc = loc;

	path = image;
	this->useWorldPos = useWorldPos;

	texType = GL_TEXTURE_2D;
	pixelType = GL_UNSIGNED_BYTE;
	textureStruct* img = textureManager::getTexture(path);

	if (!img) {
		std::cout << "Filepath NOT Found: " << path << std::endl;
		return;
	}

	positions = getPositionsList();

	if (img->texture) {
		texture = img->texture;
		ogW = img->w;
		ogH = img->h;
		w = img->w;
		h = img->h;
	} else {
		texture = NULL;
		ogW = 0;
		ogH = 0;
		w = 0;
		h = 0;
	}

	// Assigns the type of the texture ot the texture object
	type = texType;

	// Determine the pixel format of the surface.
	if (img->nChannels == 4) { // Image has an alpha channel
		textureFormat = GL_RGBA;
	} else if (img->nChannels == 3) { // No alpha channel
		textureFormat = GL_RGB;
	} else if (path != "" && path != "None") {
		std::cerr << "Warning: the image is not truecolor; this may cause issues." << std::endl;
	}

	GPULoadCollector::add(this);
}

Image::~Image() {
	if (currVAO)
		currVAO->Delete();
	if (currVBO)
		currVBO->Delete();
	if (currEBO)
		currEBO->Delete();
	glDeleteTextures(1, &ID);

	// remove itself from GPULoadCollector
	GPULoadCollector::remove(this);
}

void Image::LoadGPU() {
	if (calledLoadGPU)
		return;

	calledLoadGPU = true;
	std::vector<GLuint> indices = {
		0, 1, 3,
		3, 1, 2
	};

	if (!currVAO)
		currVAO = std::make_unique<VAO>();
	currVAO->Bind();
	currVBO = std::make_unique<VBO>(positions);
	currEBO = std::make_unique<EBO>(indices);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Generates an OpenGL texture object
	glGenTextures(1, &ID);

	// Assigns the texture to a Texture Unit
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(texType, ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	if (texture) {
		glTexImage2D(texType, 0, textureFormat, ogW, ogH, 0, textureFormat, pixelType, texture);
	}

	setLoc(loc);
}

void Image::draw(Shader* shaderProgram) {
	if (!currVAO)
		return;

	shaderProgram->Activate();
	glActiveTexture(GL_TEXTURE0);
	Bind();
	currVAO->Bind();


	glUniform1i(glGetUniformLocation(shaderProgram->ID, "useWorldPos"), useWorldPos);
	shaderProgram->setVec4("color", colorMod);
	
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	Unbind();
	currVAO->Unbind();
}

void Image::setSourceRect(std::shared_ptr<Rect> source) {
	this->source = source;

	w = source->w;
	h = source->h;

	updatePositionsList();
}

void Image::setLoc(vector loc) {
	this->loc = loc;
	if (useWorldPos) {
		this->absoluteLoc = loc;
		updatePositionsList();
	} else {
		vector halfScreen = stuff::screenSize / 2.f;
		vector newLoc;
		if (xAnchor == IMAGE_ANCHOR_LEFT) { // if anchor left
			newLoc.x = (loc.x - halfScreen.x);
		} else if (xAnchor == IMAGE_ANCHOR_RIGHT) { // if anchor right
			newLoc.x = (loc.x + halfScreen.x - w * stuff::pixelSize);
		} else if (xAnchor == IMAGE_ANCHOR_CENTER) {
			newLoc.x = (loc.x - w * stuff::pixelSize / 2.f);
		}

		if (yAnchor == IMAGE_ANCHOR_TOP) { // if anchor top
			newLoc.y = (loc.y - halfScreen.y + h * stuff::pixelSize);
		} else if (yAnchor == IMAGE_ANCHOR_BOTTOM) { // if anchor bottom
			newLoc.y = (loc.y + halfScreen.y);
		} else if (yAnchor == IMAGE_ANCHOR_CENTER) { // if anchor bottom
			newLoc.y = (loc.y + h * stuff::pixelSize / 2.f);
		}

		absoluteLoc = newLoc;
		
		updatePositionsList();
	}
}

vector Image::getLoc() {
	return loc;
}

vector Image::getAbsoluteLoc() {
	return absoluteLoc;
}

void Image::updatePositionsList(std::vector<float> positions) {
	if (!currVAO || !GPULoadCollector::isOnMainThread())
		return;

	currVAO->Bind();
	currVBO->Bind();

	// default, get data data from image data
	if (positions.size() == 0) {
		currVBO->UpdateData(getPositionsList());
	} else { // get data from rotation function
		// rounds position list
		for (int i = 0; i < positions.size(); i++)
			positions[i] = floorf(positions[i]);
		currVBO->UpdateData(positions);
	}
}

void Image::setRotation(float rot) {
	rotation = rot;

	// get center by avging out points
	// then use trig to rotate it

	float angle = glm::radians(rot);

	std::vector<float> positions = getPositionsList();

	// extract positions into vec2s
	glm::vec2 verts[4];
	for (int i = 0; i < 4; i++) {
		verts[i] = glm::vec2(positions[i * 4], positions[i * 4 + 1]);
	}

	// compute center point
	glm::vec2 center(0.f);
	for (int i = 0; i < 4; i++) {
		center += verts[i];
	}
	center /= 4.f;

	// rotate each point around center
	for (int i = 0; i < 4; i++) {
		verts[i] -= center;
		verts[i] = glm::rotate(verts[i], angle);
		verts[i] += center;
		positions[i * 4] = verts[i].x;
		positions[i * 4 + 1] = verts[i].y;
	}

	updatePositionsList(positions);
}

std::vector<float> Image::getPositionsList() {
	// source : w, h
	float x1, x2, y1, y2;
	if (source) {
		x1 = (source->x) / ogW;
		x2 = (source->x + source->w) / ogW;
		y1 = source->y / ogH;
		y2 = (source->y + source->h) / ogH;
	} else {
		x1 = 0;
		x2 = 1;
		y1 = 0;
		y2 = 1;
	}

	if (flipHoriz) {
		float xTemp = x1;
		x1 = x2;
		x2 = xTemp;
	}

	vector scale = (vector{ w, h } * stuff::pixelSize).ceil();
	vector scaledLoc = (absoluteLoc * vector{ 1, -1 }).ceil();
	if (useWorldPos)
		scaledLoc = absoluteLoc * stuff::pixelSize;
	std::vector<float> positions = {
		// Positions										// Texture Coords
		scale.x + scaledLoc.x,	scaledLoc.y,				x2, y2,
		scale.x + scaledLoc.x,	scale.y + scaledLoc.y,		x2, y1,
		scaledLoc.x,			scale.y + scaledLoc.y,	 	x1, y1,
		scaledLoc.x,			scaledLoc.y,				x1, y2
	};

	return positions;
}

bool Image::isMouseOver(bool ignoreTransparent) {
	vector screenLoc = loc;
	if (useWorldPos) {
		vector mousePos = Input::getMousePos();
		screenLoc = math::worldToScreen(loc, "topleft");

		vector size = getSize();
		vector min = { screenLoc.x, screenLoc.y - size.y };
		vector max = { screenLoc.x + size.x, screenLoc.y };
		if (mousePos.x >= min.x && mousePos.x <= max.x && mousePos.y >= min.y && mousePos.y <= max.y) {
			if (ignoreTransparent) {
				vector pos = { mousePos.x - min.x, mousePos.y - min.y };
				glm::vec4 pixelColor = GetPixelColor((int)pos.x, (int)pos.y);
				if ((int)pixelColor.a != 0)
					return true;
				return false;
			} else
				return true;
		}
	} else {
		vector mousePos = Input::getMousePos();
		bool inX = mousePos.x >= screenLoc.x && mousePos.x <= screenLoc.x + w * stuff::pixelSize;
		bool inY = mousePos.y >= screenLoc.y && mousePos.y <= screenLoc.y + h * stuff::pixelSize;
		if (inX && inY)
			return true;
	}

	return false;
}

vector Image::getSize() {
	return vector{ w, h } * stuff::pixelSize;
}

void Image::setSize(vector size) {
	w = size.x / stuff::pixelSize;
	h = size.y / stuff::pixelSize;
	updatePositionsList();
}

void Image::setImage(std::string path) {
	if (!GPULoadCollector::isOnMainThread())
		return;

	textureStruct* texStruct = textureManager::getTexture(path);
	if (texStruct) {
		this->path = path;
		texture = texStruct->texture;
		w = texStruct->w;
		h = texStruct->h;

		if (texStruct->nChannels == 4) {
			textureFormat = GL_RGBA;
		} else if (texStruct->nChannels == 3) {
			textureFormat = GL_RGB;
		} else {
			std::cerr << "Unsupported channel count: " << texStruct->nChannels << std::endl;
			return;
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(texType, ID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		glTexImage2D(texType, 0, textureFormat, w, h, 0, textureFormat, pixelType, texture);
	}
}

void Image::setUseAlpha(bool useAlpha) {
	this->useAlpha = useAlpha;
}

void Image::setColorMod(glm::vec4 colorMod) {
	this->colorMod = colorMod;
}

glm::vec4 Image::GetPixelColor(const int X, const int Y) {
	const int x = X / stuff::pixelSize;
	const int y = Y / stuff::pixelSize;

	unsigned char* pixels = texture;
	int index = (y * ogW + x) * 4;
	unsigned char r = pixels[index];
	unsigned char g = pixels[index + 1];
	unsigned char b = pixels[index + 2];
	unsigned char a = pixels[index + 3];

	return glm::vec4(r, g, b, a);
}

void Image::setAnchor(ImageAnchor xAnchor, ImageAnchor yAnchor) {
	if (useWorldPos) {
		std::cout << "This is a world object, it doesn't work";
		return;
	}

	this->xAnchor = xAnchor;
	this->yAnchor = yAnchor;
	setLoc(loc);
}

void Image::flipHoizontally(bool flip) {
	flipHoriz = flip;
	updatePositionsList();
}

void Image::texUnit(Shader& shader, const char* uniform, GLuint unit) {
	// Gets the location of the uniform
	GLuint texUni = glGetUniformLocation(shader.ID, uniform);
	// Shader needs to be activated before changing the value of a uniform
	shader.Activate();
	// Sets the value of the uniform
	glUniform1i(texUni, unit);
}

void Image::Bind() {
	glBindTexture(type, ID);
}

void Image::Unbind() {
	glBindTexture(type, 0);
}

void Image::Delete() {
	glDeleteTextures(1, &ID);
}

