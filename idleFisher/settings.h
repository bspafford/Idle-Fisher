#pragma once

#include "widget.h"
#include "saveData.h"

class text;
class UscrollBox;
class Ubutton;
class Uslider;
class horizontalBox;
class UsettingsBlock;
class ConfirmWidget;

class Usettings : public widget {
public:
	Usettings(widget* parent);
	~Usettings();
	void draw(Shader* shaderProgram);
	void setupLocs() override;
private:
	virtual void addedToViewport() override;

	void SaveSettings();

	// Updates data if the save data has changed since last open
	void UpdateData();

	virtual void removeFromViewport() override;

	void goBack();
	void cancel();

	void RevertSettings();

	bool checkIfSettingsChanged();

	void saveConfirm();
	void revertConfirm();
	void cancelConfirm();

	bool showingConfirmationBox = false;

	// keeps track of which data was changed
	FsettingsData prevSettingsData;

	std::unique_ptr<Image> background;
	std::unique_ptr<Ubutton> backButton;
	std::unique_ptr<UscrollBox> scrollBox;

	// settings
	std::unique_ptr<text> settingsTitle;
	std::unique_ptr<Ubutton> saveButton;
	std::unique_ptr<text> saveText;
	std::unique_ptr<Ubutton> cancelButton;
	std::unique_ptr<text> cancelText;

	// audio
	std::unique_ptr<text> audioTitle;
	std::unique_ptr<Uslider> masterVolumeSlider;
	std::unique_ptr<Uslider> musicVolume;
	std::unique_ptr<Uslider> sfxVolume;
	std::unique_ptr<Uslider> dialogVolume;


	// graphics
	std::unique_ptr<text> graphicsTitle;
	std::unique_ptr<UsettingsBlock> pixelFontBlock;
	std::unique_ptr<UsettingsBlock> shortNumBlock;
	std::unique_ptr<UsettingsBlock> petBlock;
	std::unique_ptr<UsettingsBlock> rainBlock;
	std::unique_ptr<UsettingsBlock> cursorBlock;

	std::unique_ptr<ConfirmWidget> confirmWidget;
};