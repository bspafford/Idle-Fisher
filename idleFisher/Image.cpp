#include "Image.h"
#include "Input.h"
#include "textureManager.h"

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

	textureStructPtr = textureManager::getTexture(path);

	handle = textureStructPtr->handle;

	ogW = textureStructPtr->w;
	ogH = textureStructPtr->h;
	w = textureStructPtr->w;
	h = textureStructPtr->h;

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
	vector imgLoc = absoluteLoc;
	if (useWorldPos) // converts to screen position
		imgLoc = math::worldToScreen(absoluteLoc);

	vector size = getSize();
	vector min = imgLoc;
	vector max = min + size;
	if (mousePos.x >= min.x && mousePos.x <= max.x && mousePos.y >= min.y && mousePos.y <= max.y) {
		if (ignoreTransparent) {
			vector relPos = mousePos - min;
			if (textureStructPtr->GetAlphaAtPos(relPos))
				return true;
		} else
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