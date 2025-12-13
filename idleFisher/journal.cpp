#include "journal.h"
#include "main.h"
#include "saveData.h"
#include "shortNumbers.h"
#include "upgrades.h"

#include "fishBox.h"
#include "button.h"
#include "text.h"
#include "timer.h"
#include "animation.h"
#include "progressBar.h"
#include "journalProgressWidget.h"

#include "debugger.h"

Ujournal::Ujournal(widget* parent) : widget(parent) {
	background = std::make_unique<Image>("./images/widget/journal.png", vector{ 0, 0 }, false);
	journalClosed = std::make_unique<Image>("./images/widget/journalClosed.png", vector{ 0, 0 }, false);

	std::unordered_map < std::string, animDataStruct> anim;
	anim.insert({ "open", animDataStruct({0, 0}, {5, 0}, false) });
	anim.insert({ "close", animDataStruct({0, 1}, {5, 1}, false) });
	journalAnim = std::make_unique<animation>("widget/journalOpen.png", 282, 240, anim, false, vector{ 0, 0 });
	journalAnim->addFinishedCallback(this, &Ujournal::journalAnimFinish);
	journalAnim->setAnimation("open");

	journalTimer = std::make_unique<timer>();
	journalTimer->addUpdateCallback(this, &Ujournal::moveAnim);
	journalTimer->addCallback(this, &Ujournal::journalTimerFinish);

	forwardButton = std::make_unique<Ubutton>(this, "widget/fowardButton.png", 19, 12, 1, vector{ 0, 0 }, false, false);
	backButton = std::make_unique<Ubutton>(this, "widget/backButton.png", 19, 12, 1, vector{ 0, 0 }, false, false);
	xButton = std::make_unique<Ubutton>(this, "widget/xButton.png", 11, 11, 1, vector{ 0, 0 }, false, false);
	forwardButton->addCallback(this, &Ujournal::forwardPage);
	backButton->addCallback(this, &Ujournal::backwardPage);
	xButton->addCallback(this, &Ujournal::closeWidget);

	if (SaveData::data.worldData.size() >= 2) {
		worldName1 = std::make_unique<text>(this, SaveData::data.worldData[0].name, "biggerStraightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);
		worldName2 = std::make_unique<text>(this, SaveData::data.worldData[0].name, "biggerStraightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);

		worldProgress1 = std::make_unique<UprogressBar>(this, false, 100, 3);
		worldProgress1->setForegroundColor({ 227, 120, 64, 255 }, 0);
		worldProgress1->addProgressBar(0, 3, { 232, 210, 75, 255 });
		worldProgress1->addProgressBar(0, 1, { 120, 158, 36, 255 });
		worldProgress2 = std::make_unique<UprogressBar>(this, false, 100, 3);
		worldProgress2->setForegroundColor({ 227, 120, 64, 255 }, 0);
		worldProgress2->addProgressBar(0, 3, { 232, 210, 75, 255 });
		worldProgress2->addProgressBar(0, 1, { 120, 158, 36, 255 });

		journalProgressWidget = std::make_unique<UjournalProgressWidget>(this, 5, 15, 5);
	}

	for (int i = 0; i < SaveData::data.fishData.size(); i++) {
		FfishData* currFish = &SaveData::data.fishData[i];
		FsaveFishData* currSaveFish = &SaveData::saveData.fishData[i];

		std::unique_ptr<UfishBox> fishBox = std::make_unique<UfishBox>(this, currFish, currSaveFish);

		fishBoxList.push_back(std::move(fishBox));
	}

	// selected fish page
	fishThumbnail = std::make_unique<Image>("", vector{ 0, 0 }, false);
	notesBackground = std::make_unique<Image>("./images/widget/journalNotes.png", vector{ 0, 0 }, false);
	selectedFishName = std::make_unique<text>(this, " ", "biggerStraightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);
	selectedFishDescription = std::make_unique<text>(this, " ", "straightDark", vector{ 0, 0 });

	statsTitle = std::make_unique<text>(this, "Stats", "biggerStraightDark", vector{ 0 , 0 }, false, false, TEXT_ALIGN_CENTER);
	baseCurrency = std::make_unique<text>(this, "Base Currency", "straightDark", vector{ 0, 0 });
	baseCurrencyNum = std::make_unique<text>(this, " ", "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);
	currency = std::make_unique<text>(this, "Currency", "straightDark", vector{ 0, 0 });
	currencyNum = std::make_unique<text>(this, " ", "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);
	caught = std::make_unique<text>(this, "Caught", "straightDark", vector{ 0, 0 });
	caughtNum = std::make_unique<text>(this, " ", "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);
	power = std::make_unique<text>(this, "Power", "straightDark", vector{ 0, 0 });
	powerNum = std::make_unique<text>(this, " ", "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);
	speed = std::make_unique<text>(this, "Speed", "straightDark", vector{ 0, 0 });
	speedNum = std::make_unique<text>(this, " ", "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);
	yellow = std::make_unique<text>(this, "Yellow", "straightDark", vector{ 0, 0 });
	yellowNum = std::make_unique<text>(this, " ", "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);
	green = std::make_unique<text>(this, "Green", "straightDark", vector{ 0, 0 });
	greenNum = std::make_unique<text>(this, " ", "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);
	probability = std::make_unique<text>(this, "Probability", "straightDark", vector{ 0, 0 });
	probabilityNum = std::make_unique<text>(this, " ", "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);
	fishSize = std::make_unique<text>(this, "Biggest (Max: 0)", "straightDark", vector{ 0, 0 });
	fishSizeNum = std::make_unique<text>(this, " ", "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);

	star1 = std::make_unique<Image>("./images/emptyStar.png", vector{ 0, 0 }, false);
	star1Text = std::make_unique<text>(this, "0", "straight", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);
	star1Text->setTextColor(85, 177, 241);
	star2 = std::make_unique<Image>("./images/emptyStar.png", vector{ 0, 0 }, false);
	star2Text = std::make_unique<text>(this, "0", "straight", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);
	star2Text->setTextColor(85, 177, 241);
	star3 = std::make_unique<Image>("./images/emptyStar.png", vector{ 0, 0 }, false);
	star3Text = std::make_unique<text>(this, "0", "straight", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);
	star3Text->setTextColor(85, 177, 241);

	setupLocs();
	updatePages();
}

Ujournal::~Ujournal() {

}

void Ujournal::draw(Shader* shaderProgram) {
	if (journalClosed && !journalTimer->finished())
		journalClosed->draw(shaderProgram);
	if (journalAnim && !journalAnim->finished() && journalTimer->finished())
		journalAnim->draw(shaderProgram);

	if (!journalAnim || !journalAnim->finished() || !journalTimer->finished())
		return;

	background->draw(shaderProgram);
	
	for (int i = 0; i < fishBoxList.size(); i++) {
		if (pageNum == -1)
			break;

		if (fishBoxList[i]->fishData->levelName == SaveData::data.worldData[pageNum].worldName || (SaveData::data.worldData.size() > pageNum && fishBoxList[i]->fishData->levelName == SaveData::data.worldData[pageNum].worldName))
			fishBoxList[i]->draw(shaderProgram);
	}

	if (backButton && pageNum != 0)
		backButton->draw(shaderProgram);
	if (xButton)
		xButton->draw(shaderProgram);

	if (pageNum != -1) {
		if (forwardButton && pageNum != SaveData::data.worldData.size() - 1)
			forwardButton->draw(shaderProgram);
		worldName1->draw(shaderProgram);
		worldName2->draw(shaderProgram);

		if (SaveData::saveData.worldList[pageNum].unlocked) {
			worldProgress1->draw(shaderProgram);
			worldProgress2->draw(shaderProgram);
		}

		if (worldProgress1->mouseOver()) {
			journalProgressWidget->draw(shaderProgram);
			journalProgressWidget->updateNumbers(unlockedFish1, fishStars1, maxFishSize1);
		} else if (worldProgress2->mouseOver()) {
			journalProgressWidget->draw(shaderProgram);
			journalProgressWidget->updateNumbers(unlockedFish2, fishStars2, maxFishSize2);
		}

	} else {
		fishThumbnail->draw(shaderProgram);
		notesBackground->draw(shaderProgram);
		selectedFishName->draw(shaderProgram);
		selectedFishDescription->draw(shaderProgram);
		if (map)
			map->draw(shaderProgram);

		statsTitle->draw(shaderProgram);
		baseCurrency->draw(shaderProgram);
		baseCurrencyNum->draw(shaderProgram);
		currency->draw(shaderProgram);
		currencyNum->draw(shaderProgram);
		caught->draw(shaderProgram);
		caughtNum->draw(shaderProgram);
		power->draw(shaderProgram);
		powerNum->draw(shaderProgram);
		speed->draw(shaderProgram);
		speedNum->draw(shaderProgram);
		yellow->draw(shaderProgram);
		yellowNum->draw(shaderProgram);
		green->draw(shaderProgram);
		greenNum->draw(shaderProgram);
		probability->draw(shaderProgram);
		probabilityNum->draw(shaderProgram);
		fishSize->draw(shaderProgram);
		fishSizeNum->draw(shaderProgram);

		star1->draw(shaderProgram);
		star2->draw(shaderProgram);
		star3->draw(shaderProgram);

		if (star1->isMouseOver() && selectedSaveFish->totalNumOwned[1])
			star1Text->draw(shaderProgram);
		if (star2->isMouseOver() && selectedSaveFish->totalNumOwned[2])
			star2Text->draw(shaderProgram);
		if (star3->isMouseOver() && selectedSaveFish->totalNumOwned[3])
			star3Text->draw(shaderProgram);
	}
}

void Ujournal::forwardPage() {
	pageNum++;
	int size = int(SaveData::data.worldData.size()) - 1;
	if (pageNum > size)
		pageNum = size;

	updatePages();
}

void Ujournal::backwardPage() {
	if (pageNum != -1) {
		pageNum--;
		if (pageNum < 0)
			pageNum = 0;
	} else {
		pageNum = tempPageNum;
	}

	selectedFish = nullptr;
	selectedSaveFish = nullptr;

	updatePages();
}

void Ujournal::updatePages() {
	if (SaveData::saveData.worldList.size() == 0)
		return;

	if (pageNum != -1) {
		std::string worldName = SaveData::data.worldData[pageNum].name;

		if (SaveData::saveData.worldList[pageNum].unlocked)
			worldName1->setText(worldName);
		else
			worldName1->setText("???");

		calcWorldPercentage(worldProgress1.get(), worldProgress2.get(), pageNum);

		bool world2Unlocked = SaveData::saveData.worldList[pageNum].unlocked;
		if (world2Unlocked && SaveData::data.worldData.size() > pageNum) // checks if in range, incase add world and its only on left page
			worldName2->setText(worldName);
		else if (!world2Unlocked)
			worldName2->setText("???");
		else
			worldName2->setText("");

		vector center = stuff::screenSize / 2;
		vector firstFish = center + vector{ -110, -67 } *stuff::pixelSize;
		vector increment = vector{ 48, 48 } * stuff::pixelSize;
		vector secondpageDist = { 130 * stuff::pixelSize, 0 };
		fishBoxList[pageNum * 10 + 1]->setLoc(firstFish);
		fishBoxList[pageNum * 10 + 2]->setLoc(firstFish + vector{ increment.x, 0 });
		fishBoxList[pageNum * 10 + 3]->setLoc(firstFish + vector{ 0, increment.y });
		fishBoxList[pageNum * 10 + 4]->setLoc(firstFish + vector{ increment.x, increment.y });
		fishBoxList[pageNum * 10 + 5]->setLoc(firstFish + vector{ increment.x / 2, increment.y * 2 });

		fishBoxList[pageNum * 10 + 6]->setLoc(firstFish + secondpageDist);
		fishBoxList[pageNum * 10 + 7]->setLoc(firstFish + vector{ increment.x, 0 } + secondpageDist);
		fishBoxList[pageNum * 10 + 8]->setLoc(firstFish + vector{ 0, increment.y } + secondpageDist);
		fishBoxList[pageNum * 10 + 9]->setLoc(firstFish + vector{ increment.x, increment.y } + secondpageDist);
		fishBoxList[pageNum * 10 + 10]->setLoc(firstFish + vector{ increment.x / 2, increment.y * 2 } + secondpageDist);
	} else {
		selectedFishName->setLoc(vector{ 256, 98 } * stuff::pixelSize); // 174 57
		selectedFishDescription->setLoc(vector{ 126+ 80, 150+40 } * stuff::pixelSize);
		fishThumbnail->w *= 3;
		fishThumbnail->h *= 3;
		fishThumbnail->setLoc((vector{ 150 + 80 + 25, 83 + 41 + 27 } *stuff::pixelSize) - (vector{ float(fishThumbnail->w), float(fishThumbnail->h) } / 2.f * stuff::pixelSize));
		notesBackground->setLoc(vector{ 123 + 80, 140 + 41 } * stuff::pixelSize);
		selectedFishDescription->setLineLength(102 * stuff::pixelSize);

		statsTitle->setLoc(vector{ 256 + 129, 98 } * stuff::pixelSize);
		baseCurrency->setLoc(vector{ 254 + 80, 60 + 50 } *stuff::pixelSize);
		baseCurrencyNum->setLoc(vector{ 352 + 80, 60 + 50 } *stuff::pixelSize);
		currency->setLoc(vector{254 + 80, 68 + 50 } * stuff::pixelSize);
		currencyNum->setLoc(vector{ 352 + 80, 68 + 50 } * stuff::pixelSize);
		caught->setLoc(vector{ 254 + 80, 76 + 50 } * stuff::pixelSize);
		caughtNum->setLoc(vector{ 352 + 80, 76 + 50 } * stuff::pixelSize);
		power->setLoc(vector{ 254 + 80, 84 + 50 } * stuff::pixelSize);
		powerNum->setLoc(vector{ 352 + 80, 84 + 50 } * stuff::pixelSize);
		speed->setLoc(vector{ 254 + 80, 92 + 50 } * stuff::pixelSize);
		speedNum->setLoc(vector{ 352 + 80, 92 + 50 } * stuff::pixelSize);
		yellow->setLoc(vector{ 254 + 80, 100 + 50 } * stuff::pixelSize);
		yellowNum->setLoc(vector{ 352 + 80, 100 + 50 } * stuff::pixelSize);
		green->setLoc(vector{ 254 + 80, 108 + 50 } * stuff::pixelSize);
		greenNum->setLoc(vector{ 352 + 80, 108 + 50 } * stuff::pixelSize);
		probability->setLoc(vector{ 254 + 80, 116 + 50 } * stuff::pixelSize);
		probabilityNum->setLoc(vector{ 352 + 80, 116 + 50 } * stuff::pixelSize);
		fishSize->setLoc(vector{ 254 + 80, 124 + 50 } * stuff::pixelSize);
		fishSizeNum->setLoc(vector{ 352 + 80, 124 + 50 } * stuff::pixelSize);

		star1->setLoc(vector{ 174 + 58, 65 + 41 } * stuff::pixelSize);
		star2->setLoc(vector{ 174 + 74, 65 + 41 } * stuff::pixelSize);
		star3->setLoc(vector{ 174 + 90, 65 + 41 } * stuff::pixelSize);

		star1Text->setLoc(star1->getLoc() + star1->getSize() / 2.f - vector{ 0, star1Text->getSize().y / 2.f });
		star2Text->setLoc(star2->getLoc() + star2->getSize() / 2.f - vector{ 0, star2Text->getSize().y / 2.f });
		star3Text->setLoc(star3->getLoc() + star3->getSize() / 2.f - vector{ 0, star3Text->getSize().y / 2.f });
	}
}

void Ujournal::openFishPage(FfishData* fishData, FsaveFishData* saveFishData) {
	if (pageNum != -1)
		tempPageNum = pageNum;
	pageNum = -1;

	selectedFish = fishData;
	selectedSaveFish = saveFishData;

	fishThumbnail->setImage(fishData->thumbnail);
	selectedFishName->setText(fishData->name);
	selectedFishDescription->setText(fishData->description);

	// find world name
	std::string worldName = "";
	for (FworldStruct world : SaveData::data.worldData) {
		if (world.worldName == fishData->levelName) {
			worldName = world.name;
		}
	}

	baseCurrencyNum->setText(shortNumbers::convert2Short(fishData->currencyNum));
	currencyNum->setText(shortNumbers::convert2Short(upgrades::getFishSellPrice(*fishData, 0)));
	caughtNum->setText(shortNumbers::convert2Short(saveFishData->calcTotalCaughtFish()));
	powerNum->setText(shortNumbers::convert2Short(fishData->fishingPower));
	speedNum->setText(shortNumbers::convert2Short(fishData->fishSpeed));
	yellowNum->setText(shortNumbers::convert2Short(fishData->yellowDifficulty));
	greenNum->setText(shortNumbers::convert2Short(fishData->greenDifficulty));
	probabilityNum->setText(shortNumbers::convert2Short(fishData->probability));
	fishSizeNum->setText(shortNumbers::convert2Short(saveFishData->biggestSizeCaught) + "in");
	fishSize->setText("Biggest (Max: " + std::to_string(fishData->maxSize) + "in)");

	map = std::make_unique<Image>("./images/widget/maps/" + fishData->levelName + ".png", vector{ 0, 0 }, false);
	map->setLoc(vector{ 252+80, 134+46+6 } * stuff::pixelSize);

	if (saveFishData->totalNumOwned[1] != 0)
		star1->setImage("./images/widget/bronzeStar.png");
	if (saveFishData->totalNumOwned[2] != 0)
		star2->setImage("./images/widget/silverStar.png");
	if (saveFishData->totalNumOwned[3] != 0)
		star3->setImage("./images/widget/goldStar.png");

	star1Text->setText(shortNumbers::convert2Short(saveFishData->totalNumOwned[1]));
	star2Text->setText(shortNumbers::convert2Short(saveFishData->totalNumOwned[2]));
	star3Text->setText(shortNumbers::convert2Short(saveFishData->totalNumOwned[3]));

	updatePages();
}

void Ujournal::addedToViewport() {
	if (opening || closing)
		return;

	if (open) {
		closeWidget();
		return;
	}

	if (selectedFish && selectedSaveFish) {
		openFishPage(selectedFish, selectedSaveFish);
	} else
		updatePages();

	// update all the total num caught
	for (int i = 0; i < fishBoxList.size(); i++) {
		fishBoxList[i]->updateUI();
	}

	opening = true;
	journalClosed->setLoc(stuff::screenSize);
	journalTimer->start(.5);
}

void Ujournal::moveAnim() {
	float percent = journalTimer->getTime() / journalTimer->getMaxTime();

	std::vector<vector> locs = { {0, 1000}, { 0, 500 }, {0, 100}, {0, 30}, {0, 10}, {0, 0}, {0, 0} };
	if (opening) // open anim
		journalClosed->setLoc(background->getLoc() + locs[floor(percent * 6)] - vector{ 0, 27 } *stuff::pixelSize);
	else // close anim
		journalClosed->setLoc(background->getLoc() + locs[floor((1 - percent) * 6)] - vector{0, 27} *stuff::pixelSize);
}

void Ujournal::journalTimerFinish() {
	if (opening) {
		journalAnim->setAnimation("open");
		journalAnim->start();
		closing = false;
	} else {
		removeFromViewport();
		opening = false;
		closing = false;
	}
}

void Ujournal::closeWidget() {
	journalAnim->setAnimation("close");
	journalAnim->start();
	opening = false;
	closing = true;
	open = false;
}

void Ujournal::journalAnimFinish() {
	opening = false;

	if (journalAnim->currAnim == "open")
		open = true;
	else if (journalAnim->currAnim == "close")
		journalTimer->start(.5);
}

void Ujournal::removeFromViewport() {
	closing = false;
	opening = false;
	open = false;
	journalTimer->stop();
	journalAnim->stop();

	__super::removeFromViewport();
}

void Ujournal::setupLocs() {
	__super::setupLocs();
	
	background->setLoc((stuff::screenSize - background->getSize()) / 2);
	journalAnim->setLoc(background->getLoc() - vector{0, 28 * stuff::pixelSize});
	journalClosed->setLoc(background->getLoc() + stuff::screenSize);

	forwardButton->setLoc(stuff::screenSize / 2 + vector{ 107, 71 } *stuff::pixelSize);
	backButton->setLoc(stuff::screenSize / 2 + vector{ -126, 71 } *stuff::pixelSize);
	xButton->setLoc(stuff::screenSize / 2 + vector{ 114, -84 } * stuff::pixelSize);

	worldName1->setLoc(stuff::screenSize / 2 + vector{ -63, -80 } * stuff::pixelSize);
	worldName2->setLoc(stuff::screenSize / 2 + vector{ 68, -80 } * stuff::pixelSize);

	worldProgress1->setLoc(stuff::screenSize / 2 + vector{ -63-50, -80+11 } * stuff::pixelSize);
	worldProgress2->setLoc(stuff::screenSize / 2 + vector{ 68-50, -80+11 } * stuff::pixelSize);
}

void Ujournal::calcWorldPercentage(UprogressBar* normalProgressBar, UprogressBar* rareProgressBar, int worldId) {
	unlockedFish1 = 0, fishStars1 = 0, maxFishSize1 = 0;
	unlockedFish2 = 0, fishStars2 = 0, maxFishSize2 = 0;
	unlockedTotal1 = 0, starsTotal1 = 0, SizeTotal1 = 0;
	unlockedTotal2 = 0, starsTotal2 = 0, SizeTotal2 = 0;

	for (FfishData fishData : SaveData::data.fishData) {
		FsaveFishData saveFishData = SaveData::saveData.fishData[fishData.id];
		if (fishData.levelName == SaveData::data.worldData[worldId].worldName) {
			if (!fishData.isRareFish) { //  && saveFishData.unlocked
				unlockedTotal1++;
				starsTotal1 += 3;
				SizeTotal1++;

				if (saveFishData.unlocked) {
					unlockedFish1++;
					fishStars1 += saveFishData.totalNumOwned[1] != 0;
					fishStars1 += saveFishData.totalNumOwned[2] != 0;
					fishStars1 += saveFishData.totalNumOwned[3] != 0;
					maxFishSize1 += saveFishData.biggestSizeCaught == fishData.maxSize;
				}
			} else {
				unlockedTotal2++;
				starsTotal2 += 3;
				SizeTotal2++;

				if (saveFishData.unlocked) {
					unlockedFish2++;
					fishStars2 += saveFishData.totalNumOwned[1] != 0;
					fishStars2 += saveFishData.totalNumOwned[2] != 0;
					fishStars2 += saveFishData.totalNumOwned[3] != 0;
					maxFishSize2 += saveFishData.biggestSizeCaught == fishData.maxSize;
				}
			}
		}
	}

	if (unlockedTotal1)
		normalProgressBar->setPercent(unlockedFish1 / unlockedTotal1, 0);
	else
		normalProgressBar->setPercent(0, 0);

	if (starsTotal1)
		normalProgressBar->setPercent(fishStars1 / starsTotal1, 1);
	else
		normalProgressBar->setPercent(0, 1);

	if (SizeTotal1)
		normalProgressBar->setPercent(maxFishSize1 / SizeTotal1, 2);
	else
		normalProgressBar->setPercent(0, 2);

	

	if (unlockedTotal2)
		rareProgressBar->setPercent(unlockedFish2 / unlockedTotal2, 0);
	else
		rareProgressBar->setPercent(0, 0);

	if (starsTotal2)
		rareProgressBar->setPercent(fishStars2 / starsTotal2, 1);
	else
		rareProgressBar->setPercent(0, 1);

	if (SizeTotal2)
		rareProgressBar->setPercent(maxFishSize2 / SizeTotal2, 2);
	else
		rareProgressBar->setPercent(0, 2);

}