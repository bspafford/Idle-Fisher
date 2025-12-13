#pragma once

#include "widget.h"

class text;
class Ubutton;
struct FrebirthStruct;
struct FsaveRebirthStruct;

class UrebirthUnlock : public widget {
public:
	UrebirthUnlock(widget* parent, int id);
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
	FsaveRebirthStruct* saveRebirthInfo;
};