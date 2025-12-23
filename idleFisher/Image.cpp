#include "Image.h"
#include "Input.h"
#include "stuff.h"
#include "textureManager.h"
#include <glm/gtx/rotate_vector.hpp>

#include "debugger.h"

Image::Image(std::shared_ptr<Image> image, std::shared_ptr<Rect> source, vector loc, bool useWorldPos) {
	handle = image->handle;
	
	path = image->path;
	this->source = source;
	this->useWorldPos = useWorldPos;

	ogW = image->ogW;
	ogH = image->ogH;
	w = source->w;
	h = source->h;

	setLoc(loc);

	this->normalizedSource = *source / vector{ ogW, ogH };
}

Image::Image(std::string image, vector loc, bool useWorldPos) {
	this->loc = loc;

	path = image;
	this->useWorldPos = useWorldPos;

	textureStruct* img = textureManager::getTexture(path);

	handle = img->handle;

	ogW = img->w;
	ogH = img->h;
	w = img->w;
	h = img->h;

	// sets up absolute loc
	setLoc(loc);

	normalizedSource = { 0, 0, 1, 1 };
}

void Image::draw(Shader* shader) {
	absoluteLoc = GetAbsoluteLoc(loc, getSize(), useWorldPos, pivot, xAnchor, yAnchor);
	textureManager::DrawImage(shader, absoluteLoc, vector{w, h}, normalizedSource, useWorldPos, colorMod, handle);
}

void Image::setSourceRect(std::shared_ptr<Rect> source) {
	this->source = source;
	this->normalizedSource = *source / vector{ ogW, ogH };

	w = source->w;
	h = source->h;
}

void Image::setLoc(vector loc) {
	this->loc = loc;
	absoluteLoc = GetAbsoluteLoc(loc, getSize(), useWorldPos, pivot, xAnchor, yAnchor);
}

vector Image::getLoc() {
	return loc;
}

vector Image::getAbsoluteLoc() {
	return absoluteLoc;
}

void Image::setRotation(float rot) {
	rotation = rot;
}

bool Image::isMouseOver(bool ignoreTransparent) {
	vector mousePos = Input::getMousePos();
	if (useWorldPos) {
		vector screenLoc = math::worldToScreen(absoluteLoc);

		vector size = getSize();
		vector min = screenLoc;
		vector max = min + size;
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
		bool inX = mousePos.x >= absoluteLoc.x && mousePos.x <= absoluteLoc.x + w;
		bool inY = mousePos.y >= absoluteLoc.y && mousePos.y <= absoluteLoc.y + h;
		if (inX && inY)
			return true;
	}

	return false;
}

vector Image::getSize() {
	return vector{ w, h };
}

void Image::setSize(vector size) {
	w = size.x;
	h = size.y;
}

void Image::setImage(std::string path) {
	textureStruct* texStruct = textureManager::getTexture(path);
	if (texStruct) {
		this->path = path;
		w = texStruct->w;
		h = texStruct->h;
	}
}

void Image::setUseAlpha(bool useAlpha) {
	this->useAlpha = useAlpha;
}

void Image::setColorMod(glm::vec4 colorMod) {
	this->colorMod = colorMod;
}

glm::vec4 Image::GetPixelColor(const int X, const int Y) {
	return glm::vec4(1);

	/*const int x = X / stuff::pixelSize;
	const int y = Y / stuff::pixelSize;

	unsigned char* pixels = texture;
	int index = (y * ogW + x) * 4;
	unsigned char r = pixels[index];
	unsigned char g = pixels[index + 1];
	unsigned char b = pixels[index + 2];
	unsigned char a = pixels[index + 3];

	return glm::vec4(r, g, b, a);*/
}

void Image::SetAnchor(Anchor xAnchor, Anchor yAnchor) {
	if (useWorldPos) {
		std::cout << "This is a world object, it doesn't work";
		return;
	}

	this->xAnchor = xAnchor;
	this->yAnchor = yAnchor;
	setLoc(loc);
}

void Image::SetPivot(vector pivot) {
	this->pivot = vector::clamp(pivot, 0.f, 1.f);
	setLoc(loc);
}

void Image::flipHoizontally(bool flip) {
	flipHoriz = flip;
}