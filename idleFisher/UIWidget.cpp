#include "UIWidget.h"
#include "main.h"
#include "saveData.h"
#include "shortNumbers.h"
#include "text.h"
#include "premiumBuffWidget.h"
#include "progressBar.h"
#include "button.h"
#include "worlds.h"
#include "timer.h"

#include "merchant.h"
#include "fisherman.h"
#include "mechanic.h"
#include "petSeller.h"
#include "sailor.h"
#include "atm.h"

#include "debugger.h"

UUIWidget::UUIWidget(widget* parent) : widget(parent) {
	progressBar = std::make_unique<UprogressBar>(this, vector{ 43.f, 6.f }, false);
	progressBar->setBackgroundColor(glm::vec4(0, 0, 0, 1.f));
	progressBar->setForegroundColor(glm::vec4(0, 1.f, 1.f, 1.f));
	currRunRebirthPoints = std::make_unique<text>(this, " ", "straight", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);
	rebirthPointNum = std::make_unique<text>(this, " ", "straight", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);

	NPCshowButton = std::make_unique<Ubutton>(this, "widget/npcButtons/shopIcon.png", 26, 22, 1, vector{ 0, 0 }, false, false);
	merchantButton = std::make_unique<Ubutton>(this, "widget/npcButtons/npcButton1.png", 18, 18, 1, vector{ 0, 0 }, false, false);
	sailorButton = std::make_unique<Ubutton>(this, "widget/npcButtons/npcButton2.png", 18, 18, 1, vector{ 0, 0 }, false, false);
	fishermanButton = std::make_unique<Ubutton>(this, "widget/npcButtons/npcButton3.png", 18, 18, 1, vector{ 0, 0 }, false, false);
	mechanicButton = std::make_unique<Ubutton>(this, "widget/npcButtons/npcButton4.png", 18, 18, 1, vector{ 0, 0 }, false, false);
	petSellerButton = std::make_unique<Ubutton>(this, "widget/npcButtons/npcButton5.png", 18, 18, 1, vector{ 0, 0 }, false, false);
	atmButton = std::make_unique<Ubutton>(this, "widget/npcButtons/npcButton1.png", 18, 18, 1, vector{ 0, 0 }, false, false);

	// set alpha to 0
	if (merchantButton) {
		merchantButton->SetColorMod(glm::vec4(0));
		merchantButton->enable(false);
		merchantButton->addCallback(this, &UUIWidget::openMerchantWidget);
	}
	if (sailorButton) {
		sailorButton->SetColorMod(glm::vec4(0));
		sailorButton->enable(false);
		sailorButton->addCallback(this, &UUIWidget::openSailorWidget);
	}
	if (fishermanButton) {
		fishermanButton->SetColorMod(glm::vec4(0));
		fishermanButton->enable(false);
		fishermanButton->addCallback(this, &UUIWidget::openFishermanWidget);
	}
	if (mechanicButton) {
		mechanicButton->SetColorMod(glm::vec4(0));
		mechanicButton->enable(false);
		mechanicButton->addCallback(this, &UUIWidget::openMechanicWidget);
	}
	if (petSellerButton) {
		petSellerButton->SetColorMod(glm::vec4(0));
		petSellerButton->enable(false);
		petSellerButton->addCallback(this, &UUIWidget::openPetSellerWidget);
	}
	if (atmButton) {
		atmButton->SetColorMod(glm::vec4(0));
		atmButton->enable(false);
		atmButton->addCallback(this, &UUIWidget::openAtmWidget);
	}

	if (NPCshowButton)
		NPCshowButton->addCallback(this, &UUIWidget::showNPCButtons);
	buttonsTimer = CreateDeferred<Timer>();
	buttonsTimer->addUpdateCallback(this, &UUIWidget::updateButtonsLoc);
	buttonsTimer->addCallback(this, &UUIWidget::finishButtons);

	setupLocs();
}

UUIWidget::~UUIWidget() {

}

void UUIWidget::draw(Shader* shaderProgram) {
	progressBar->draw(shaderProgram);
	currRunRebirthPoints->draw(shaderProgram);
	rebirthPointNum->draw(shaderProgram);

	int worldId = Scene::getWorldIndexFromName(Scene::getCurrWorldName());
	if (worldId != -1) {
		if (atmButton && SaveData::saveData.npcSave[worldId].atmDiscovered)
			atmButton->draw(shaderProgram);
		if (petSellerButton && SaveData::saveData.npcSave[worldId].petSellerDiscovered)
			petSellerButton->draw(shaderProgram);
		if (mechanicButton && SaveData::saveData.npcSave[worldId].mechanicDiscovered)
			mechanicButton->draw(shaderProgram);
		if (fishermanButton && SaveData::saveData.npcSave[worldId].fishermanDiscovered)
			fishermanButton->draw(shaderProgram);
		if (sailorButton && SaveData::saveData.npcSave[worldId].sailorDiscovered)
			sailorButton->draw(shaderProgram);
		if (merchantButton && SaveData::saveData.npcSave[worldId].merchantDiscovered)
			merchantButton->draw(shaderProgram);
	}

	if (NPCshowButton)
		NPCshowButton->draw(shaderProgram);

	for (int i = 0; i < Main::premiumBuffList.size(); i++) {
		Main::premiumBuffList[i]->draw(shaderProgram);
	}
}

void UUIWidget::updateProgressBar(double points, double progress, double needed) {

	progressBar->setPercent(static_cast<float>(progress));

	std::string currPointString = shortNumbers::convert2Short(points);
	if (currRunRebirthPoints->getString() != currPointString)
		currRunRebirthPoints->setText(currPointString);
	std::string pointString = shortNumbers::convert2Short(SaveData::saveData.rebirthCurrency);
	if (rebirthPointNum->getString() != pointString)
		rebirthPointNum->setText(shortNumbers::convert2Short(SaveData::saveData.rebirthCurrency));
}

void UUIWidget::setupLocs() {
	vector size = progressBar->getSize();
	vector loc = { stuff::screenSize.x - size.x - 2.f, 2.f };
	progressBar->setLoc(loc);
	currRunRebirthPoints->setLoc(loc);
	rebirthPointNum->setLoc(vector{ stuff::screenSize.x - 1.f, loc.y + rebirthPointNum->getSize().y + 1.f });

	if (NPCshowButton) {
		NPCshowButton->setLoc({ 4, 4 });
		if (merchantButton)
			merchantButton->setLoc(NPCshowButton->getLoc());
		if (sailorButton)
			sailorButton->setLoc(NPCshowButton->getLoc());
		if (fishermanButton)
			fishermanButton->setLoc(NPCshowButton->getLoc());
		if (mechanicButton)
			mechanicButton->setLoc(NPCshowButton->getLoc());
		if (petSellerButton)
			petSellerButton->setLoc(NPCshowButton->getLoc());
		if (atmButton)
			atmButton->setLoc(NPCshowButton->getLoc());
	}

	float offset = stuff::screenSize.x;
	for (int i = 0; i < Main::premiumBuffList.size(); i++) {
		Main::premiumBuffList[i]->setLoc(vector{ offset, float(stuff::screenSize.y) } - Main::premiumBuffList[i]->getSize());
		offset -= Main::premiumBuffList[i]->getSize().x;
	}
}

void UUIWidget::showNPCButtons() {
	if (!canOpenClose)
		return;
	// show the buttons
	// make them slide out / animation
	// stay still, and make the buttons clickable?

	// maybe go away after not using them for a lil?
	canOpenClose = false;
	buttonsTimer->start(.5);
}

void UUIWidget::updateButtonsLoc() {
	// temp
	int worldId = Scene::getWorldIndexFromName(Scene::getCurrWorldName());
	if (worldId == -1)
		return;
	
	buttonsList.clear();

	if (SaveData::saveData.npcSave[worldId].merchantDiscovered)
		buttonsList.push_back(merchantButton.get());
	if (SaveData::saveData.npcSave[worldId].sailorDiscovered)
		buttonsList.push_back(sailorButton.get());
	if (SaveData::saveData.npcSave[worldId].fishermanDiscovered)
		buttonsList.push_back(fishermanButton.get());
	if (SaveData::saveData.npcSave[worldId].mechanicDiscovered)
		buttonsList.push_back(mechanicButton.get());
	if (SaveData::saveData.npcSave[worldId].petSellerDiscovered)
		buttonsList.push_back(petSellerButton.get());
	if (SaveData::saveData.npcSave[worldId].atmDiscovered)
		buttonsList.push_back(atmButton.get());

	vector startLoc = NPCshowButton->getLoc() + vector{ 8.f, 2.f };
	float length = 20.f;
	float end = buttonsList.size() * length + startLoc.x;
	for (int i = 0; i < buttonsList.size(); i++) {
		vector endLoc = vector{ length * (i+1), 0 } + startLoc;
		if (buttonsList[i]->getLoc().x >= endLoc.x && !open) {
			buttonsList[i]->setLoc(endLoc - vector{ 0, 1.f });
			continue;
		}

		float percent = buttonsTimer->getTime() / buttonsTimer->getMaxTime();
		if (open)
			percent = 1 - percent;

		// exponential
		float exp = percent * sqrt(percent); //percent * percent;

		exp = math::clamp(exp, 0, 1);
		float x = math::lerp(startLoc.x, end, exp);
		float y = startLoc.y;

		float distance = math::distance({ x, y }, endLoc);
		float distFromStart = 80.f;
		float startToEndDist = math::distance(startLoc, endLoc);
		float m = 1 / (startToEndDist - (startToEndDist - distFromStart));
		
		float newPercent = 1 - math::distance({ x, y }, endLoc) / math::distance(startLoc, endLoc);
		float alpha = -m * ((startToEndDist - distFromStart) - newPercent * startToEndDist);
		alpha = math::clamp(alpha, 0, 1);

		if ((x >= endLoc.x && open)) {
			alpha = 1;
		} else if ((x <= endLoc.x && open)) {
			buttonsList[i]->setLoc({ x, y });

		} else if (!open)
			buttonsList[i]->setLoc({ x, y });

		buttonsList[i]->SetColorMod(glm::vec4(glm::vec3(1.f), alpha));
	}
}

void UUIWidget::finishButtons() {
	canOpenClose = true;
	open = !open;

	if (open)
		for (int i = 0; i < buttonsList.size(); i++)
			buttonsList[i]->enable(true);
	else
		for (int i = 0; i < buttonsList.size(); i++)
			buttonsList[i]->enable(false);
}


void UUIWidget::openMerchantWidget() {
	world::currWorld->merchant->click();
}

void UUIWidget::openFishermanWidget() {
	world::currWorld->fisherman->click();
}

void UUIWidget::openMechanicWidget() {
	world::currWorld->mechanic->click();
}

void UUIWidget::openPetSellerWidget() {
	world::currWorld->petSeller->click();
}

void UUIWidget::openSailorWidget() {
	world::currWorld->sailor->click();
}

void UUIWidget::openAtmWidget() {
	world::currWorld->atm->click();
}