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
	background = std::make_unique<Image>("images/widget/journal.png", vector(0.f, 0.f), false);
	background->SetAnchor(ANCHOR_CENTER, ANCHOR_CENTER);
	background->SetPivot({ 0.5f, 0.5f });

	journalClosed = std::make_unique<Image>("images/widget/journalClosed.png", vector{ 0, 0 }, false);
	journalClosed->SetAnchor(ANCHOR_CENTER, ANCHOR_CENTER);
	journalClosed->SetPivot({ 0.5f, 0.5f });

	std::unordered_map < std::string, animDataStruct> anim;
	anim.insert({ "open", animDataStruct({0, 0}, {5, 0}, false) });
	anim.insert({ "close", animDataStruct({0, 1}, {5, 1}, false) });
	journalAnim = std::make_unique<animation>("widget/journalOpen.png", 282, 244, anim, false, vector{ 0, 0 });
	journalAnim->addFinishedCallback(this, &Ujournal::journalAnimFinish);
	journalAnim->setAnimation("open");

	journalTimer = CreateDeferred<Timer>();
	journalTimer->addUpdateCallback(this, &Ujournal::moveAnim);
	journalTimer->addCallback(this, &Ujournal::journalTimerFinish);

	forwardButton = std::make_unique<Ubutton>(this, "widget/fowardButton.png", 19, 12, 1, vector{ 102, -71 }, false, false);
	forwardButton->SetAnchor(ANCHOR_CENTER, ANCHOR_CENTER);
	forwardButton->addCallback(this, &Ujournal::forwardPage);
	backButton = std::make_unique<Ubutton>(this, "widget/backButton.png", 19, 12, 1, vector{ -121, -71 }, false, false);
	backButton->SetAnchor(ANCHOR_CENTER, ANCHOR_CENTER);
	backButton->addCallback(this, &Ujournal::backwardPage);
	xButton = std::make_unique<Ubutton>(this, "widget/xButton.png", 11, 11, 1, vector{ 114, 79 }, false, false);
	xButton->SetAnchor(ANCHOR_CENTER, ANCHOR_CENTER);
	xButton->addCallback(this, &Ujournal::closeWidget);

	if (SaveData::orderedData.worldData.size() >= 2) {
		ProgressionNode& worldData = SaveData::data.progressionData.at(4u);
		worldName1 = std::make_unique<text>(this, worldData.name, "biggerStraightDark", vector{0.f, 0.f}, false, false, TEXT_ALIGN_CENTER);
		worldName2 = std::make_unique<text>(this, worldData.name, "biggerStraightDark", vector{0.f, 0.f}, false, false, TEXT_ALIGN_CENTER);

		worldProgress1 = std::make_unique<UprogressBar>(this, vector{ 100.f, 5.f }, false);
		worldProgress1->SetPivot({ 0.5f, 0.f });
		worldProgress1->setForegroundColor({ 227.f / 255.f, 120.f / 255.f, 64.f / 255.f, 1.f }, 0);
		worldProgress1->addProgressBar(0, 3, { 232.f / 255.f, 210.f / 255.f, 75.f / 255.f, 1.f });
		worldProgress1->addProgressBar(0, 1, { 120.f / 255.f, 158.f / 255.f, 36.f / 255.f, 1.f });
		worldProgress2 = std::make_unique<UprogressBar>(this, vector{ 100.f, 5.f }, false);
		worldProgress2->SetPivot({ 0.5f, 0.f });
		worldProgress2->setForegroundColor({ 227.f / 255.f, 120.f / 255.f, 64.f / 255.f, 1.f }, 0);
		worldProgress2->addProgressBar(0, 3, { 232.f / 255.f, 210.f / 255.f, 75.f / 255.f, 1.f });
		worldProgress2->addProgressBar(0, 1, { 120.f / 255.f, 158.f / 255.f, 36.f / 255.f, 1.f });

		journalProgressWidget = std::make_unique<UjournalProgressWidget>(this, 5, 15, 5);
	}

	for (auto& [fishId, fishData] : SaveData::data.fishData) {
		FsaveFishData* saveFishData = &SaveData::saveData.fishData.at(fishId);

		std::unique_ptr<UfishBox> fishBox = std::make_unique<UfishBox>(this, &fishData, saveFishData);

		fishBoxList.push_back(std::move(fishBox));
	}

	// selected fish page
	fishThumbnail = std::make_unique<Image>("images/fish/dirtFish.png", vector{ 0, 0 }, false);
	fishThumbnail->SetPivot({ 0.5f, 0.5f });
	notesBackground = std::make_unique<Image>("images/widget/journalNotes.png", vector{ 0, 0 }, false);
	notesBackground->SetPivot({ 0.5f, 0.f });
	selectedFishName = std::make_unique<text>(this, " ", "biggerStraightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);
	selectedFishDescription = std::make_unique<text>(this, " ", "straightDark", vector{ 0, 0 });

	statsTitle = std::make_unique<text>(this, "Stats", "biggerStraightDark", vector{ 0 , 0 }, false, false, TEXT_ALIGN_CENTER);
	baseCurrency = std::make_unique<text>(this, "Base Currency", "straightDark", vector{ 0, 0 });
	baseCurrency->SetPivot({ 0, 1.f });
	baseCurrencyNum = std::make_unique<text>(this, " ", "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);
	baseCurrencyNum->SetPivot({ 0, 1.f });
	currency = std::make_unique<text>(this, "Currency", "straightDark", vector{ 0, 0 });
	currency->SetPivot({ 0, 1.f });
	currencyNum = std::make_unique<text>(this, " ", "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);
	currencyNum->SetPivot({ 0, 1.f });
	caught = std::make_unique<text>(this, "Caught", "straightDark", vector{ 0, 0 });
	caught->SetPivot({ 0, 1.f });
	caughtNum = std::make_unique<text>(this, " ", "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);
	caughtNum->SetPivot({ 0, 1.f });
	power = std::make_unique<text>(this, "Power", "straightDark", vector{ 0, 0 });
	power->SetPivot({ 0, 1.f });
	powerNum = std::make_unique<text>(this, " ", "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);
	powerNum->SetPivot({ 0, 1.f });
	speed = std::make_unique<text>(this, "Speed", "straightDark", vector{ 0, 0 });
	speed->SetPivot({ 0, 1.f });
	speedNum = std::make_unique<text>(this, " ", "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);
	speedNum->SetPivot({ 0, 1.f });
	yellow = std::make_unique<text>(this, "Yellow", "straightDark", vector{ 0, 0 });
	yellow->SetPivot({ 0, 1.f });
	yellowNum = std::make_unique<text>(this, " ", "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);
	yellowNum->SetPivot({ 0, 1.f });
	green = std::make_unique<text>(this, "Green", "straightDark", vector{ 0, 0 });
	green->SetPivot({ 0, 1.f });
	greenNum = std::make_unique<text>(this, " ", "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);
	greenNum->SetPivot({ 0, 1.f });
	probability = std::make_unique<text>(this, "Probability", "straightDark", vector{ 0, 0 });
	probability->SetPivot({ 0, 1.f });
	probabilityNum = std::make_unique<text>(this, " ", "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);
	probabilityNum->SetPivot({ 0, 1.f });
	fishSize = std::make_unique<text>(this, "Biggest (Max: 0)", "straightDark", vector{ 0, 0 });
	fishSize->SetPivot({ 0, 1.f });
	fishSizeNum = std::make_unique<text>(this, " ", "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);
	fishSizeNum->SetPivot({ 0, 1.f });

	star1 = std::make_unique<Image>("images/emptyStar.png", vector{ 0, 0 }, false);
	star1->SetPivot({ 0.5f, 0.f });
	star1Text = std::make_unique<text>(this, "0", "straight", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);
	star1Text->SetPivot({ 0.f, 0.5f });
	star1Text->setTextColor(85, 177, 241);
	star2 = std::make_unique<Image>("images/emptyStar.png", vector{ 0, 0 }, false);
	star2->SetPivot({ 0.5f, 0.f });
	star2Text = std::make_unique<text>(this, "0", "straight", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);
	star2Text->SetPivot({ 0.f, 0.5f });
	star2Text->setTextColor(85, 177, 241);
	star3 = std::make_unique<Image>("images/emptyStar.png", vector{ 0, 0 }, false);
	star3->SetPivot({ 0.5f, 0.f });
	star3Text = std::make_unique<text>(this, "0", "straight", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);
	star3Text->SetPivot({ 0.f, 0.5f });
	star3Text->setTextColor(85, 177, 241);

	setupLocs();
	updatePages();
}

Ujournal::~Ujournal() {

}

void Ujournal::draw(Shader* shaderProgram) {
	uint32_t worldId = Scene::GetWorldId(pageNum);

	if (journalClosed && !journalTimer->IsFinished())
		journalClosed->draw(shaderProgram);
	if (journalAnim && !journalAnim->IsFinished() && journalTimer->IsFinished())
		journalAnim->draw(shaderProgram);

	if (!journalAnim || !journalAnim->IsFinished() || !journalTimer->IsFinished())
		return;

	background->draw(shaderProgram);
	
	for (int i = 0; i < fishBoxList.size(); i++) {
		if (pageNum == -1)
			break;

		if (fishBoxList[i]->fishData->worldId == worldId || (SaveData::orderedData.worldData.size() > pageNum && fishBoxList[i]->fishData->worldId == worldId))
			fishBoxList[i]->draw(shaderProgram);
	}

	if (backButton && pageNum != 0)
		backButton->draw(shaderProgram);
	if (xButton)
		xButton->draw(shaderProgram);

	if (pageNum != -1) {
		if (forwardButton && pageNum != SaveData::orderedData.worldData.size() - 1)
			forwardButton->draw(shaderProgram);
		worldName1->draw(shaderProgram);
		worldName2->draw(shaderProgram);

		if (SaveData::saveData.progressionData.at(worldId).level) {
			worldProgress1->draw(shaderProgram);
			worldProgress2->draw(shaderProgram);

			if (worldProgress1->mouseOver()) {
				journalProgressWidget->draw(shaderProgram);
				journalProgressWidget->updateNumbers(unlockedFish1, fishStars1, maxFishSize1);
			} else if (worldProgress2->mouseOver()) {
				journalProgressWidget->draw(shaderProgram);
				journalProgressWidget->updateNumbers(unlockedFish2, fishStars2, maxFishSize2);
			}
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
	int size = int(SaveData::orderedData.worldData.size()) - 1;
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
	if (SaveData::orderedData.worldData.size() == 0)
		return;

	if (pageNum != -1) {
		ProgressionNode& worldData = SaveData::data.progressionData.at(Scene::GetWorldId(pageNum));
		SaveEntry& saveWorldData = SaveData::saveData.progressionData.at(worldData.id);
		std::string worldName = worldData.name;

		if (saveWorldData.level)
			worldName1->setText(worldName);
		else
			worldName1->setText("???");

		calcWorldPercentage(worldProgress1.get(), worldProgress2.get(), worldData.id);

		bool world2Unlocked = saveWorldData.level;
		if (world2Unlocked && SaveData::orderedData.worldData.size() > pageNum) // checks if in range, incase add world and its only on left page
			worldName2->setText(worldName);
		else if (!world2Unlocked)
			worldName2->setText("???");
		else
			worldName2->setText("");

		vector center = stuff::screenSize / (stuff::pixelSize * 2.f);
		vector firstFish = center + vector{ -98.f, 30.f };
		vector increment = vector{ 64.f, -48.f };
		vector secondpageDist = { 130, 0 };
		fishBoxList[pageNum * 10 + 1]->setLoc(firstFish);
		fishBoxList[pageNum * 10 + 2]->setLoc(firstFish + vector{ increment.x, 0.f });
		fishBoxList[pageNum * 10 + 3]->setLoc(firstFish + vector{ 0.f, increment.y });
		fishBoxList[pageNum * 10 + 4]->setLoc(firstFish + vector{ increment.x, increment.y });
		fishBoxList[pageNum * 10 + 5]->setLoc(firstFish + vector{ increment.x / 2.f, increment.y * 2.f });

		fishBoxList[pageNum * 10 + 6]->setLoc(firstFish + secondpageDist);
		fishBoxList[pageNum * 10 + 7]->setLoc(firstFish + vector{ increment.x, 0.f } + secondpageDist);
		fishBoxList[pageNum * 10 + 8]->setLoc(firstFish + vector{ 0.f, increment.y } + secondpageDist);
		fishBoxList[pageNum * 10 + 9]->setLoc(firstFish + vector{ increment.x, increment.y } + secondpageDist);
		fishBoxList[pageNum * 10 + 10]->setLoc(firstFish + vector{ increment.x / 2.f, increment.y * 2.f } + secondpageDist);
	} else {
		vector center = stuff::screenSize / (stuff::pixelSize * 2.f);

		selectedFishName->setLoc(center + vector{ -63.f, 80.f });
		selectedFishDescription->setLoc(center + vector{ -113.f, -8.f });
		selectedFishDescription->setLineLength(102);
		fishThumbnail->setSize(fishThumbnail->getSize() * 3.f);
		fishThumbnail->setLoc(center + vector{ -63.f, 32.f });
		notesBackground->setLoc(center + vector{ -63.f, -60.f });

		vector horizOffset = vector{ 100.f, 9.f };
		vector vertOffset = vector{ 0.f, -8.f };
		statsTitle->setLoc(center + vector{ 68.f, 80.f });
		baseCurrency->setLoc(statsTitle->getAbsoluteLoc() + vector{ -50.f, -6.f });
		baseCurrencyNum->setLoc(baseCurrency->getAbsoluteLoc() + horizOffset);
		currency->setLoc(baseCurrency->getAbsoluteLoc() + vector{ 0.f, 1.f });
		currencyNum->setLoc(baseCurrency->getAbsoluteLoc() + horizOffset + vertOffset);
		caught->setLoc(baseCurrency->getAbsoluteLoc() + vertOffset);
		caughtNum->setLoc(baseCurrency->getAbsoluteLoc() + horizOffset + vertOffset * 2.f);
		power->setLoc(baseCurrency->getAbsoluteLoc() + vertOffset * 2.f);
		powerNum->setLoc(baseCurrency->getAbsoluteLoc() + horizOffset + vertOffset * 3.f);
		speed->setLoc(baseCurrency->getAbsoluteLoc() + vertOffset * 3.f);
		speedNum->setLoc(baseCurrency->getAbsoluteLoc() + horizOffset + vertOffset * 4.f);
		yellow->setLoc(baseCurrency->getAbsoluteLoc() + vertOffset * 4.f);
		yellowNum->setLoc(baseCurrency->getAbsoluteLoc() + horizOffset + vertOffset * 5.f);
		green->setLoc(baseCurrency->getAbsoluteLoc() + vertOffset * 5.f);
		greenNum->setLoc(baseCurrency->getAbsoluteLoc() + horizOffset + vertOffset * 6.f);
		probability->setLoc(baseCurrency->getAbsoluteLoc() + vertOffset * 6.f);
		probabilityNum->setLoc(baseCurrency->getAbsoluteLoc() + horizOffset + vertOffset * 7.f);
		fishSize->setLoc(baseCurrency->getAbsoluteLoc() + vertOffset * 7.f);
		fishSizeNum->setLoc(baseCurrency->getAbsoluteLoc() + horizOffset + vertOffset * 8.f);

		star1->setLoc(center + vector{ -78.f, 65.f });
		star2->setLoc(center + vector{ -63.f, 65.f });
		star3->setLoc(center + vector{ -48.f, 65.f });

		star1Text->setLoc(star1->getAbsoluteLoc() + star1->getSize() / 2.f);
		star2Text->setLoc(star2->getAbsoluteLoc() + star2->getSize() / 2.f);
		star3Text->setLoc(star3->getAbsoluteLoc() + star3->getSize() / 2.f);
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

	vector center = stuff::screenSize / (stuff::pixelSize * 2.f);
	map = std::make_unique<Image>("images/widget/maps/" + std::to_string(fishData->worldId) + ".png", vector{ 0, 0 }, false);
	map->SetPivot({ 0.5f, 0.5f });
	map->setLoc(center + vector{ 63.f, -35.f });

	// update stars
	if (saveFishData->totalNumOwned[1] != 0)
		star1->setImage("images/widget/bronzeStar.png");
	else
		star1->setImage("images/emptyStar.png");

	if (saveFishData->totalNumOwned[2] != 0)
		star2->setImage("images/widget/silverStar.png");
	else
		star2->setImage("images/emptyStar.png");

	if (saveFishData->totalNumOwned[3] != 0)
		star3->setImage("images/widget/goldStar.png");
	else
		star3->setImage("images/emptyStar.png");

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

	std::vector<float> percents = { -0.926f, -0.463f, -0.093f, -0.028f, -0.009f, 0.f, 0.f };
	if (opening) // open anim
		journalClosed->setLoc(vector{ 0.f, percents[floor(percent * 6)] * stuff::screenSize.y / stuff::pixelSize });
	else // close anim
		journalClosed->setLoc(vector{ 0.f, percents[floor((1 - percent) * 6)] * stuff::screenSize.y / stuff::pixelSize });
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

	if (journalAnim->GetCurrAnim() == "open")
		open = true;
	else if (journalAnim->GetCurrAnim() == "close")
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

	journalAnim->setLoc(background->getAbsoluteLoc() - vector(0.f, 3.f));

	vector center = stuff::screenSize / (stuff::pixelSize * 2.f);
	worldName1->setLoc(center + vector{ -63.f, 80.f });
	worldName2->setLoc(center + vector{ 68.f, 80.f });

	worldProgress1->setLoc(center + vector{ -63.f, 71.f });
	worldProgress2->setLoc(center + vector{ 68.f, 71.f });
}

void Ujournal::calcWorldPercentage(UprogressBar* normalProgressBar, UprogressBar* rareProgressBar, uint32_t worldId) {
	unlockedFish1 = 0, fishStars1 = 0, maxFishSize1 = 0;
	unlockedFish2 = 0, fishStars2 = 0, maxFishSize2 = 0;
	unlockedTotal1 = 0, starsTotal1 = 0, SizeTotal1 = 0;
	unlockedTotal2 = 0, starsTotal2 = 0, SizeTotal2 = 0;

	for (auto& [fishId, fishData] : SaveData::data.fishData) {
		FsaveFishData saveFishData = SaveData::saveData.fishData[fishData.id];
		if (fishData.worldId == worldId) {
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