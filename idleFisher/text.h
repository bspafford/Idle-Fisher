#pragma once

#include <string>
#include <vector>
#include <mutex>

#include "math.h"
#include "widget.h"
#include "Image.h"

enum TextAlign {
	TEXT_ALIGN_LEFT,
	TEXT_ALIGN_RIGHT,
	TEXT_ALIGN_CENTER,
};

struct FtextInfo {
	vector loc = { 0, 0 };
	vector size = { 0, 0 };
};

class text : public widget {
public:
	text(widget* parent, std::string text, std::string font, vector loc, bool useWorldPos = false, bool isometric = false, TextAlign alignment = TEXT_ALIGN_LEFT);
	~text();
	void LoadGPU();
	void setText(std::string text);
	void draw(Shader* shader);
	// void setAlignment(std::string alignment);
	void makeText(int i, std::string text, vector &offset);
	// includeDanglingCharSizes is whether it should count the size of g, y, j, etc
	// this is required for things like fbo sizing
	vector getSize();
	std::string getString();
	void loadTextImg();
	void setTextColor(int r, int g, int b);
	void setupLocs() override;

	// Wraps text if it goes above line length, set to -1 if text doesn't wrap
	void setLineLength(float length);
	float getLineLength();

	void setLocAndSize(vector loc, vector size) override;
	void setLoc(vector loc) override;
	// anchor::...
	void setAnchor(ImageAnchor xAnchor, ImageAnchor yAnchor);

	// used when changing the font from pixel to normal or vice versa
	static void changeFontAll();
	void changeFont();

	void makeTextTexture();
private:
	void UpdateGPUData(GLint preboundFBO);

	vector getFBOSize();

	static inline std::vector<text*> instances;

	std::string textString;
	std::string futureTextString; // keeps track if text was changed while on another thread

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
	std::vector<FtextInfo> textInfo = std::vector<FtextInfo>(126);

	glm::vec4 colorMod = glm::vec4(1.f);

	// text fbo
	GLuint fbo = 0;
	GLuint textTexture;
	GLuint64 handle = 0;
	vector fboSize = { 1, 1 };

	vector absoluteLoc;
	ImageAnchor xAnchor = IMAGE_ANCHOR_LEFT;
	ImageAnchor yAnchor = IMAGE_ANCHOR_TOP;

	int textHeight;
};