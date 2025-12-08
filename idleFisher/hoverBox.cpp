#include "hoverBox.h"

#include "Input.h"
#include "saveData.h"

#include "text.h"

#include "debugger.h"

UhoverBox::UhoverBox(widget* parent) : widget(parent) {
	img = std::make_unique<Image>("./images/widget/hoverImg.png", Input::getMousePos(), false);
	name = std::make_unique<text>(this, " ", "straight", vector{0, 0});
	description = std::make_unique<text>(this, " ", "straight", vector{ 0, 0 });
	description->setLineLength(250);
	other = std::make_unique<text>(this, " ", "straight", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);
	size = img->getSize();
}

UhoverBox::~UhoverBox() {

}

void UhoverBox::draw(Shader* shaderProgram) {
	if (!visible)
		return;
	// if right is off screen, add offset
	// same for bottom

	vector mousePos = Input::getMousePos();
	if (mousePos.x + size.x + 7 * stuff::pixelSize >= stuff::screenSize.x) // out of range on right
		img->setLoc({ mousePos.x - size.x - 7 * stuff::pixelSize, img->getLoc().y });
	else
		img->setLoc({ mousePos.x + 7 * stuff::pixelSize, img->getLoc().y });

	if (mousePos.y + size.y + 7 * stuff::pixelSize >= stuff::screenSize.y) // out of range on bottom
		img->setLoc({ img->getLoc().x, mousePos.y - size.y - 7 * stuff::pixelSize });
	else
		img->setLoc({ img->getLoc().x, mousePos.y + 7 * stuff::pixelSize });

	name->setLoc(img->getLoc() + 5 * stuff::pixelSize);
	description->setLoc({ img->getLoc() + vector{5, 11} *stuff::pixelSize});
	other->setLoc(img->getLoc() + vector{img->getSize().x, 0} + vector{-5, 5} *stuff::pixelSize);

	img->draw(shaderProgram);
	name->draw(shaderProgram);
	description->draw(shaderProgram);
	other->draw(shaderProgram);
}

void UhoverBox::setInfo(std::string newName, std::string newDescription, std::string newOther) {
	if (name->getString() == newName && description->getString() == newDescription && other->getString() == newOther) // if still the same data
		return;

	name->setText(newName);
	description->setText(newDescription);
	other->setText(newOther);
}