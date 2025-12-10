#include "currencyWidget.h"

#include "main.h"
#include "saveData.h"

#include "debugger.h"

UcurrencyWidget::UcurrencyWidget(widget* parent) : widget(parent) {

}

UcurrencyWidget::~UcurrencyWidget() {
	currencyList.clear();
}

void UcurrencyWidget::updateList() {
	float yOffset = 15 * stuff::pixelSize;

	// setup currency
	currencyList.clear();
	for (int i = 0; i < SaveData::saveData.currencyList.size(); i++) {
		FsaveCurrencyStruct saveCurrency = SaveData::saveData.currencyList[i];

		if (saveCurrency.numOwned != 0) {
			yOffset += stuff::pixelSize;

			std::unique_ptr<UfishNumWidget> widget = std::make_unique<UfishNumWidget>(this);
			
			FcurrencyStruct* currencyStruct = &SaveData::data.currencyData[saveCurrency.id];

			widget->setLoc(vector{ widget->getLoc().x, yOffset }.floor());
			widget->setup(currencyStruct, saveCurrency.numOwned);
			yOffset += widget->getSize().y + 1 * stuff::pixelSize;

			currencyList.push_back(std::move(widget));
		}
	}

}

void UcurrencyWidget::draw(Shader* shaderProgram) {
	float yOffset = 15 * stuff::pixelSize;
	for (int i = 0; i < currencyList.size(); i++) {
		currencyList[i]->setLoc(vector{ stuff::screenSize.x - currencyList[i]->getSize().x - 11 * stuff::pixelSize, yOffset}.floor());
		currencyList[i]->draw(shaderProgram);
		yOffset += currencyList[i]->getSize().y + 1 * stuff::pixelSize;
	}
}