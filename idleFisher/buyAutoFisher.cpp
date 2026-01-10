#include "buyAutoFisher.h"

#include "main.h"
#include "Input.h"
#include "worlds.h"
#include "shortNumbers.h"
#include "character.h"
#include "AautoFisher.h"
#include "animation.h"
#include "collision.h"
#include "fishTransporter.h"

// widget
#include "currencyWidget.h"
#include "text.h"

#include "debugger.h"

buyAutoFisher::buyAutoFisher(vector loc) {
	std::unordered_map<std::string, animDataStruct> plusData;
	plusData.insert({ "normal", animDataStruct({0, 0}, {7, 0}, true) });
	plusData.insert({ "hover", animDataStruct({0, 1}, {7, 1}, true) });
	plusAnim = std::make_unique<animation>("autoFisher/buyer/buyer.png", 26, 46, plusData, true);
	plusAnim->setAnimation("normal");
	plusAnim->start();

	price = calcPrice();
	priceText = std::make_unique<text>(nullptr, shortNumbers::convert2Short(price), "bold", loc, true, true);

	updateLoc();
	setupCollision();
}

buyAutoFisher::~buyAutoFisher() {
	// deletes collision from list
	collision::removeCollisionObject(collision.get());
}

void buyAutoFisher::draw(Shader* shaderProgram) {
	if (calcMaxAutoFishers() || !plusAnim)
		return;

	bool prevMouseOver = bMouseOver;
	bMouseOver = plusAnim->IsMouseOver(false);
	if (bMouseOver)
		IHoverable::setHoveredItem(this);
	if (!prevMouseOver && bMouseOver) {
		plusAnim->setAnimation("hover");
	} else if (prevMouseOver && !bMouseOver) {
		plusAnim->setAnimation("normal");
	}

	if (bMouseOver && Input::getMouseButtonDown(MOUSE_BUTTON_LEFT) && !widget::getCurrWidget())
		Input::setLeftClick(this, &buyAutoFisher::spawnAutoFisher);

	calcIfPlayerInFront();
	plusAnim->draw(shaderProgram);
	priceText->draw(shaderProgram);
}

void buyAutoFisher::spawnAutoFisher() {
	if (calcMaxAutoFishers())
		return;

	if (!hasCurrency())
		return;


	price = calcPrice();
	SaveData::saveData.currencyList[1].numOwned -= price;
	Main::currencyWidget->updateList();

	//vector loc = Main::autoFisherLocs[(int)Main::autoFisherList.size()];
	std::unique_ptr<AautoFisher> autoFisher = std::make_unique<AautoFisher>(calcAutoFisherId()); // int(world::autoFisherList.size())
	world::currWorld->autoFisherList.push_back(std::move(autoFisher));

	// need to update price after its leveled up
	priceText->setText(shortNumbers::convert2Short(calcPrice()));
	updateLoc();

	if (world::currWorld->fishTransporter)
		world::currWorld->fishTransporter->startPathFinding();

	if (calcMaxAutoFishers()) {
		std::cout << "max autofishers" << std::endl;
		collision::removeCollisionObject(collision.get());
	}
}

int buyAutoFisher::calcAutoFisherId() {
	int autoFisherNumPerWorld = 0;
	int autoFisherWorldNum = 0;
	std::string currWorldName = Scene::getCurrWorldName();
	for (FautoFisherStruct autoFisher : SaveData::data.autoFisherData) {
		if (autoFisher.worldName == "world1")
			autoFisherNumPerWorld++;

		if (autoFisher.worldName == currWorldName && SaveData::saveData.autoFisherList[autoFisher.id].unlocked)
			autoFisherWorldNum++;
	}

	std::string stringNum = currWorldName.substr(std::string("world").size(), currWorldName.size());
	int worldMultiplier = (std::stoi(stringNum) - 1) * autoFisherNumPerWorld + autoFisherWorldNum;

	return worldMultiplier;
}

double buyAutoFisher::calcPrice() {
	// 10 * 1.6^x
	return 10 * pow(1.6, (int)world::currWorld->autoFisherList.size());
}

bool buyAutoFisher::hasCurrency() {
	if (SaveData::saveData.currencyList[1].numOwned >= calcPrice())
		return true;
	return false;
}

void buyAutoFisher::updateLoc() {
	if (calcMaxAutoFishers()) {
		return;
	}

	if (plusAnim) {
		plusAnim->setLoc({ SaveData::data.autoFisherData[int(world::currWorld->autoFisherList.size())].xLoc + 4, SaveData::data.autoFisherData[int(world::currWorld->autoFisherList.size())].yLoc + 2 });
		priceText->setLoc(plusAnim->getLoc() + vector{ 0, plusAnim->GetCellSize().y - 6.f * stuff::pixelSize});
	}


	price = calcPrice();
	priceText->setText(shortNumbers::convert2Short(price));

	setupCollision();
}

void buyAutoFisher::calcIfPlayerInFront() {
	vector loc = plusAnim->getLoc();
	inFrontPlayer = GetCharacter()->getCharLoc().y >= loc.y + 6.f;
}

void buyAutoFisher::setupCollision() {
	if (calcMaxAutoFishers() || !plusAnim)
		return;

	std::vector<vector> points = std::vector<vector>(4);
	for (int i = 0; i < 4; i++)
		points[i] = { collisionPoints[i].x + plusAnim->getLoc().x, collisionPoints[i].y + plusAnim->getLoc().y};

	if (!collision) {
		collision = std::make_unique<Fcollision>(points, ' ');
		collision::addCollisionObject(collision.get());
	} else {
		// find collision in list, then replace it
		// reseting it this way cause i need the min and max values to update along with the points
		std::unique_ptr<Fcollision> newCol = std::make_unique<Fcollision>(points, ' ');
		collision::replaceCollisionObject(collision.get(), newCol.get());
		collision = std::move(newCol);
	}
}

bool buyAutoFisher::calcMaxAutoFishers() {
	int maxAutoFishers = 0;
	for (FautoFisherStruct autoFisher : SaveData::data.autoFisherData)
		if (autoFisher.worldName == Scene::getCurrWorldName())
			maxAutoFishers++;

	return world::currWorld->autoFisherList.size()  >= maxAutoFishers;
}