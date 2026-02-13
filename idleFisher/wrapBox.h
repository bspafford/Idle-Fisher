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

	void UpdateChildren();

	void SetPadding(vector padding);

private:
	std::vector<widget*> childrenList;

	float overflowSizeY = 0.f;

	vector padding = 0.f;
};