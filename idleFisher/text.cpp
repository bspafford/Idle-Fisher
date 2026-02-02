#include "text.h"
#include "Scene.h"
#include "saveData.h"

#include <fstream>
#include <iostream>

#include "GPULoadCollector.h"
#include "FBO.h"
#include "PakReader.h"

#include "debugger.h"

void text::Init() {
	PakReader::ParseFonts("data/fonts.pak");
}

void text::Shutdown() {
	PakReader::ClearTextData();
}

text::text(widget* parent, std::string text, std::string font, vector loc, bool useWorldPos, bool isometric, TextAlign alignment) : widget(parent) {
	instances.push_back(this);

	this->alignment = alignment;
	this->font = font;
	this->loc = loc.floor();
	this->isometric = isometric;
	this->useWorldPos = useWorldPos;

	textImg = std::make_shared<Image>("fonts/" + font + "/" + font + ".png", vector{ 0, 0 }, useWorldPos);
	fontInfo = PakReader::GetFontData(font);

	setText(text);

	GPULoadCollector::add(this);
}

text::~text() {
	auto it = std::find(instances.begin(), instances.end(), this);
	if (it != instances.end())
		instances.erase(it);

	letters.clear();

	GPULoadCollector::remove(this);
}

void text::LoadGPU() {
	if (updateWhileOnMain) {
		makeTextTexture();
		updateWhileOnMain = false;
	}
}

void text::changeFontAll() {
	for (text* text : instances) {
		text->changeFont();
	}
}

void text::changeFont() {
	if (!SaveData::settingsData.pixelFont) {
		
	} else {
		setText(textString);
	}
}

void text::setTextColor(int r, int g, int b) {
	colorMod = glm::vec4(r / 255.f, g / 255.f, b / 255.f, 1.f);
}

void text::SetAlpha(float alpha) {
	colorMod.a = alpha;
}

void text::makeText(int& i, std::string text, vector &offset) {
	if (text[i] == ' ') {
		start = i;
		numLetters = 0;
	}

	Rect letterRect = fontInfo->letterRect[text[i]];
	// ignores special chars, and makes sure the char is in the font
	if (text[i] >= 32 && letterRect.x == 0 && letterRect.y == 0 && letterRect.w == 0 && letterRect.h == 0) {
		std::cout << "Char is not in list: '" << text[i] << "' (" << (int)text[i] << ") not in '" << font << "'\n";
		std::cout << "'" << textString << "'" << "\n";
		throw std::runtime_error("Character is not in list");
	}

	std::string dropList("qypgj");
	if (std::find(dropList.begin(), dropList.end(), text[i]) != dropList.end())
		hasDropChar = true;	

	// makes new line
	if (text[i] == '\n') {
		offset.x = 0;
		offset.y -= (fontInfo->height + 1);
	} else {
		std::shared_ptr<Rect> source = std::make_shared<Rect>(letterRect);

		letters[i] = std::make_unique<Image>(textImg, source, vector{ 0, 0 }, false);
		Image* letter = letters[i].get();

		if (alignment != TEXT_ALIGN_RIGHT)
			letter->setLoc(offset);

		int temp = 1;
		if (alignment == TEXT_ALIGN_RIGHT)
			temp = -1;

		offset.x += letter->getSize().x * temp;// *stuff::pixelSize;

		if (isometric) {
			if (text[i] == '.')
				offset.y += 1 * temp;
			else
				offset.y += 2 * temp;
		}

		if (alignment == TEXT_ALIGN_RIGHT)
			letter->setLoc(offset);

		// ONLY works for ALIGNED LEFT
		// wraps text if its length is greater than its linelength
		if (lineLength != -1 && offset.x > lineLength) {
			offset.x = 0;
			offset.y -= (letter->getSize().y + 1);
			for (int j = start; j < start + numLetters + 1; j++) {
				letters[j]->setLoc(offset);
				offset.x += letters[j]->getSize().x;
			}
		}
	}

	numLetters++;
}

void text::setText(std::string text) {
	std::string parsed = ParseTextString(text);
	if (parsed == textString)
		return;

	textString = parsed;
	hasDropChar = false;
	
	if (textString == "")
		return;

	if (!SaveData::settingsData.pixelFont) {
		changeFont();
	} else {
		letters.clear();
		letters = std::vector<std::unique_ptr<Image>>(textString.size());

		vector offset = { 0, 0 };
		if (alignment == TEXT_ALIGN_RIGHT) {
			for (int i = letters.size() - 1; i >= 0; i--) {
				makeText(i, textString, offset);
			}
		} else if (alignment == TEXT_ALIGN_LEFT || alignment == TEXT_ALIGN_CENTER) {
			for (int i = 0; i < letters.size(); i++) {
				makeText(i, textString, offset);
			}
		}

		// corrects for chars being out of textures "frame" if isometric, aligned center, or aligned right
		if (alignment != TEXT_ALIGN_LEFT || isometric) {
			if (letters[0] && letters[letters.size() - 1]) {
				vector _loc = { -letters[0]->getLoc().x, -letters[letters.size() - 1]->getLoc().y};
				for (int i = 0; i < letters.size(); i++) {
					letters[i]->setLoc((letters[i]->getLoc() + _loc));
				}
			}
		}						


		if (GPULoadCollector::isOnMainThread())
			makeTextTexture();
		else {
			updateWhileOnMain = true;
		}
	}
}

std::string text::ParseTextString(std::string text) {
	colorList.clear();

	// converts \\n to \n
	size_t pos = 0;
	while ((pos = text.find("\\n", pos)) != std::string::npos) {
		text.replace(pos, 2, "\n");
	}
	
	for (size_t i = 0; i < text.size(); i++) {
		if (text[i] == '<') { // color modifier
			// find >, and parse inner
			// if start with '/' then its ending the color
			size_t pos = text.find('>', i);
			if (pos != std::string::npos) {
				int start = i + 1;
				int len = pos - i - 1;
				std::string color = text.substr(start, len);
				text.erase(start - 1, len + 2); // remove color modifier from string, including <>

				colorList.push_back(std::pair(i, ParseColor(color)));
			}
		}
	}

	return text;
}

glm::vec4 text::ParseColor(const std::string& color) {
	if (color == "red")
		return glm::vec4(1, 0, 0, 1);
	else if (color == "green")
		return glm::vec4(0, 1, 0, 1);
	else if (color == "blue")
		return glm::vec4(0, 0, 1, 1);
	return glm::vec4(1); // default white
}

void text::makeTextTexture() {
	if (!GPULoadCollector::isOnMainThread())
		return;

	if (letters.size() == 0)
		return;

	vector fboSize = getFBOSize();
	fboSize += vector(1, int(fboSize.y) % 2); // seems to fix text subpixel problems

	if (fboSize.x == 0 || fboSize.y == 0)
		return;

	absoluteLoc = absoluteLoc.floor();

	fbo = std::make_unique<FBO>(fboSize, useWorldPos);
	fbo->Bind();
	
	int colorIndex = -1; // keeps track of what color the text is on

	// Draw to the FBO
	for (int i = 0; i < letters.size(); i++) {
		// set color from color modifier
		if (colorIndex + 1 < colorList.size() && i >= colorList[colorIndex + 1].first) // check to see if should change to next color
			colorIndex++;

		if (letters[i]) {
			letters[i]->setLoc(vector{ letters[i]->getLoc().x, letters[i]->getLoc().y - fontInfo->height + fboSize.y }.round()); // push to top of fbo
			std::string dropList("qypgj");
			if (std::find(dropList.begin(), dropList.end(), textString[i]) != dropList.end())
				letters[i]->setLoc(vector{ letters[i]->getLoc().x, letters[i]->getLoc().y - fontInfo->dropHeight }.round()); // add add dropHeight

			if (colorIndex != -1)
				letters[i]->setColorMod(colorList[colorIndex].second);

			letters[i]->draw(Scene::twoDShader);
		}
	}
	fbo->Unbind();

	setLoc(loc);
	
	letters.clear();
}

void text::draw(Shader* shader) {
	if (!fbo || textString == "")
		return;

	fbo->Draw(shader, absoluteLoc, Rect{ 0, 0, 1, 1 }, useWorldPos, colorMod);
}

void text::setLocAndSize(vector loc, vector size) {
	setLoc(loc);
}

void text::setLoc(vector loc) {
	loc = loc.ToPixel();
	__super::setLoc(loc);

	size = getSize();
	vector pivotLoc = size * pivot;

	vector textAlignOffset = { 0, 0 };
	if (useWorldPos) {
		if (alignment == TEXT_ALIGN_LEFT) {
			textAlignOffset = loc;
			if (isometric)
				textAlignOffset += vector{ 0, size.y };
		} else if (alignment == TEXT_ALIGN_RIGHT) {
			textAlignOffset = loc - vector{ size.x, 0 };
			if (isometric)
				textAlignOffset -= vector{ 0, size.y };
		} else if (alignment == TEXT_ALIGN_CENTER) {
			textAlignOffset = loc - vector{ size.x / 2.f, 0.f };
		}
	} else {
		vector halfScreen = (stuff::screenSize / 2.f);

		if (alignment == TEXT_ALIGN_LEFT) {
			textAlignOffset = loc;
		} else if (alignment == TEXT_ALIGN_RIGHT) {
			textAlignOffset = loc - vector{ size.x, 0.f };
		} else if (alignment == TEXT_ALIGN_CENTER) {
			textAlignOffset = loc - vector{ (size.x / 2.f), 0.f };
		}
	}

	absoluteLoc = textAlignOffset - pivotLoc;
}

void text::setAnchor(Anchor xAnchor, Anchor yAnchor) {
	if (useWorldPos) {
		std::cout << "This is a world object, it doesn't work";
		return;
	}

	this->xAnchor = xAnchor;
	this->yAnchor = yAnchor;
	setLoc(loc);
}

vector text::getSize() {
	return fbo ? fbo->GetSize() : getFBOSize();
}

vector text::getFBOSize() {
	if (letters.size() == 0)
		return { 0, 0 };

	float minX = INFINITY, minY = INFINITY;
	float maxX = 0, maxY = 0;
	for (int i = 0; i < letters.size(); i++) {
		Image* letter = letters[i].get();
		if (!letter)
			continue;

		vector letterLoc = letter->getLoc();
		if (minX > letterLoc.x)
			minX = letterLoc.x;
		if (maxX < letterLoc.x + letter->getSize().x)
			maxX = letterLoc.x + letter->getSize().x;

		if (minY > letterLoc.y)
			minY = letterLoc.y;
		if (maxY < letterLoc.y + letter->getSize().y)
			maxY = letterLoc.y + letter->getSize().y;
	}
	vector size = { ceil(maxX - minX), ceil(maxY - minY) };
	if (hasDropChar)
		size += vector{ 0.f, static_cast<float>(fontInfo->dropHeight) };
	return size;
}

void text::setLineLength(float length) {
	lineLength = length;
	setText(textString);
}

std::string text::getString() {
	return textString;
}

float text::getLineLength() {
	return lineLength;
}

void text::setupLocs() {
	setText(textString);
}