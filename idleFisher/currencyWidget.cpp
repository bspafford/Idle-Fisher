#include "currencyWidget.h"

#include "main.h"
#include "saveData.h"

#include "debugger.h"

UcurrencyWidget::UcurrencyWidget(widget* parent) : widget(parent) {
	SetAnchor(ANCHOR_RIGHT, ANCHOR_TOP);
	SetPivot({ 0.f, 1.f });
	setLoc({ -45.f, 0.f });
}

UcurrencyWidget::~UcurrencyWidget() {
	currencyList.clear();
}

void UcurrencyWidget::updateList() {
	float yOffset = -15.f;

	// setup currency
	currencyList.clear();
	for (int i = 0; i < SaveData::saveData.currencyList.size(); i++) {
		FsaveCurrencyStruct saveCurrency = SaveData::saveData.currencyList[i];

		if (saveCurrency.numOwned != 0) {
			yOffset -= stuff::pixelSize;

			std::unique_ptr<UfishNumWidget> widget = std::make_unique<UfishNumWidget>(this);
			widget->SetPivot(pivot);

			FcurrencyStruct* currencyStruct = &SaveData::data.currencyData[saveCurrency.id];

			widget->setLoc(vector{ widget->getLoc().x, yOffset }.floor());
			widget->setup(currencyStruct, saveCurrency.numOwned);
			yOffset -= widget->getSize().y + 1.f;

			currencyList.push_back(std::move(widget));
		}
	}

}

void UcurrencyWidget::draw(Shader* shaderProgram) {
	float yOffset = absoluteLoc.y - 6.f;
	for (int i = 0; i < currencyList.size(); i++) {
		currencyList[i]->setLoc(vector{ absoluteLoc.x - 3.f, yOffset}.floor());
		currencyList[i]->draw(shaderProgram);
		yOffset -= currencyList[i]->getSize().y + 1;
	}
}