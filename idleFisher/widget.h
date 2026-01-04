#pragma once

#include <set>

#include "math.h"
#include "shaderClass.h"
#include "Image.h"

class widget {
protected:
	widget(widget* parent);
	virtual ~widget();

	bool visible = true;

	static inline std::set<widget*> instances;

	// called when resizing screen
	virtual void setupLocs();

	// calls when widget is added to viewport
	virtual void addedToViewport();

	vector loc = { 0, 0 };
	vector absoluteLoc = { 0, 0 };
	// the original location of a widget, used for scrollboxes and wrapboxes
	vector ogLoc = { 0, 0 };
	vector size = { 0, 0 };
	// where (0, 0) is relative to the object
	// pivot (0, 0): is bottom left, (1, 1): is top right
	vector pivot = { 0, 0 };
	Anchor xAnchor = ANCHOR_LEFT;
	Anchor yAnchor = ANCHOR_BOTTOM;

public:
	virtual void draw(Shader* shaderProgram);

	// parent: which widget added the viewport to the screen
	virtual void addToViewport(widget* parent);

	bool isVisible();
	void setVisibility(bool visible);

	virtual void removeFromViewport();

	virtual bool mouseOver();

	static void resizeScreen();

	// loc and size
	virtual vector getLoc();
	virtual vector getOgLoc();
	virtual vector getAbsoluteLoc();
	virtual vector getSize();
	virtual void setLoc(vector loc);
	virtual void setOgLoc(vector loc);
	virtual void setSize(vector size);
	virtual void setLocAndSize(vector loc, vector size);
	void SetAnchor(Anchor xAnchor, Anchor yAnchor);
	void SetPivot(vector pivot);

	// returns the override widget if there is one
	static widget* getCurrWidget();

	widget* getParent();
	void setParent(widget* newParent);
	// returns root parent variable
	widget* getRootParent();
	void updateAllChildrensRootParent(widget* curr, widget* newRootParent);

private:
	widget* parent = nullptr;
	widget* rootParent = nullptr;
	// which widget added me to the viewport
	widget* viewportParent = nullptr;
	// calculates the root parent from this objects position
	widget* recalcRootParent();
	void setRootParent(widget* rootParent);
	std::vector<widget*> children;

	// is the overriding widget that is open on screen
	// something like the journal, NPC menus, settings, etc
	static inline widget* currWidget;
};