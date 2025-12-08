#include "decorationBox.h"
#include "decoratorWidget.h"
#include "saveData.h"
#include "shortNumbers.h"
#include "button.h"
#include "text.h"

#include "debugger.h"

UdecorationBox::UdecorationBox(widget* parent, UdecoratorWidget* parentRef, widget* buttonParent, FvaultUnlocksStruct* vaultUnlock, FsaveVaultUnlocksStruct* saveVaultUnlock) : widget(parent) {
	this->parentRef = parentRef;
	this->setParent(parentRef);
	this->vaultUnlock = vaultUnlock;
	this->saveVaultUnlock = saveVaultUnlock;

	thumbnail = std::make_unique<Image>(vaultUnlock->thumbnailPath, vector{ 0, 0 }, false);
	buyButton = std::make_unique<Ubutton>(parentRef, "button.png", 21, 7, 1, vector{ 0, 0 }, false, false);
	buyButton->addCallback(this, &UdecorationBox::buyDecoration);
	buyButton->setParent(buttonParent);
	buttonText = std::make_unique<text>(this, " ", "straight", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);
}

UdecorationBox::~UdecorationBox() {

}

void UdecorationBox::draw(Shader* shaderProgram) {
	thumbnail->draw(shaderProgram);
	buyButton->draw(shaderProgram);
	buttonText->draw(shaderProgram);
}

void UdecorationBox::buyDecoration() {
	if (!saveVaultUnlock->unlocked) {
		// buy
		saveVaultUnlock->unlocked = true;
	} else if (!saveVaultUnlock->placed) {
		// place
		parentRef->placeItem(vaultUnlock, saveVaultUnlock);
	} else {
		// remove
		saveVaultUnlock->placed = false;
		parentRef->removeItem(vaultUnlock, saveVaultUnlock);
	}

	updateWidget();
}

void UdecorationBox::updateWidget() {
	if (!saveVaultUnlock->unlocked) {
		// buy
		buttonText->setText(shortNumbers::convert2Short(vaultUnlock->currencyNum));
	} else if (!saveVaultUnlock->placed) {
		// place
		buttonText->setText("Place");
	} else {
		// remove
		buttonText->setText("Remove");
	}
}

void UdecorationBox::setLoc(vector loc) {
	__super::setLoc(loc);

	thumbnail->setLoc(loc);
	buyButton->setLoc(loc + vector{ 0, (float)thumbnail->h * stuff::pixelSize });
	buttonText->setLoc(loc + vector{ buyButton->getSize().x / 2, (float)thumbnail->h * stuff::pixelSize });
}

vector UdecorationBox::getSize() {
	return buyButton->getSize() + 3 * stuff::pixelSize;
}