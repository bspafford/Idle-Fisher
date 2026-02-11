#include "Image.h"
#include "Input.h"
#include "textureManager.h"
#include "ScissorTest.h"
#include "GPULoadCollector.h"

#include "debugger.h"

Image::Image(std::shared_ptr<Image> image, std::shared_ptr<Rect> source, vector loc, bool useWorldPos, bool useDepth) {
	handle = image->handle;
	
	path = image->path;
	this->source = source;
	this->useWorldPos = useWorldPos;
	this->useDepth = useDepth;

	textureStructPtr = image->textureStructPtr;
	if (!handle || !textureStructPtr)
		GPULoadCollector::add(this);

	ogW = image->ogW;
	ogH = image->ogH;
	w = source->w;
	h = source->h;

	this->normalizedSource = *source / vector{ ogW, ogH };

	setLoc(loc);
}

Image::Image(std::string image, vector loc, bool useWorldPos, bool useDepth) {
	path = image;
	this->useWorldPos = useWorldPos;
	this->useDepth = useDepth;

	normalizedSource = { 0, 0, 1, 1 };

	textureStructPtr = textureManager::getTexture(path);
	if (textureStructPtr) {
		w = textureStructPtr->w;
		h = textureStructPtr->h;
		ogW = textureStructPtr->w;
		ogH = textureStructPtr->h;
		GPULoadCollector::add(this);
	}

	setLoc(loc);
}

Image::~Image() {
	GPULoadCollector::remove(this);
}

void Image::LoadGPU() {
	if (textureStructPtr)
		handle = textureStructPtr->handle;

	// sets up absolute loc
	setLoc(loc);
}

void Image::draw(Shader* shader) {
	absoluteLoc = GetAbsoluteLoc(loc, getSize(), useWorldPos, pivot, xAnchor, yAnchor);
	textureManager::DrawImage(shader, absoluteLoc, vector{w, h}, normalizedSource, useWorldPos, colorMod, handle, useDepth);
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
	absoluteLoc = GetAbsoluteLoc(loc, { w, h }, useWorldPos, pivot, xAnchor, yAnchor);
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

	// checks to see if there is a scissor test and if the mouse is inside of it
	bool insideScissor = math::IsPointInRect(mousePos, ScissorTest::GetCurrRect());

	if (insideScissor && math::IsPointInRect(mousePos, min, max)) {
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

std::string Image::getPath() {
	return path;
}

void Image::setImage(std::string path) {
	textureStructPtr = textureManager::getTexture(path);
	if (textureStructPtr) {
		this->path = path;
		w = textureStructPtr->w;
		h = textureStructPtr->h;

		this->loc = loc;
		
		handle = textureStructPtr->handle;

		ogW = textureStructPtr->w;
		ogH = textureStructPtr->h;

		// sets up absolute loc
		setLoc(loc);

		if (source)
			this->normalizedSource = *source / vector{ ogW, ogH };
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

	normalizedSource.w = abs(normalizedSource.w);
	if (flip)
		normalizedSource.w *= -1.f;
}