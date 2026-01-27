#pragma once

#include "widget.h"

class text;
class Ubutton;
struct FrebirthStruct;
struct SaveEntry;

class UrebirthUnlock : public widget {
public:
	UrebirthUnlock(widget* parent, uint32_t id);
	~UrebirthUnlock();
	void draw(Shader* shaderProgram) override;
	bool isMouseOver();
	void setLoc(vector loc) override;
	vector getSize() override;

private:
	void unlock();
	void onClick();
	bool prerequisitesMet();

	std::unique_ptr<Image> background;
	std::unique_ptr<Ubutton> button;
	std::unique_ptr<text> upgradeCost;

	FrebirthStruct* rebirthInfo;
	SaveEntry* saveRebirthInfo;
};