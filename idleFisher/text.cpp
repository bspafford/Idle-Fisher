#include "text.h"

#include "main.h"
#include "saveData.h"

#include <fstream>
#include <iostream>

#include "camera.h"
#include "GPULoadCollector.h"

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

	deleteObjects();

	letters.clear();
	textInfo.clear();

	GPULoadCollector::remove(this);
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

int text::calcFontSize() {
	if (font == "tall" || font == "straightDark" || font == "straightBoldDark" || font == "straight" || font == "normal" || font == "boldDark" || font == "bold") {
		return 5 * stuff::pixelSize;
	} else {
		return 7 * stuff::pixelSize;
	}
}

void text::loadTextImg() {
	textImg = std::make_shared<Image>("./fonts/" + font + "/" + font + ".png", vector{ 0, 0 }, useWorldPos);

	// make a list
	// this list would be the size of all char (126)
	// then it would contain the starting location and the size of the char
	// then i could get that infromation by using the char num
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

	// gets the height of the text
	std::string textHeightStr;
	std::getline(file, textHeightStr);
	textHeight = std::stoi(textHeightStr);

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
		offset.y += (textHeight + 1) * stuff::pixelSize;
	} else {
		std::shared_ptr<Rect> source = std::make_shared<Rect>(currInfo.loc.x, currInfo.loc.y, currInfo.size.x, currInfo.size.y);

		letters[i] = std::make_unique<Image>(textImg, source, vector{ 0, 0 }, false);
		letters[i]->setAnchor(IMAGE_ANCHOR_CENTER, IMAGE_ANCHOR_CENTER);
		Image* letter = letters[i].get();

		if (alignment != TEXT_ALIGN_RIGHT)
			letter->setLoc(offset);

		int temp = 1;
		if (alignment == TEXT_ALIGN_RIGHT)
			temp = -1;

		offset.x += letter->w * temp * stuff::pixelSize;

		if (isometric) {
			if (text[i] == '.')
				offset.y -= 1 * stuff::pixelSize * temp;
			else
				offset.y -= 2 * stuff::pixelSize * temp;
		}

		if (alignment == TEXT_ALIGN_RIGHT)
			letter->setLoc(offset);

		// ONLY works for ALIGNED LEFT
		// wraps text if its length is greater than its linelength
		if (lineLength != -1 && offset.x > lineLength) { //  && textString[start] != ' '
			offset.x = 0;
			offset.y += (letter->h + 1) * stuff::pixelSize;
			for (int j = start; j < start + numLetters + 1; j++) {
				letters[j]->setLoc(offset);
				offset.x += letters[j]->w * stuff::pixelSize;
			}
		}
	}

	if (textString == "Buy Fish Transporter") {
		std::cout << "offset: " << offset << "\n";
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

		makeTextTexture();
	}
}

void text::deleteObjects() {
	if (currVAO)
		currVAO->Delete();
	if (currEBO)
		currEBO->Delete();

	glDeleteBuffers(1, &VBOId);
	glDeleteTextures(1, &textTexture);
	glDeleteFramebuffers(1, &fbo);

	VBOId = 0;
	textTexture = 0;
	fbo = 0;
}

void text::makeTextTexture() {
	if (!GPULoadCollector::isOnMainThread())
		return;

	deleteObjects();

	if (letters.size() == 0)
		return;

	fboSize = (getFBOSize() + vector{ 0, textHeight / 2.f * stuff::pixelSize }).ceil();
	// don't know but it fixed pixel texture offset problem
	fboSize.x += (int)fboSize.x % 2 != 0; // make even
	fboSize.y += (int)fboSize.y % 2 == 0; // make odd

	if (fboSize.x == 0 || fboSize.y == 0)
		return;

	glm::mat4 currProjection = Main::camera->getProjectionMat();

	Main::twoDShader->Activate();
	Main::twoDShader->setMat4("projection", Main::camera->getProjectionMat(fboSize));

	absoluteLoc = absoluteLoc.floor();
	vector scaledLoc = (absoluteLoc * vector{ 1, -1 });
	if (useWorldPos)
		scaledLoc = absoluteLoc * stuff::pixelSize;

	float positions[] = {
		// Positions											// Texture Coords
		floor(fboSize.x + scaledLoc.x),	floor(scaledLoc.y),				1.f, 1.f,	// Bottom-right
		floor(fboSize.x + scaledLoc.x),	floor(fboSize.y + scaledLoc.y),	1.f, 0.f,	// Top-right
		floor(scaledLoc.x),				floor(fboSize.y + scaledLoc.y),	0.f, 0.f,	// Top-left
		floor(scaledLoc.x),				floor(scaledLoc.y),				0.f, 1.f	// Bottom-left
	};

	if (textString == "Buy Fish Transporter")
		std::cout << "fboSize: " << fboSize << ", absoluteLoc: " << absoluteLoc << ", scaledLoc : " << scaledLoc << "\n";

	std::vector<GLuint> indices = {
		0, 1, 3, // First triangle
		3, 1, 2  // Second triangle
	};

	currVAO = std::make_unique<VAO>();
	currVAO->Bind();

	glGenBuffers(1, &VBOId);
	glBindBuffer(GL_ARRAY_BUFFER, VBOId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

	currEBO = std::make_unique<EBO>(indices);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	//currVBO->Unbind();
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Create Texture
	glGenTextures(1, &textTexture);
	glBindTexture(GL_TEXTURE_2D, textTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fboSize.x, fboSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Create FBO
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, fboSize.x, fboSize.y);
	// puts scissor size to fbo and saves previous size
	GLint scissorBox[4];
	glGetIntegerv(GL_SCISSOR_BOX, scissorBox);
	glScissor(0, 0, fboSize.x, fboSize.y);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "FBO is incomplete!" << std::endl;
	}

	// Draws to the FBO
	for (int i = 0; i < letters.size(); i++)
		if (letters[i]) {
			letters[i]->setLoc(vector{ letters[i]->getLoc().x - (fboSize.x - letters[i]->getSize().x) / 2.f, letters[i]->getLoc().y + (letters[i]->getSize().y - fboSize.y) / 2.f }.floor());
			// drops characters if they are inside the drop list and the correct font
			std::string dropList = "gpqjy";
			std::vector<std::string> dontDropFont = { "afScreen", "biggerStraight", "biggerStraightDark" };
			if (std::find(dontDropFont.begin(), dontDropFont.end(), font) == dontDropFont.end() && std::find(dropList.begin(), dropList.end(), textString[i]) != dropList.end())
				letters[i]->setLoc(vector{letters[i]->getLoc().x, letters[i]->getLoc().y + letters[i]->getSize().y / 2.f}.floor());

			letters[i]->draw(Main::twoDShader);
		}

	if (textString == "Buy Fish Transporter")
		std::cout << "fboSize: " << fboSize << "\n";

	// Unbind FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, stuff::screenSize.x, stuff::screenSize.y);
	// puts scissor size back to what it was before
	glScissor(scissorBox[0], scissorBox[1], scissorBox[2], scissorBox[3]);

	setLoc(loc);

	// restores projection
	Main::twoDShader->setMat4("projection", currProjection);
}

void text::draw(Shader* shaderProgram) {
	if (textString == "")
		return;
	drawTexture(shaderProgram, textTexture);
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
				absoluteLoc += vector{ 0, size.y / stuff::pixelSize };
		} else if (alignment == TEXT_ALIGN_RIGHT) {
			absoluteLoc = loc - vector{ size.x / stuff::pixelSize, 0 };
			if (isometric)
				absoluteLoc -= vector{ 0, getSize().y / stuff::pixelSize };
		} else if (alignment == TEXT_ALIGN_CENTER) {
			absoluteLoc = loc - vector{ getSize().x / 2.f, 0.f };
		}

		updatePositionsList();
	} else {
		vector size = getSize();
		vector halfScreen = (stuff::screenSize / 2.f);

		// convert { 0, 0 } from center to top left of screen and makes the top left the { 0, 0 } of the text
		loc += (-halfScreen + vector{0, size.y});

		if (alignment == TEXT_ALIGN_LEFT) {
			absoluteLoc = loc;
		} else if (alignment == TEXT_ALIGN_RIGHT) {
			absoluteLoc = loc - vector{ getSize().x, 0.f };
		} else if (alignment == TEXT_ALIGN_CENTER) {
			absoluteLoc = loc - vector{ (getSize().x / 2.f), 0.f };
		}

		updatePositionsList();
	}
}

void text::updatePositionsList() {
	if (!GPULoadCollector::isOnMainThread())
		return;

	if (!currVAO)
		return;

	currVAO->Bind();
	glBindBuffer(GL_ARRAY_BUFFER, VBOId);

	float x1 = 0;
	float x2 = 1;
	float y1 = 0;
	float y2 = 1;

	vector scale = getSize();
	vector scaledLoc = (absoluteLoc * vector{ 1, -1 });
	if (useWorldPos)
		scaledLoc = (absoluteLoc * stuff::pixelSize);
	float positions[] = {
		// Positions // Texture Coords
		round(fboSize.x + scaledLoc.x),	round(scaledLoc.y),				1.f, 0.f,	// Bottom-right
		round(fboSize.x + scaledLoc.x),	round(fboSize.y + scaledLoc.y),	1.f, 1.f,	// Top-right // idk why the y texCoords have to be flipped
		round(scaledLoc.x),				round(fboSize.y + scaledLoc.y),	0.f, 1.f,	// Top-left
		round(scaledLoc.x),				round(scaledLoc.y),				0.f, 0.f	// Bottom-left
	};

	// updates the tex coords
	void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (ptr) {
		memcpy(ptr, positions, sizeof(positions)); // Copy updated vertex data
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}
}

void text::setAnchor(ImageAnchor xAnchor, ImageAnchor yAnchor) {
	if (useWorldPos) {
		std::cout << "This is a world object, it doesn't work";
		return;
	}

	this->xAnchor = xAnchor;
	this->yAnchor = yAnchor;
	setLoc(loc);
}

vector text::getSize() {
	return fboSize - vector{ 0, textHeight / 2.f * stuff::pixelSize };
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
		if (maxX < letterLoc.x + letter->w * stuff::pixelSize)
			maxX = letterLoc.x + letter->w * stuff::pixelSize;

		if (minY > letterLoc.y)
			minY = letterLoc.y;
		if (maxY < letterLoc.y + letter->h * stuff::pixelSize)
			maxY = letterLoc.y + letter->h * stuff::pixelSize;
	}
	return { ceil(maxX - minX), ceil(maxY - minY) };
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

void text::drawTexture(Shader* shaderProgram, GLuint textureID) {
	if (!currVAO)
		return;

	shaderProgram->Activate();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	currVAO->Bind();

	glUniform1i(glGetUniformLocation(shaderProgram->ID, "useWorldPos"), useWorldPos);
	shaderProgram->setVec4("color", colorMod);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	currVAO->Unbind();
}