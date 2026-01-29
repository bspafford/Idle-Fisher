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

	std::unordered_map<uint32_t, std::pair<std::unique_ptr<Ubutton>, std::unique_ptr<text>>> worldButtons;
	std::unordered_map<uint32_t, vector> worldButtonLoc = { { 53u, vector(401.f, 725.f) },
															{ 54u, vector(398.f, 583.f) },
															{ 55u, vector(535.f, 566.f) },
															{ 56u, vector(639.f, 630.f) },
															{ 57u, vector(775.f, 479.f) },
															{ 58u, vector(705.f, 401.f) },
															{ 59u, vector(711.f, 334.f) },
															{ 60u, vector(567.f, 383.f) },
															{ 61u, vector(395.f, 349.f) },
															{ 62u, vector(425.f, 415.f) }};

	std::unordered_map<uint32_t, std::unique_ptr<Image>> worldLines;
	std::unordered_map<uint32_t, vector> worldLineLoc = { { 54u, vector(398.f, 609.f) },
														  { 55u, vector(425.f, 588.f) },
														  { 56u, vector(557.f, 584.f) },
														  { 57u, vector(619.f, 434.f) },
														  { 58u, vector(722.f, 423.f) },
														  { 59u, vector(721.f, 358.f) },
														  { 60u, vector(588.f, 323.f) },
														  { 61u, vector(419.f, 344.f) },
														  { 62u, vector(390.f, 376.f) } };

	void openWorld1() { openLevel(53u); }
	void openWorld2() { openLevel(54u); }
	void openWorld3() { openLevel(55u); }
	void openWorld4() { openLevel(56u); }
	void openWorld5() { openLevel(57u); }
	void openWorld6() { openLevel(58u); }
	void openWorld7() { openLevel(59u); }
	void openWorld8() { openLevel(60u); }
	void openWorld9() { openLevel(61u); }
	void openWorld10() { openLevel(62u); }
	void openLevel(uint32_t levelId);
};