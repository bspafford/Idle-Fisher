#pragma once

#include "widget.h"

#include <vector>

struct vertChildComp {
	widget* child;
	float widgetHeight;
};

class verticalBox : public widget {
public:
	verticalBox(widget* parent) : widget(parent) {}

	virtual void draw(Shader* shaderProgram) override;

	void addChild(widget* child, float widgetHeight);
	void removeChild(widget* child);
	void removeChild(int index);
	void RemoveAllChildren();
	float getOverflowSize();
	void changeChildHeight(widget* child, float newHeight);

	void setLocAndSize(vector loc, vector size) override;
	void setOgLoc(vector ogLoc) override;
	vertChildComp GetChildAt(int index);
	int GetChildrenCount();

	void UpdateChildren();

protected:
	std::vector<vertChildComp> childrenList;

	// is the size of the components inside the widget
	float overflowSizeY = 0;
};
