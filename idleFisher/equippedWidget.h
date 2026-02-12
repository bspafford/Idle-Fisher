#pragma once

#include "widget.h"

class Ubutton;
class text;
class UhoverBox;

class UequippedWidget : public widget {
public:
	UequippedWidget(widget* parent);
	~UequippedWidget();
	void draw(Shader* shaderProgram) override;
	vector getSize() override;
	void setLoc(vector loc) override;
	void update();
	bool mouseOverSlot(std::string &name, std::string &description);
private:
	//std::unique_ptr<Image> charImg;

	// fishing equipped
	std::unique_ptr<Image> fishingRodEquipSlot;
	std::unique_ptr<Image> baitEquipSlot;
	std::unique_ptr<Image> petEquipSlot;
	// clothes equipped
	//std::unique_ptr<Image> headEquipSlot;
	//std::unique_ptr<Image> shirtEquipSlot;
	//std::unique_ptr<Image> legsEquipSlot;
	//std::unique_ptr<Image> shoesEquipSlot;

	// fishing thumbnails
	std::unique_ptr<Image> fishingRodEquip;
	std::unique_ptr<Image> baitEquip;
	std::unique_ptr<Image> petEquip;
	// clothes thumbnails
	//std::unique_ptr<Image> headEquip;
	//std::unique_ptr<Image> shirtEquip;
	//std::unique_ptr<Image> legsEquip;
	//std::unique_ptr<Image> shoesEquip;
};