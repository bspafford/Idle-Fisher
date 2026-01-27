#pragma once

#include "widget.h"
#include "saveData.h"

class Ubutton;
class text;
class UheldFishWidget;
class UscrollBox;

class UidleProfitWidget : public widget {
public:
	UidleProfitWidget(widget* parent);
	~UidleProfitWidget();
	void draw(Shader* shaderProgram) override;
	void setup(std::unordered_map<uint32_t, FsaveFishData> fishList);
private:
	void setupLocs() override;

	// components
	std::unique_ptr<Image> background;
	std::unique_ptr<Ubutton> collectButton;
	std::unique_ptr<text> title;
	std::unique_ptr<text> collectText;

	std::unique_ptr<UheldFishWidget> heldFishWidget;
};