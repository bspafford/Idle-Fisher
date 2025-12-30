#pragma once

#include "verticalBox.h"
#include "Hoverable.h"

class UscrollBox : public verticalBox, public IHoverable {
public:
	UscrollBox(widget* parent) : verticalBox(parent) {}

	void draw(Shader* shaderProgram) override;

	bool mouseOver();
	void scrolling();
	void scrolling(int mouseWheelDir);

private:
	float clampY(float y);

	vector mouseStartPos;
	vector startLoc;

	bool scrollingActive = false;
};