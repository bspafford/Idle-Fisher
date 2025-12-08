#pragma once

#include "widget.h"
#include "Hoverable.h"

class text;

class Uslider : public widget, public IHoverable {
public:
	Uslider(widget* parent, bool useCharLoc, vector size, float minVal, float maxVal, bool showValueText = true);
	~Uslider();
	void setBackgroundColor(glm::vec4 color);
	void setForegroundColor(glm::vec4 color);
	void draw(Shader* shaderProgram) override;
	void setLoc(vector loc) override;
	void setSize(vector size) override;
	void setSliderTitle(std::string title); // gives a title on the left size of the progress bar
	void bindValue(float* value);
	// tells the widget how big the title should be, so it can do alignments with other items
	// 0 to go off actual title length
	void setTitleLength(float titleLength);

	bool mouseOver() override;

	void UpdateValue();

private:
	std::unique_ptr<text> sliderTitle;
	std::unique_ptr<text> sliderValueText; // text to right of slider

	bool useCharLoc;
	float* value;
	float minVal;
	float maxVal;
	std::unique_ptr<URectangle> foreground;
	std::unique_ptr<URectangle> background;
	std::unique_ptr<Image> handle;

	bool sliding = false;
	float defaultVal;
	float titleLength = -1;
};