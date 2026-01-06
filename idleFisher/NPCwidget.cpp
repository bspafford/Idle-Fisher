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
	closeButton->SetPivot({ 0.5f, 0.5f });
	
	if (npcName != "fishGod") {
		npcImg = std::make_unique<Image>("images/widget/npcButtons/" + npcName + ".png", vector{ 0, 0 }, false);
		npcImg->SetPivot({ 0.5f, 0.f });
	}

	name = std::make_unique<text>(this, " ", "biggerStraightDark", vector{ 0,0 });
	description = std::make_unique<text>(this, " ", "straightDark", vector{ 0,0 });
	nameHolder = std::make_unique<verticalBox>(this);
	nameHolder->addChild(name.get(), 8.f);
	nameHolder->addChild(description.get(), 4.f);

	npcBackground = std::make_unique<Image>("images/widget/npcBackground.png", vector{ 0, 0 }, false);
	npcBackground->SetAnchor(ANCHOR_CENTER, ANCHOR_CENTER);
	npcBackground->SetPivot({ 1.f, 0.f });
	infoBackground = std::make_unique<Image>("images/widget/infoBackground.png", vector{ 0, 0 }, false);
	infoBackground->SetAnchor(ANCHOR_CENTER, ANCHOR_CENTER);
	infoBackground->SetPivot({ 1.f, 1.f });
	upgradeBackground = std::make_unique<Image>("images/widget/upgradeBackground.png", vector{ 0, 0 }, false);
	upgradeBackground->SetAnchor(ANCHOR_CENTER, ANCHOR_CENTER);
	upgradeBackground->SetPivot({ 0.f, 0.5f });

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
	nameHolder->changeChildHeight(name.get(), name->getSize().y + 1.f);
	nameHolder->changeChildHeight(description.get(), description->getSize().y + 1.f);
}

void NPCwidget::setupLocs() {
	__super::setupLocs();

	float widgetWidth = npcBackground->getSize().x + upgradeBackground->getSize().x;
	vector center = vector{ widgetWidth / 2.f - upgradeBackground->getSize().x, 0.f};

	npcBackground->setLoc(center + vector{ -1.f, 1.f });
	infoBackground->setLoc(center + vector{ -1.f, -1.f });
	upgradeBackground->setLoc(center + vector{ 1.f, 1.f });

	if (npcImg)
		npcImg->setLoc(npcBackground->getAbsoluteLoc() + vector{ npcBackground->getSize().x / 2.f, 3.f});

	vector upgradeHolderPos = (upgradeBackground->getAbsoluteLoc() + vector{ 4, 3 }).floor();
	upgradeHolder->setLocAndSize(upgradeHolderPos, upgradeBackground->getSize() - vector{ 8.f, 6.f });

	if (closeButton) {
		vector closeButtonSize = closeButton->getSize();
		closeButton->setLoc(upgradeBackground->getAbsoluteLoc() + upgradeBackground->getSize());
	}

	if (infoBackground) {
		vector nameHolderSize = infoBackground->getSize() - 10.f;
		name->setLineLength(nameHolderSize.x);
		description->setLineLength(nameHolderSize.x);
		nameHolder->setLocAndSize(infoBackground->getAbsoluteLoc() + 5.f, nameHolderSize);
	}
}