#include "settings.h"
#include "main.h"
#include "math.h"
#include "text.h"
#include "scrollBox.h"
#include "button.h"
#include "pauseMenu.h"
#include "slider.h"
#include "horizontalBox.h"
#include "saveData.h"
#include "settingsBlock.h"
#include "confirmWidget.h"

#include "Cursor.h"

#include "debugger.h"

Usettings::Usettings(widget* parent) : widget(parent) {
	// button click anim
	std::vector<std::string> buttonClickAnim;
	for (int i = 0; i < 2; i++)
		buttonClickAnim.push_back("./images/widget/upgradeButton" + std::to_string(i + 1) + ".png");

	background = std::make_unique<Image>("./images/widget/achievementBackground.png", vector{ 0, 0 }, false);

	backButton = std::make_unique<Ubutton>(this, "widget/npcXButton.png", 11, 11, 1, vector{ 0, 0 }, false, false);
	backButton->addCallback(this, &Usettings::goBack);

	scrollBox = std::make_unique<UscrollBox>(this);

	settingsTitle = std::make_unique<text>(this, "Settings", "biggerStraight", vector{ 0, 0 });
	audioTitle = std::make_unique<text>(this, "  Audio:", "biggerStraight", vector{ 0, 0 });
	graphicsTitle = std::make_unique<text>(this, "  Graphics:", "biggerStraight", vector{ 0, 0 });

	// padding
	scrollBox->addChild(nullptr, 6 * stuff::pixelSize);

// settings
	scrollBox->addChild(settingsTitle.get(), settingsTitle->getSize().y + 3 * stuff::pixelSize);

	saveButton = std::make_unique<Ubutton>(this, "widget/upgradeButton.png", 37, 16, 1, vector{ 0, 0 }, false, false);
	saveButton->addCallback(this, &Usettings::SaveSettings);
	saveText = std::make_unique<text>(this, "Save", "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);
	
	cancelButton = std::make_unique<Ubutton>(this, "widget/upgradeButton.png", 37, 16, 1, vector{ 0, 0 }, false, false);
	cancelButton->addCallback(this, &Usettings::cancel);
	
	cancelText = std::make_unique<text>(this, "Revert", "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);
	
// audio
	scrollBox->addChild(audioTitle.get(), audioTitle->getSize().y + 3 * stuff::pixelSize);


	float length = background->getSize().x - 50 * stuff::pixelSize;
	float sliderHeight = 3 * stuff::pixelSize;
	float titleSliderLength = 38 * stuff::pixelSize;
	glm::vec4 sliderForegroundColor = glm::vec4(0.94901960784, 0.91372549019, 0.82745098039, 1);
	glm::vec4 sliderBackgroundColor = glm::vec4(181.f / 255.f, 145.f / 255.f, 101.f/255.f, 1);
	masterVolumeSlider = std::make_unique<Uslider>(this, false, vector{ length, sliderHeight }, 0, 100);
	masterVolumeSlider->setForegroundColor(sliderForegroundColor);
	masterVolumeSlider->setBackgroundColor(sliderBackgroundColor);
	masterVolumeSlider->setSliderTitle("     Master");
	masterVolumeSlider->bindValue(&SaveData::settingsData.masterVolume);
	masterVolumeSlider->setTitleLength(titleSliderLength);

	musicVolume = std::make_unique<Uslider>(this, false, vector{ length, sliderHeight }, 0, 100);
	musicVolume->setForegroundColor(sliderForegroundColor);
	musicVolume->setBackgroundColor(sliderBackgroundColor);
	musicVolume->setSliderTitle("     Music");
	musicVolume->bindValue(&SaveData::settingsData.musicVolume);
	musicVolume->setTitleLength(titleSliderLength);

	sfxVolume = std::make_unique<Uslider>(this, false, vector{ length, sliderHeight }, 0, 100);
	sfxVolume->setForegroundColor(sliderForegroundColor);
	sfxVolume->setBackgroundColor(sliderBackgroundColor);
	sfxVolume->setSliderTitle("     SFX");
	sfxVolume->bindValue(&SaveData::settingsData.sfxVolume);
	sfxVolume->setTitleLength(titleSliderLength);

	dialogVolume = std::make_unique<Uslider>(this, false, vector{ length, sliderHeight }, 0, 100);
	dialogVolume->setForegroundColor(sliderForegroundColor);
	dialogVolume->setBackgroundColor(sliderBackgroundColor);
	dialogVolume->setSliderTitle("     Dialog");
	dialogVolume->bindValue(&SaveData::settingsData.dialogVolume);
	dialogVolume->setTitleLength(titleSliderLength);

	float sliderPadding = 17 * stuff::pixelSize;
	scrollBox->addChild(masterVolumeSlider.get(), masterVolumeSlider->getSize().y + sliderPadding);
	scrollBox->addChild(musicVolume.get(), musicVolume->getSize().y + sliderPadding);
	scrollBox->addChild(sfxVolume.get(), sfxVolume->getSize().y + sliderPadding);
	scrollBox->addChild(dialogVolume.get(), dialogVolume->getSize().y + sliderPadding);
	scrollBox->addChild(nullptr, 5 * stuff::pixelSize);

// graphics
	scrollBox->addChild(graphicsTitle.get(), graphicsTitle->getSize().y + 3 * stuff::pixelSize);

	pixelFontBlock = std::make_unique<UsettingsBlock>(this, "Pixel Font", length, std::vector<std::string>{ "Off", "On" }, &SaveData::settingsData.pixelFont);
	pixelFontBlock->addCallback(text::changeFontAll);
	scrollBox->addChild(pixelFontBlock.get(), pixelFontBlock->getSize().y);

	shortNumBlock = std::make_unique<UsettingsBlock>(this, "Enable Short Numbers", length, std::vector<std::string>{ "Long", "Short" }, &SaveData::settingsData.shortNumbers);
	scrollBox->addChild(shortNumBlock.get(), shortNumBlock->getSize().y);

	petBlock = std::make_unique<UsettingsBlock>(this, "Show Pets", length, std::vector<std::string>{ "Off", "On" }, &SaveData::settingsData.showPets);
	scrollBox->addChild(petBlock.get(), petBlock->getSize().y);

	// show rain
	rainBlock = std::make_unique<UsettingsBlock>(this, "Show Rain", length, std::vector<std::string>{ "Off", "On" }, &SaveData::settingsData.showRain);
	scrollBox->addChild(rainBlock.get(), rainBlock->getSize().y);

	// show cursor
	cursorBlock = std::make_unique<UsettingsBlock>(this, "Cursor", length, std::vector<std::string>{ "Off", "On" }, &SaveData::settingsData.cursor);
	cursorBlock->addCallback(Cursor::toggleCursor);
	scrollBox->addChild(cursorBlock.get(), cursorBlock->getSize().y);

	// add bottom padding
	scrollBox->addChild(nullptr, 20 * stuff::pixelSize);

	confirmWidget = std::make_unique<ConfirmWidget>(this);
	confirmWidget->AddSaveCallback(this, &Usettings::saveConfirm);
	confirmWidget->AddRevertCallback(this, &Usettings::revertConfirm);
	confirmWidget->AddCancelCallback(this, &Usettings::cancelConfirm);

	setupLocs();
}

Usettings::~Usettings() {

}

void Usettings::draw(Shader* shaderProgram) {
	background->draw(shaderProgram);
	backButton->draw(shaderProgram);

	scrollBox->draw(shaderProgram);

	if (saveButton)
		saveButton->draw(shaderProgram);
	saveText->draw(shaderProgram);
	if (cancelButton)
		cancelButton->draw(shaderProgram);
	cancelText->draw(shaderProgram);

	if (showingConfirmationBox)
		confirmWidget->draw(shaderProgram);
}

void Usettings::addedToViewport() {
	prevSettingsData = SaveData::settingsData;
	UpdateData();
}

void Usettings::SaveSettings() {
	prevSettingsData = SaveData::settingsData;
	SaveData::saveSettings();
}

void Usettings::UpdateData() {
	// audio
	masterVolumeSlider->UpdateValue();
	musicVolume->UpdateValue();
	sfxVolume->UpdateValue();
	dialogVolume->UpdateValue();

	// graphics
	pixelFontBlock->UpdateValue();
	shortNumBlock->UpdateValue();
	petBlock->UpdateValue();
	rainBlock->UpdateValue();
	cursorBlock->UpdateValue();
}

void Usettings::removeFromViewport() {
	// just like hitting cancel on the confirmation box, brings you back to the settings
	if (showingConfirmationBox) {
		showingConfirmationBox = false;
		return;
	} else if (checkIfSettingsChanged()) {
		showingConfirmationBox = true;
		return;
	}

	__super::removeFromViewport();

	goBack();
}

void Usettings::goBack() {
	if (checkIfSettingsChanged()) {
		showingConfirmationBox = true;
		return;
	}

	RevertSettings();

	// go back to the pause screen
	Main::pauseMenu->addToViewport(true);
}

void Usettings::setupLocs() {
	if (background)
		background->setLoc(vector{ stuff::screenSize.x / 2.f, stuff::screenSize.y / 2.f } - background->getSize() / 2);

	if (backButton) {
		vector backButtonSize = backButton->getSize();
		backButton->setLoc({ float(background->getLoc().x + (background->w - 4) * stuff::pixelSize - backButtonSize.x / 2), float(background->getLoc().y - backButtonSize.y / 2 + 4 * stuff::pixelSize)});
	}

	vector scrollBoxLoc = background->getLoc() + vector{10, 8} *stuff::pixelSize;
	vector scrollBoxSize = vector{ background->getSize().x, background->getSize().y - 15 * stuff::pixelSize};
	scrollBox->setLocAndSize(scrollBoxLoc, scrollBoxSize);
	scrollBox->setOgLoc(scrollBoxLoc);

	if (cancelButton && background && saveButton)
		cancelButton->setLoc(background->getLoc() + background->getSize() - saveButton->getSize() - vector{10, 10} *stuff::pixelSize);
	if (cancelText && cancelButton)
		cancelText->setLoc(cancelButton->getLoc() + cancelButton->getSize() / 2);
	if (saveButton && cancelButton)
		saveButton->setLoc(cancelButton->getLoc() - vector{saveButton->getSize().x + 2 * stuff::pixelSize, 0 });
	if (saveText && saveButton)
		saveText->setLoc(saveButton->getLoc() + saveButton->getSize() / 2);
}

void Usettings::cancel() {
	// just revert all settings back to original before save
	RevertSettings();
}

void Usettings::RevertSettings() {
	// updates settings variable while keeping memory the same for the setting blocks
	memcpy(&SaveData::settingsData, &prevSettingsData, sizeof(FsettingsData));
	UpdateData();
}

bool Usettings::checkIfSettingsChanged() {
	return SaveData::settingsData != prevSettingsData;
}

// saves then leaves page
void Usettings::saveConfirm() {
	SaveSettings();
	goBack();
}

// reverts then leaves page
void Usettings::revertConfirm() {
	RevertSettings();
	goBack();
}

// cancels and returns to page
void Usettings::cancelConfirm() {
	showingConfirmationBox = false;
}