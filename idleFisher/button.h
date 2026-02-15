#pragma once

#include <string>
#include <vector>
#include <stdio.h>
#include <cassert>
#include <functional>

#include "widget.h"
#include "Hoverable.h"
#include "math.h"
#include "audio.h"

class animation;
class Background;

class Ubutton : public widget, public IHoverable {
public:
	// uses custom sprites
	Ubutton(widget* parent, std::string spriteSheetPath, int cellWidth, int cellHeight, int numberOfFrames, vector loc, bool useWorldLoc, bool useAlpha);
	// uses a background component that can be resized
	Ubutton(widget* parent, vector size);

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

	void SetClickAudio(std::string path);

	void DisableDistanceCheck(bool disable);

private:
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

	std::unique_ptr<Audio> clickAudio;
	std::unique_ptr<Background> background;

	// whether to check if the character is close enough for world space buttons
	bool distanceCheck = true;

public:
	std::shared_ptr<animation> buttonAnim;
};
