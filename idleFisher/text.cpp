#include "text.h"

#include "main.h"
#include "saveData.h"

#include <fstream>
#include <iostream>

#include "camera.h"
#include "GPULoadCollector.h"
#include "FBO.h"

#include "debugger.h"

text::text(widget* parent, std::string text, std::string font, vector loc, bool useWorldPos, bool isometric, TextAlign alignment) : widget(parent) {
	instances.push_back(this);

	this->alignment = alignment;
	this->font = font;
	this->loc = loc.floor();
	this->isometric = isometric;
	this->useWorldPos = useWorldPos;

	loadTextImg();
	setText(text);

	GPULoadCollector::add(this);
}

text::~text() {
	auto it = std::find(instances.begin(), instances.end(), this);
	if (it != instances.end())
		instances.erase(it);

	letters.clear();
	textInfo.clear();

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

void text::loadTextImg() {
	textImg = std::make_shared<Image>("./fonts/" + font + "/" + font + ".png", vector{ 0, 0 }, useWorldPos);

	std::ifstream file("./fonts/" + font + "/" + font + ".txt");
	if (!file) {
		std::cout << "no font file\n";
		return;
	}

	std::string line;
	std::string delimiter = " ";
	std::string delimiter1 = ":";

	int lineNum = 0;
	int xOffset = 0;

	// parse header row
	std::string headerLine;
	std::getline(file, headerLine);
	std::vector<std::string> delimLine;
	std::istringstream stream(headerLine);
	while (std::getline(stream, line, ' '))
		delimLine.push_back(line);

	if (delimLine.size() > 0)
		textHeight = std::stoi(delimLine[0]);
	if (delimLine.size() > 1)
		dropHeight = std::stoi(delimLine[1]);

	// parse body
	while (std::getline(file, line)) {
		// get each word in line
		size_t pos = 0;
		std::string token;
		while ((pos = line.find(delimiter)) != std::string::npos || line != "") {
			// includes the last item in list
			if (line.find(delimiter) == std::string::npos)
				pos = line.size();

			char token1;
			size_t pos1 = 0;
			token = line.substr(0, pos);
			if (token.find("32:") != std::string::npos) {
				line.erase(0, pos + delimiter.length());
				if ((pos1 = token.find(":")) != std::string::npos) {
					token.erase(0, pos1 + delimiter1.length());
					textInfo[32].loc = { float(xOffset), float(lineNum) };
				}

				token1 = 32;
			} else if (token.find("58:") != std::string::npos) {
				line.erase(0, pos + delimiter.length());
				if ((pos1 = token.find(":")) != std::string::npos) {
					token.erase(0, pos1 + delimiter1.length());
					textInfo[58].loc = { float(xOffset), float(lineNum) };
				}

				token1 = 58;
			} else {
				line.erase(0, pos + delimiter.length());
				while ((pos1 = token.find(":")) != std::string::npos) {
					token1 = *token.substr(0, pos1).c_str();
					token.erase(0, pos1 + delimiter1.length());
					textInfo[token1].loc = { float(xOffset), float(lineNum) };
				}
			}

			xOffset += std::stoi(token);
			textInfo[token1].size = { std::stof(token), float(textHeight) };
		}

		xOffset = 0;
		lineNum += textHeight;
	}
}

void text::setTextColor(int r, int g, int b) {
	colorMod = glm::vec4(r / 255.f, g / 255.f, b / 255.f, 1.f);
}

void text::makeText(int i, std::string text, vector &offset) {
	if (text[i] == ' ') {
		start = i;
		numLetters = 0;
	}

	FtextInfo currInfo = textInfo[text[i]];
	// ignores special chars, and makes sure the char is in the font
	if (text[i] >= 32 && currInfo.loc.x == 0 && currInfo.loc.y == 0 && currInfo.size.x == 0 && currInfo.size.y == 0) {
		std::cout << "Char is not in list: '" << text[i] << "' (" << (int)text[i] << ") not in '" << font << "'\n";
		std::cout << "'" << textString << "'" << "\n";
		throw std::runtime_error("Character is not in list");
	}

	// makes new line
	if (text[i] == '\n') {
		offset.x = 0;
		offset.y += (textHeight + 1);
	} else {
		std::shared_ptr<Rect> source = std::make_shared<Rect>(currInfo.loc.x, currInfo.loc.y, currInfo.size.x, currInfo.size.y);

		letters[i] = std::make_unique<Image>(textImg, source, vector{ 0, 0 }, false);
		Image* letter = letters[i].get();

		if (alignment != TEXT_ALIGN_RIGHT)
			letter->setLoc(offset);

		int temp = 1;
		if (alignment == TEXT_ALIGN_RIGHT)
			temp = -1;

		offset.x += letter->w * temp;// *stuff::pixelSize;

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
		if (lineLength != -1 && offset.x > lineLength) { //  && textString[start] != ' '
			offset.x = 0;
			offset.y += (letter->h + 1);
			for (int j = start; j < start + numLetters + 1; j++) {
				letters[j]->setLoc(offset);
				offset.x += letters[j]->w;
			}
		}
	}

	numLetters++;
}

void text::setText(std::string text) {
	if (text == textString)
		return;

	// converts \\n to \n
	size_t pos = 0;
	while ((pos = text.find("\\n", pos)) != std::string::npos) {
		text.replace(pos, 2, "\n");
		pos += 1;
	}

	textString = text;
	
	if (text == "")
		return;

	if (!SaveData::settingsData.pixelFont) {
		changeFont();
	} else {
		letters.clear();
		letters = std::vector<std::unique_ptr<Image>>(text.size());

		vector offset = { 0, 0 };
		if (alignment == TEXT_ALIGN_RIGHT) {
			for (int i = letters.size() - 1; i >= 0; i--) {
				makeText(i, text, offset);
			}
		} else if (alignment == TEXT_ALIGN_LEFT || alignment == TEXT_ALIGN_CENTER) {
			for (int i = 0; i < letters.size(); i++) {
				makeText(i, text, offset);
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
		else
			updateWhileOnMain = true;
	}
}

void text::makeTextTexture() {
	if (!GPULoadCollector::isOnMainThread())
		return;

	if (letters.size() == 0)
		return;

	vector fboSize = getFBOSize();

	if (fboSize.x == 0 || fboSize.y == 0)
		return;

	glm::mat4 currProjection = Camera::getProjectionMat();

	Main::twoDShader->Activate();
	Main::twoDShader->setMat4("projection", Camera::getProjectionMat(fboSize * stuff::pixelSize));

	absoluteLoc = absoluteLoc.floor();

	fbo = std::make_unique<FBO>(fboSize * stuff::pixelSize, useWorldPos);
	fbo->BindFramebuffer();
	// Draw to the FBO
	for (int i = 0; i < letters.size(); i++)
		if (letters[i]) {
			letters[i]->setLoc({letters[i]->getLoc().x, letters[i]->getLoc().y - textHeight + fboSize.y}); // push to top of fbo
			std::string dropList("qypgj");
			if (std::find(dropList.begin(), dropList.end(), textString[i]) != dropList.end())
				letters[i]->setLoc(vector{letters[i]->getLoc().x, letters[i]->getLoc().y - dropHeight }); // add add dropHeight
			letters[i]->draw(Main::twoDShader); // can't use draw, cause it just queues it, need to actually draw it
		}
	// Unbind FBO
	fbo->UnbindFramebuffer();

	setLoc(loc);

	// restores projection
	Main::twoDShader->setMat4("projection", currProjection);

	letters.clear();
}

void text::draw(Shader* shader) {
	if (textString == "")
		return;

	fbo->Draw(shader, absoluteLoc, getSize(), Rect{ 0, 0, 1, 1 }, useWorldPos, colorMod);
}

void text::setLocAndSize(vector loc, vector size) {
	setLoc(loc);
}

void text::setLoc(vector loc) {
	loc = loc.floor();
	__super::setLoc(loc);
	if (useWorldPos) {
		vector size = getSize();

		if (alignment == TEXT_ALIGN_LEFT) {
			absoluteLoc = loc;
			if (isometric)
				absoluteLoc += vector{ 0, size.y };
		} else if (alignment == TEXT_ALIGN_RIGHT) {
			absoluteLoc = loc - vector{ size.x, 0 };
			if (isometric)
				absoluteLoc -= vector{ 0, getSize().y };
		} else if (alignment == TEXT_ALIGN_CENTER) {
			absoluteLoc = loc - vector{ getSize().x / 2.f, 0.f };
		}
	} else {
		vector size = getSize();
		vector halfScreen = (stuff::screenSize / 2.f);

		if (alignment == TEXT_ALIGN_LEFT) {
			absoluteLoc = loc;
		} else if (alignment == TEXT_ALIGN_RIGHT) {
			absoluteLoc = loc - vector{ getSize().x, 0.f };
		} else if (alignment == TEXT_ALIGN_CENTER) {
			absoluteLoc = loc - vector{ (getSize().x / 2.f), 0.f };
		}
	}
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
	return fbo ? fbo->GetSize() : vector{ 0, 0 };
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
		if (maxX < letterLoc.x + letter->w)
			maxX = letterLoc.x + letter->w;

		if (minY > letterLoc.y)
			minY = letterLoc.y;
		if (maxY < letterLoc.y + letter->h)
			maxY = letterLoc.y + letter->h;
	}
	return { ceil(maxX - minX), ceil(maxY - minY) + dropHeight };
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