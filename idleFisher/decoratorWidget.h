#pragma once

#include "widget.h"

class UscrollBox;
class Adecorator;
class Ubutton;
class text;
class verticalBox;
class UwrapBox;
class npc;
struct FvaultUnlocksStruct;
struct FsaveVaultUnlocksStruct;
class URectangle;

class UdecoratorWidget : public widget {
public:
	UdecoratorWidget(widget* parent, Adecorator* decoratorParent);
	~UdecoratorWidget();

	void draw(Shader* shaderProgram) override;
	void setupLocs() override;
	void addedToViewport() override;
	void placeItem(FvaultUnlocksStruct* vaultUnlock, FsaveVaultUnlocksStruct* saveVaultUnlock);
	void removeItem(FvaultUnlocksStruct* vaultUnlock, FsaveVaultUnlocksStruct* saveVaultUnlock);
	void moveMode();

private:
	void finishPlaceMode();

	npc* parent;

	// upgrades
	std::unique_ptr<UscrollBox> scrollBox;
	std::unique_ptr<UwrapBox> wrapBox;
	std::unique_ptr<URectangle> upgradeBackground;

	// stats
	std::unique_ptr<verticalBox> nameHolder;
	std::unique_ptr<text> name;
	std::unique_ptr<text> description;
	std::unique_ptr<URectangle> infoBackground;

	// npc img
	std::unique_ptr<Image> npcImg;
	std::unique_ptr<URectangle> npcBackground;

	std::unique_ptr<Ubutton> closeButton;

	// placing items
	std::unique_ptr<Ubutton> finishPlacingButton;
	std::unique_ptr<Ubutton> moveButton;
};