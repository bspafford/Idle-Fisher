#pragma once

#include "widget.h"
#include "deferredPtr.h"

class text;
class Ubutton;
class UfishBox;
class Timer;
class animation;
class UprogressBar;
class UjournalProgressWidget;
struct FfishData;
struct FsaveFishData;

class Ujournal : public widget {
public:
	Ujournal(widget* parent);
	~Ujournal();

	void draw(Shader* shaderProgram);

	void openFishPage(FfishData* fishData, FsaveFishData* saveFishData);


private:
	void forwardPage();
	void backwardPage();
	void closeWidget();
	void updatePages();
	void addedToViewport() override;
	void setupLocs() override;

	void removeFromViewport() override;

	void calcWorldPercentage(UprogressBar* normalProgressBar, UprogressBar* rareProgressBar, int worldId);

	std::unique_ptr<Image> background;
	std::unique_ptr<animation> journalAnim;
	std::unique_ptr<Image> journalClosed;

	DeferredPtr<Timer> journalTimer;

	void moveAnim();
	void journalTimerFinish();
	void journalAnimFinish();

	bool opening = false;
	bool closing = false;
	// if journal is all the way open
	bool open = false;

	std::unique_ptr<Ubutton> forwardButton;
	std::unique_ptr<Ubutton> backButton;
	std::unique_ptr<Ubutton> xButton;
	std::unique_ptr<text> worldName1;
	std::unique_ptr<text> worldName2;

	std::unique_ptr<UprogressBar> worldProgress1;
	std::unique_ptr<UprogressBar> worldProgress2;

	std::unique_ptr<UjournalProgressWidget> journalProgressWidget;
	int unlockedFish1 = 0, fishStars1 = 0, maxFishSize1 = 0;
	int unlockedFish2 = 0, fishStars2 = 0, maxFishSize2 = 0;
	float unlockedTotal1 = 0, starsTotal1 = 0, SizeTotal1 = 0;
	float unlockedTotal2 = 0, starsTotal2 = 0, SizeTotal2 = 0;

	std::vector<std::unique_ptr<UfishBox>> fishBoxList;

	int pageNum = 0;
	int tempPageNum = 0; // used to get previous page when going back from selected fish

	// selected fish page
	FfishData* selectedFish;
	FsaveFishData* selectedSaveFish;

	std::unique_ptr<Image> fishThumbnail;
	std::unique_ptr<Image> notesBackground;
	std::unique_ptr<text> selectedFishName;
	std::unique_ptr<text> selectedFishDescription;

	std::unique_ptr<text> statsTitle;
	std::unique_ptr<text> baseCurrency;
	std::unique_ptr<text> baseCurrencyNum;
	std::unique_ptr<text> currency;
	std::unique_ptr<text> currencyNum;
	std::unique_ptr<text> caught;
	std::unique_ptr<text> caughtNum;
	std::unique_ptr<text> power;
	std::unique_ptr<text> powerNum;
	std::unique_ptr<text> speed;
	std::unique_ptr<text> speedNum;
	std::unique_ptr<text> yellow;
	std::unique_ptr<text> yellowNum;
	std::unique_ptr<text> green;
	std::unique_ptr<text> greenNum;
	std::unique_ptr<text> probability;
	std::unique_ptr<text> probabilityNum;
	std::unique_ptr<text> fishSize;
	std::unique_ptr<text> fishSizeNum;

	std::unique_ptr<Image> star1;
	std::unique_ptr<Image> star2;
	std::unique_ptr<Image> star3;
	std::unique_ptr<text> star1Text;
	std::unique_ptr<text> star2Text;
	std::unique_ptr<text> star3Text;

	std::unique_ptr<Image> map;
};