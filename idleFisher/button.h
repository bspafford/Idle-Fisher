#pragma once

#include <string>
#include <vector>
#include <stdio.h>
#include <cassert>
#include <functional>

#include "widget.h"
#include "Hoverable.h"
#include "math.h"

class animation;

class Ubutton : public widget, public IHoverable {
public:
	Ubutton(widget* parent, std::string spriteSheetPath, int cellWidth, int cellHeight, int numberOfFrames, vector loc, bool useWorldLoc, bool useAlpha);
	~Ubutton();

	// non static
	template <class T> void addCallback(T* const object, void (T::* const callback) ()) {
		callback_ = std::bind_front(callback, object);
	}

	// static
	void addCallback(void (*callback) ());

	void draw(Shader* shaderProgram);

	void onHover(Shader* shaderProgram);
	bool isMouseOver();
	void onClick();

	void setLoc(vector loc) override;

	vector getSize();
	
	void enable(bool enabled);

	void SetColorMod(glm::vec4 colorMod);

private:
	// parent
	widget* widgetClass;

	std::function<void()> callback_;

	bool mouseOver = false;
	bool prevMouseOver = false;

	bool useWorldLoc = false;

	bool isEnabled = true;

	bool useAlpha;

	glm::vec4 colorMod;

	//
	bool hasHover;
	bool hasDisabled;

public:
	std::shared_ptr<animation> buttonAnim;
};
