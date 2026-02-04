#pragma once

#include <vector>
#include <unordered_map>

#include "math.h"
#include "widget.h"
#include "saveData.h"
#include "fishNumWidget.h"

class Shader;
class URectangle;
class verticalBox;
class UscrollBox;

class UheldFishWidget : public widget {
public:
	UheldFishWidget(widget* parent);

	// if fishList is empty it will use SaveData::data.fishData
	void updateList(bool useFishData, std::unordered_map<uint32_t, FsaveFishData> saveFishList = std::unordered_map<uint32_t, FsaveFishData>());

	void draw(Shader* shaderProgram);

private:
	void setupLocs() override;

	// removes all fish that there are 0 of
	std::unordered_map<uint32_t, FsaveFishData> removeUnneededFish();
	void UpdateCurrencyMap();
	int currencyInList(uint32_t id, const std::vector<std::pair<uint32_t, double>>& currencyList);

	std::unordered_map<uint32_t, FsaveFishData> fishList;
	// id, price * numOwned
	std::unordered_map<uint32_t, double> currency;

	std::vector<std::unique_ptr<UfishNumWidget>> fishNumList;
	std::vector< std::unique_ptr<UfishNumWidget>> currencyList;

	double numOwned = 0;

	std::unique_ptr<URectangle> line;

	std::unique_ptr<verticalBox> vertBox;
	std::unique_ptr<UscrollBox> fishScrollBox;
	std::unique_ptr<UscrollBox> currencyScrollBox;
};