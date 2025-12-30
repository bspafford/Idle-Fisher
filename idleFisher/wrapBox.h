#pragma once

#include "widget.h"

class UwrapBox : public widget {
public:
	UwrapBox(widget* parent, vector loc, vector size);

	void draw(Shader* shaderProgram);
	void addChild(widget* child);
	widget* getChildAt(int index);
	std::vector<widget*> getAllChildren();

	void setLoc(vector loc) override;

	float getOverflowSize();

private:
	void UpdateChildren();

	std::vector<widget*> childrenList;

	float overflowSizeY = 0.f;
};