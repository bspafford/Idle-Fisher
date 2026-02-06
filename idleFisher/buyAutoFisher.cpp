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
#include "achievement.h"

// widget
#include "currencyWidget.h"
#include "text.h"

#include "debugger.h"

buyAutoFisher::buyAutoFisher(vector loc) {
	std::unordered_map<std::string, animDataStruct> plusData;
	plusData.insert({ "normal", animDataStruct({0, 0}, {7, 0}, true) });
	plusData.insert({ "hover", animDataStruct({0, 1}, {7, 1}, true) });
	plusAnim = std::make_unique<animation>("autoFisher/buyer.png", 26, 46, plusData, true);
	plusAnim->addFrameCallback(this, &buyAutoFisher::AnimUpdate);
	plusAnim->setAnimation("normal");
	plusAnim->start();

	price = calcPrice();
	priceText = std::make_unique<text>(nullptr, shortNumbers::convert2Short(price), "bold", loc, true, true);
	priceText->SetPivot(vector(0.5f, 0.f));

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

	if (SaveData::saveData.currencyList.at(53u).numOwned >= calcPrice())
		priceText->setTextColor(glm::vec4(1));
	else
		priceText->setTextColor(glm::vec4(1, 0, 0, 1));

	priceText->draw(shaderProgram);
}

void buyAutoFisher::spawnAutoFisher() {
	if (calcMaxAutoFishers())
		return;

	if (!hasCurrency())
		return;


	price = calcPrice();
	SaveData::saveData.currencyList.at(Scene::GetCurrWorldId()).numOwned -= price;
	Main::currencyWidget->updateList();

	//vector loc = Main::autoFisherLocs[(int)Main::autoFisherList.size()];
	std::unique_ptr<AautoFisher> autoFisher = std::make_unique<AautoFisher>(calcAutoFisherId()); // int(world::autoFisherList.size())
	SaveData::saveData.progressionData.at(autoFisher->id).level++; // level should be 1 when purchased
	autoFisher->startFishing();

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

	Achievements::CheckGroup(AchievementTrigger::PurchaseAutoFisher);
}

uint32_t buyAutoFisher::calcAutoFisherId() {
	int autoFisherNumPerWorld = 0;
	int autoFisherWorldNum = 0;
	uint32_t currWorld = Scene::GetCurrWorldId();
	for (uint32_t afId : SaveData::orderedData.autoFisherData) {
		ProgressionNode& afData = SaveData::data.progressionData.at(afId);
		if (afData.worldId == 53u) // world1
			autoFisherNumPerWorld++;

		if (afData.worldId == currWorld && SaveData::saveData.progressionData.at(afId).level)
			autoFisherWorldNum++;
	}

	uint32_t id = SaveData::orderedData.autoFisherData.at(Scene::GetWorldIndex() * autoFisherNumPerWorld + autoFisherWorldNum);
	return id;
}

double buyAutoFisher::calcPrice() {
	return (1000 + 500 * world::currWorld->autoFisherList.size()) * pow(3, (int)world::currWorld->autoFisherList.size());
}

bool buyAutoFisher::hasCurrency() {
	if (SaveData::saveData.currencyList.at(Scene::GetCurrWorldId()).numOwned >= calcPrice())
		return true;
	return false;
}

void buyAutoFisher::updateLoc() {
	if (calcMaxAutoFishers()) {
		return;
	}

	if (plusAnim) { // go to next location
		uint32_t nextAfId = 0;
		if (world::currWorld->autoFisherList.size() > 0) {
			uint32_t lastAfId = world::currWorld->autoFisherList.back()->id;
			for (int i = 0; i < SaveData::orderedData.autoFisherData.size(); i++) {
				uint32_t id = SaveData::orderedData.autoFisherData[i];
				if (id == lastAfId) {
					nextAfId = SaveData::orderedData.autoFisherData[i + 1];
				}
			}
		} else {
			nextAfId = SaveData::orderedData.autoFisherData[0]; // get first auto fisher
		}

		FautoFisherStruct& afData = SaveData::data.autoFisherData.at(nextAfId);
		plusAnim->setLoc(vector(afData.xLoc, afData.yLoc));
		AnimUpdate(plusAnim->GetCurrFrameLoc().x);
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
	for (uint32_t afId : SaveData::orderedData.autoFisherData) {
		ProgressionNode& afData = SaveData::data.progressionData.at(afId);
		if (afData.worldId == Scene::GetCurrWorldId())
			maxAutoFishers++;
	}

	return world::currWorld->autoFisherList.size()  >= maxAutoFishers;
}

void buyAutoFisher::AnimUpdate(int frame) {
	if (!priceText)
		return;

	std::vector<float> y{ 0, 2, 3, 2, 0, -2, -3, -2 };
	priceText->setLoc(plusAnim->getLoc() + vector(15.f, 13.f + y[frame]));
}