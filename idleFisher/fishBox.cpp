#include "fishBox.h"
#include "stuff.h"
#include "shortNumbers.h"
#include "saveData.h"

#include "journal.h"
#include "text.h"
#include "button.h"

#include "debugger.h"

UfishBox::UfishBox(Ujournal* parent, FfishData* fishData, FsaveFishData* saveFishData) : widget(parent) {
	this->ref = parent;

	this->fishData = fishData;
	this->saveFishData = saveFishData;
	
	std::string fishThumbnail = fishData->thumbnail;
	fishThumbnail.erase(0, 7); // removes 'images/'
	fishButton = std::make_unique<Ubutton>(this, fishThumbnail, 16, 16, 1, vector{ 0, 0 }, false, false);
	fishButton->SetPivot({ 0.5f, 0.f });
	fishButton->addCallback(this, &UfishBox::openFishPage);
	name = std::make_unique<text>(this, fishData->name, "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);
	checkMark = std::make_unique<Image>("images/widget/check.png", vector{ 0, 0 }, false);

	if (name && fishButton && !saveFishData->unlocked) {
		name->setText("???");
		fishButton->SetColorMod(glm::vec4(0, 0, 0, 1));
		fishButton->enable(false);
	}
}

UfishBox::~UfishBox() {

}

void UfishBox::draw(Shader* shaderProgram) {
	fishButton->draw(shaderProgram);
	name->draw(shaderProgram);

	if (checkMark && isUnlocked() && hasAllStars() && hasBiggestSize())
		checkMark->draw(shaderProgram);
}

void UfishBox::openFishPage() {
	ref->openFishPage(fishData, saveFishData);
}

void UfishBox::setLoc(vector loc) {
	__super::setLoc(loc);

	if (name)
		name->setLoc(loc);

	if (fishButton)
		fishButton->setLoc(name->getAbsoluteLoc() + vector{ 0, 9.f });

	if (checkMark)
		checkMark->setLoc(fishButton->getAbsoluteLoc() + vector{ fishButton->getSize().x / 2.f + 3.f, 0.f });
}

void UfishBox::updateUI() {
	if (!saveFishData->unlocked)
		return;

	//img->setImgColorMod(255, 255, 255);
	fishButton->SetColorMod(glm::vec4(1));
	fishButton->enable(true);
	name->setText(fishData->name);
}

bool UfishBox::hasAllStars() {
	return saveFishData->totalNumOwned[1] != 0 && saveFishData->totalNumOwned[2] != 0 && saveFishData->totalNumOwned[3] != 0;
}

bool UfishBox::hasBiggestSize() {
	return saveFishData->biggestSizeCaught == fishData->maxSize;
}

bool UfishBox::isUnlocked() {
	return saveFishData->unlocked;
}