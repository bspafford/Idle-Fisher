#include "NPCwidget.h"

#include "main.h"
#include "npc.h"

#include "verticalBox.h"
#include "text.h"
#include "button.h"

#include "image.h"

#include "debugger.h"

NPCwidget::NPCwidget(widget* parent, npc* NPCParent, std::string npcName) : widget(parent) {
	this->NPCParent = NPCParent;

	upgradeHolder = std::make_unique<UscrollBox>(this);
	
	closeButton = std::make_unique<Ubutton>(this, "widget/npcXButton.png", 11, 11, 1, vector{ 0, 0 }, false, false);
	closeButton->addCallback<widget>(this, &NPCwidget::removeFromViewport);
	
	if (npcName != "fishGod")
		npcImg = std::make_unique<Image>("./images/widget/npcButtons/" + npcName + ".png", vector{ 0, 0 }, false);

	name = std::make_unique<text>(this, " ", "biggerStraightDark", vector{ 0,0 });
	description = std::make_unique<text>(this, " ", "straightDark", vector{ 0,0 });
	nameHolder = std::make_unique<verticalBox>(this);
	nameHolder->addChild(name.get(), 8 * stuff::pixelSize);
	nameHolder->addChild(description.get(), 4 * stuff::pixelSize);

	npcBackground = std::make_unique<Image>("./images/widget/npcBackground.png", vector{ 0, 0 }, false);
	infoBackground = std::make_unique<Image>("./images/widget/infoBackground.png", vector{ 0, 0 }, false);
	upgradeBackground = std::make_unique<Image>("./images/widget/upgradeBackground.png", vector{ 0, 0 }, false);

	setupLocs();
}

NPCwidget::~NPCwidget() {

}

void NPCwidget::draw(Shader* shaderProgram) {
	__super::draw(shaderProgram);

	if (!visible)
		return;

	upgradeBackground->draw(shaderProgram);
	npcBackground->draw(shaderProgram);
	if (infoBackground)
		infoBackground->draw(shaderProgram);

	if (npcImg)
		npcImg->draw(shaderProgram);

	nameHolder->draw(shaderProgram);
	upgradeHolder->draw(shaderProgram);

	if (closeButton)
		closeButton->draw(shaderProgram);
}

void NPCwidget::setNameDescription(std::string nameString, std::string descriptionString) {
	name->setText(nameString);
	description->setText(descriptionString);

	// change nameHolder sizes
	nameHolder->changeChildHeight(name.get(), name->getSize().y + stuff::pixelSize);
}

void NPCwidget::setupLocs() {
	__super::setupLocs();

	float x = (npcBackground->w + 1) * stuff::pixelSize;
	float y = (npcBackground->h + 1) * stuff::pixelSize;
	vector size = vector{ x, 0 } + upgradeBackground->getSize();
	vector center = { stuff::screenSize.x / 2, stuff::screenSize.y / 2 };
	vector topLeft = center - size / 2;
	npcBackground->setLoc(topLeft);
	if (infoBackground)
		infoBackground->setLoc(topLeft + vector{ 0, y });
	upgradeBackground->setLoc(topLeft + vector{ x, 0 });

	if (npcImg) {
		vector npcBackgroundSize = npcBackground->getSize();
		vector npcSize = npcImg->getSize();
		npcImg->setLoc(npcBackground->getLoc() + vector{ npcBackgroundSize.x / 2, npcBackgroundSize.y } - vector{ npcSize.x / 2, npcSize.y } - vector{ 0, 1 * stuff::pixelSize });
	}


	vector upgradeHolderPos = (upgradeBackground->getLoc() + vector{ 4, 3 } * stuff::pixelSize).floor();
	upgradeHolder->setLocAndSize(upgradeHolderPos, vector{float(upgradeBackground->w), float(upgradeBackground->h - 6)} *stuff::pixelSize);
	upgradeHolder->setOgLoc(upgradeHolderPos);

	if (closeButton) {
		vector closeButtonSize = closeButton->getSize();
		closeButton->setLoc({ float(upgradeBackground->getLoc().x + upgradeBackground->w * stuff::pixelSize - closeButtonSize.x / 2), float(upgradeBackground->getLoc().y - closeButtonSize.y / 2)});
	}

	if (infoBackground) {
		nameHolder->setLocAndSize({ float(infoBackground->getLoc().x) + 6 * stuff::pixelSize, float(infoBackground->getLoc().y) + 9 * stuff::pixelSize}, vector{float(infoBackground->w), float(infoBackground->h)} *stuff::pixelSize);
		name->setLineLength((infoBackground->w - 10) * stuff::pixelSize);
		description->setLineLength((infoBackground->w - 10) * stuff::pixelSize);
	}
}