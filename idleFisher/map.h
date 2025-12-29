#pragma once

#include "widget.h"
#include "hoverable.h"

class Ubutton;
class text;
class animation;
class UsailorWidget;

class Umap : public widget, public IHoverable {
public:
	Umap(UsailorWidget* parent, vector mapSize);
	void draw(Shader* shaderProgram) override;

	void setLoc(vector loc) override;
	vector getLoc() override;

	// centers map on current world
	void SetCurrWorldToCenter();

private:
	void moveMap();
	void setLocs(vector loc);

	UsailorWidget* sailorWidgetParent;

	bool mouseDown = false;
	bool prevMouseDown = false;
	bool movingMap = false;
	vector mouseStartPos;
	vector imgStartPos;

	std::unique_ptr<Image> mapImg;
	
	std::unique_ptr<animation> hereBoat;
	std::unique_ptr<text> hereText;

	std::vector< std::unique_ptr<Ubutton>> worldButtons;
	std::vector<vector> worldButtonLoc = { {401.f, 725.f}, {398.f, 583.f}, {535.f, 566.f}, {639.f, 630.f}, {775.f, 479.f}, {705.f, 401.f}, {711.f, 334.f}, {567.f, 383.f}, {395.f, 349.f}, {425.f, 415.f} };
	std::vector<std::unique_ptr<text>> worldNames;
	std::vector<std::unique_ptr<Image>> worldLines;
	std::vector<vector> worldLineLoc = { {398.f, 609.f}, {425.f, 588.f}, {557.f, 584.f}, {619.f, 434.f}, {722.f, 423.f}, {721.f, 358.f}, {588.f, 323.f}, {419.f, 344.f}, {390.f, 376.f} };

	void openWorld1() { std::cout << "is it working?" << std::endl; openLevel("world1"); }
	void openWorld2() { openLevel("world2"); }
	void openWorld3() { openLevel("world3"); }
	void openWorld4() { openLevel("world4"); }
	void openWorld5() { openLevel("world5"); }
	void openWorld6() { openLevel("world6"); }
	void openWorld7() { openLevel("world7"); }
	void openWorld8() { openLevel("world8"); }
	void openWorld9() { openLevel("world9"); }
	void openWorld10() { openLevel("world10"); }
	void openLevel(std::string levelName);
};