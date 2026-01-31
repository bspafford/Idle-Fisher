#pragma once

#include <string>
#include <vector>
#include <mutex>

#include "math.h"
#include "widget.h"
#include "Image.h"

class FBO;

enum TextAlign {
	TEXT_ALIGN_LEFT,
	TEXT_ALIGN_RIGHT,
	TEXT_ALIGN_CENTER,
};

struct FfontInfo {
	uint16_t height = 0;
	// How many pixels chars like 'g', 'y', etc, should be dropped
	uint16_t dropHeight = 0;
	std::vector<Rect> letterRect = std::vector<Rect>(126);
};

class text : public widget {
public:
	static void Init();
	static void Shutdown();

	text(widget* parent, std::string text, std::string font, vector loc, bool useWorldPos = false, bool isometric = false, TextAlign alignment = TEXT_ALIGN_LEFT);
	~text();
	void LoadGPU();
	void setText(std::string text);
	void draw(Shader* shader);
	// void setAlignment(std::string alignment);
	void makeText(int& i, std::string text, vector &offset);
	vector getSize();
	std::string getString();
	void setTextColor(int r, int g, int b);
	void setupLocs() override;

	// Wraps text if it goes above line length, set to -1 if text doesn't wrap
	void setLineLength(float length);
	float getLineLength();

	void setLocAndSize(vector loc, vector size) override;
	void setLoc(vector loc) override;
	// anchor::...
	void setAnchor(Anchor xAnchor, Anchor yAnchor);

	// used when changing the font from pixel to normal or vice versa
	static void changeFontAll();
	void changeFont();

	void makeTextTexture();

private:
	// parses out the color
	std::string ParseTextString(std::string text);
	glm::vec4 ParseColor(const std::string& color);

	vector getFBOSize();

	static inline std::vector<text*> instances;

	std::string textString;
	// if the FBO should be updated
	// set true if text was changed while not on the main thread
	bool updateWhileOnMain = false; 

	// static list of text imgs
	std::vector<std::unique_ptr<Image>> letters;

	TextAlign alignment;
	std::string font;

	bool isometric = false;
	bool useWorldPos = false;

	float lineLength = -1;

	// start of word
	int start = 0;
	int numLetters = 0;

	std::shared_ptr<Image> textImg;

	FfontInfo* fontInfo = nullptr;

	glm::vec4 colorMod = glm::vec4(1.f);

	std::unique_ptr<FBO> fbo;

	vector absoluteLoc;
	Anchor xAnchor = ANCHOR_LEFT;
	Anchor yAnchor = ANCHOR_TOP;

	bool hasDropChar = false;

	// index, color
	std::vector<std::pair<int, glm::vec4>> colorList;
};