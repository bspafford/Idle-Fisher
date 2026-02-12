#include "settings.h"
#include "main.h"
#include "math.h"
#include "text.h"
#include "scrollBox.h"
#include "button.h"
#include "slider.h"
#include "saveData.h"
#include "settingsBlock.h"
#include "confirmWidget.h"
#include "textureManager.h"
#include "Cursor.h"

#include "debugger.h"

Usettings::Usettings(widget* parent) : widget(parent) {
	// button click anim
	std::vector<std::string> buttonClickAnim;
	for (int i = 0; i < 2; i++)
		buttonClickAnim.push_back("images/widget/upgradeButton" + std::to_string(i + 1) + ".png");

	background = std::make_unique<Image>("images/widget/achievementBackground.png", vector{ 0, 0 }, false);
	background->SetAnchor(ANCHOR_CENTER, ANCHOR_CENTER);
	background->SetPivot({ 0.5f, 0.5f });

	backButton = std::make_unique<Ubutton>(this, "widget/npcXButton.png", 11, 11, 1, vector{ 0, 0 }, false, false);
	backButton->SetPivot({ 0.5f, 0.5f });
	backButton->addCallback(this, &Usettings::goBack);

	scrollBox = std::make_unique<UscrollBox>(this);

	settingsTitle = std::make_unique<text>(this, "Settings", "biggerStraight", vector{ 0, 0 });
	audioTitle = std::make_unique<text>(this, "  Audio:", "biggerStraight", vector{ 0, 0 });
	graphicsTitle = std::make_unique<text>(this, "  Graphics:", "biggerStraight", vector{ 0, 0 });
	miscTitle = std::make_unique<text>(this, "  Misc:", "biggerStraight", vector{ 0, 0 });

	// padding
	scrollBox->addChild(nullptr, 6);

// settings
	scrollBox->addChild(settingsTitle.get(), settingsTitle->getSize().y + 3);

	saveButton = std::make_unique<Ubutton>(this, "widget/upgradeButton.png", 37, 16, 1, vector{ 0, 0 }, false, false);
	saveButton->SetPivot({ 1.f, 0.f });
	saveButton->addCallback(this, &Usettings::SaveSettings);
	saveText = std::make_unique<text>(this, "Save", "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);
	saveText->SetPivot({ 0.f, 0.5f });

	cancelButton = std::make_unique<Ubutton>(this, "widget/upgradeButton.png", 37, 16, 1, vector{ 0, 0 }, false, false);
	cancelButton->SetPivot({ 1.f, 0.f });
	cancelButton->addCallback(this, &Usettings::cancel);
	
	cancelText = std::make_unique<text>(this, "Revert", "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);
	cancelText->SetPivot({ 0.f, 0.5f });

// audio
	scrollBox->addChild(audioTitle.get(), audioTitle->getSize().y + 3);

	float length = background->getSize().x - 50;
	float sliderHeight = 3;
	float titleSliderLength = 38;
	glm::vec4 sliderForegroundColor = glm::vec4(0.94901960784, 0.91372549019, 0.82745098039, 1);
	glm::vec4 sliderBackgroundColor = glm::vec4(181.f / 255.f, 145.f / 255.f, 101.f/255.f, 1);
	masterVolumeSlider = std::make_unique<Uslider>(this, false, vector{ length, sliderHeight }, 0.f, 100.f);
	masterVolumeSlider->setForegroundColor(sliderForegroundColor);
	masterVolumeSlider->setBackgroundColor(sliderBackgroundColor);
	masterVolumeSlider->setSliderTitle("     Master");
	masterVolumeSlider->bindValue(&SaveData::settingsData.masterVolume);
	masterVolumeSlider->setTitleLength(titleSliderLength);

	musicVolume = std::make_unique<Uslider>(this, false, vector{ length, sliderHeight }, 0.f, 100.f);
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

	ambientVolume = std::make_unique<Uslider>(this, false, vector{ length, sliderHeight }, 0, 100);
	ambientVolume->setForegroundColor(sliderForegroundColor);
	ambientVolume->setBackgroundColor(sliderBackgroundColor);
	ambientVolume->setSliderTitle("     Ambient");
	ambientVolume->bindValue(&SaveData::settingsData.ambientVolume);
	ambientVolume->setTitleLength(titleSliderLength);

	float sliderPadding = 17;
	scrollBox->addChild(masterVolumeSlider.get(), masterVolumeSlider->getSize().y + sliderPadding);
	scrollBox->addChild(musicVolume.get(), musicVolume->getSize().y + sliderPadding);
	scrollBox->addChild(sfxVolume.get(), sfxVolume->getSize().y + sliderPadding);
	scrollBox->addChild(ambientVolume.get(), ambientVolume->getSize().y + sliderPadding);
	scrollBox->addChild(nullptr, 5);

// graphics
	scrollBox->addChild(graphicsTitle.get(), graphicsTitle->getSize().y + 3);

	monitorBlock = std::make_unique<UsettingsBlock>(this, "Monitor", length, std::vector<std::string>{}, & SaveData::settingsData.monitorIdx);
	monitorBlock->addCallback(Main::SetFullScreen);
	getAllMonitors();
	scrollBox->addChild(monitorBlock.get(), monitorBlock->getSize().y);

	fullScreenBlock = std::make_unique<UsettingsBlock>(this, "Full Screen", length, std::vector<std::string>{ "Full Screen", "Borderless", "Windowed" }, & SaveData::settingsData.fullScreen);
	fullScreenBlock->addCallback(Main::SetFullScreen);
	scrollBox->addChild(fullScreenBlock.get(), fullScreenBlock->getSize().y);
	// drop down
	resolutionBlock = std::make_unique<UsettingsBlock>(this, "Resolution", length, std::vector<std::string>{ "Native", "1280x720", "1920x1080", "2560x1440" }, & SaveData::settingsData.resolution);
	resolutionBlock->addCallback(Main::SetResolution);
	scrollBox->addChild(resolutionBlock.get(), resolutionBlock->getSize().y);
	vsyncBlock = std::make_unique<UsettingsBlock>(this, "Vsync", length, std::vector<std::string>{ "Off", "On" }, &SaveData::settingsData.vsync);
	vsyncBlock->addCallback(Main::SetVsync);
	scrollBox->addChild(vsyncBlock.get(), vsyncBlock->getSize().y);
	// drop down
	fpsLimitBlock = std::make_unique<UsettingsBlock>(this, "Limit FPS", length, std::vector<std::string>{ "Off", "1", "30", "60", "120", "240" }, & SaveData::settingsData.fpsLimit);
	fpsLimitBlock->addCallback(Main::SetFpsLimit);
	scrollBox->addChild(fpsLimitBlock.get(), fpsLimitBlock->getSize().y);

	//pixelFontBlock = std::make_unique<UsettingsBlock>(this, "Pixel Font", length, std::vector<std::string>{ "Off", "On" }, &SaveData::settingsData.pixelFont);
	//pixelFontBlock->addCallback(text::changeFontAll);
	//scrollBox->addChild(pixelFontBlock.get(), pixelFontBlock->getSize().y);

	//shortNumBlock = std::make_unique<UsettingsBlock>(this, "Enable Short Numbers", length, std::vector<std::string>{ "Long", "Short" }, &SaveData::settingsData.shortNumbers);
	//scrollBox->addChild(shortNumBlock.get(), shortNumBlock->getSize().y);

	petBlock = std::make_unique<UsettingsBlock>(this, "Show Pets", length, std::vector<std::string>{ "Off", "On" }, &SaveData::settingsData.showPets);
	scrollBox->addChild(petBlock.get(), petBlock->getSize().y);

	// show rain
	rainBlock = std::make_unique<UsettingsBlock>(this, "Show Rain", length, std::vector<std::string>{ "Off", "On" }, &SaveData::settingsData.showRain);
	scrollBox->addChild(rainBlock.get(), rainBlock->getSize().y);

	// show cursor
	cursorBlock = std::make_unique<UsettingsBlock>(this, "Cursor", length, std::vector<std::string>{ "Off", "On" }, &SaveData::settingsData.cursor);
	cursorBlock->addCallback(Cursor::toggleCursor);
	scrollBox->addChild(cursorBlock.get(), cursorBlock->getSize().y);

// Misc
	scrollBox->addChild(miscTitle.get(), miscTitle->getSize().y + 3.f);

	movementBlock = std::make_unique<UsettingsBlock>(this, "Movement", length, std::vector<std::string>{ "Isometric", "Normal" }, & SaveData::settingsData.movement);
	scrollBox->addChild(movementBlock.get(), movementBlock->getSize().y);

	//interpMethodBlock = std::make_unique<UsettingsBlock>(this, "Interpolation", length, std::vector<std::string>{ "Nearest", "Linear" }, &SaveData::settingsData.interpMethod);
	//interpMethodBlock->addCallback(this, &Usettings::SetInterpMethod);
	//scrollBox->addChild(interpMethodBlock.get(), interpMethodBlock->getSize().y);
	
	// add bottom padding
	scrollBox->addChild(nullptr, 20);

	confirmWidget = std::make_unique<ConfirmWidget>(this);
	confirmWidget->AddSaveCallback(this, &Usettings::saveConfirm);
	confirmWidget->AddRevertCallback(this, &Usettings::revertConfirm);
	confirmWidget->AddCancelCallback(this, &Usettings::cancelConfirm);

	setupLocs();
	
	CheckSettings();
}

Usettings::~Usettings() {

}

void Usettings::draw(Shader* shaderProgram) {
	CheckSettings();

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
	// apply all of the settings / call all callback functions
	for (UsettingsBlock* settingsBlock : settingsChangedQueue)
		settingsBlock->CallCallback();
	settingsChangedQueue.clear();

	prevSettingsData = SaveData::settingsData;
	SaveData::saveSettings();
}

void Usettings::CheckSettings() {
	resolutionBlock->SetEnabled(SaveData::settingsData.fullScreen == 2); // only enable resolution setting if windowed
	fpsLimitBlock->SetEnabled(SaveData::settingsData.vsync == 0); // only allow fps limit if vsync is off
}

void Usettings::UpdateData() {
	// audio
	masterVolumeSlider->UpdateValue();
	musicVolume->UpdateValue();
	sfxVolume->UpdateValue();
	ambientVolume->UpdateValue();

	// graphics
	fullScreenBlock->UpdateValue();
	vsyncBlock->UpdateValue();
	resolutionBlock->UpdateValue();
	fpsLimitBlock->UpdateValue();
	//pixelFontBlock->UpdateValue();
	//shortNumBlock->UpdateValue();
	petBlock->UpdateValue();
	rainBlock->UpdateValue();
	cursorBlock->UpdateValue();
	//interpMethodBlock->UpdateValue();
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
}

void Usettings::goBack() {
	removeFromViewport();
}

void Usettings::setupLocs() {
	if (backButton)
		backButton->setLoc(background->getAbsoluteLoc() + background->getSize() - 3.f);

	vector scrollBoxLoc = background->getAbsoluteLoc() + vector{ 9.f, 8.f };
	vector scrollBoxSize = background->getSize() - 17.f;
	scrollBox->setLocAndSize(scrollBoxLoc, scrollBoxSize);

	cancelButton->setLoc(background->getAbsoluteLoc() + vector{ background->getSize().x - 10.f, 10.f });
	cancelText->setLoc(cancelButton->getAbsoluteLoc() + cancelButton->getSize() / 2.f);
	saveButton->setLoc(cancelButton->getAbsoluteLoc() - vector{ 2.f, 0 });
	saveText->setLoc(saveButton->getAbsoluteLoc() + saveButton->getSize() / 2.f);
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
	showingConfirmationBox = false;
	removeFromViewport();
}

// reverts then leaves page
void Usettings::revertConfirm() {
	RevertSettings();
	showingConfirmationBox = false;
	removeFromViewport();
}

// cancels and returns to page
void Usettings::cancelConfirm() {
	showingConfirmationBox = false;
}

void Usettings::AddToQueue(UsettingsBlock* settingsBlock) {
	settingsChangedQueue.push_back(settingsBlock);
}

void Usettings::getAllMonitors() {
	monitorBlock->ClearOptions();

	int count;
	GLFWmonitor** monitors = glfwGetMonitors(&count);
	for (int i = 0; i < count; i++) {
		if (i == 0)
			monitorBlock->AddOption("Primary");
		else {
			std::string monitorName = glfwGetMonitorName(monitors[i]);
			if (monitorName.size() > 10)
				monitorName = monitorName.substr(0, 7) + "...";
			monitorBlock->AddOption(monitorName);
		}
	}

	monitorBlock->UpdateValue();
}

void Usettings::SetInterpMethod() {
	switch (SaveData::settingsData.interpMethod) {
	case 0:
		textureManager::SetInterpMethod(GL_NEAREST);
		break;
	case 1:
		textureManager::SetInterpMethod(GL_LINEAR);
		break;
	}
}