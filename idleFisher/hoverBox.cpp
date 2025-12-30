#include "hoverBox.h"

#include "Input.h"
#include "saveData.h"

#include "text.h"

#include "debugger.h"

UhoverBox::UhoverBox(widget* parent) : widget(parent) {
	img = std::make_unique<Image>("./images/widget/hoverImg.png", Input::getMousePos(), false);
	img->SetPivot({ 0.f, 1.f });
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

	vector mousePos = Input::getMousePos();
	vector screenSize = stuff::screenSize / (stuff::pixelSize / 2.f);
	vector pivot = { 0.f, 0.f };
	if (mousePos.x + size.x + 7.f >= screenSize.x) // out of range on right
		pivot.x = 1.f;
	else
		pivot.x = 0.f;
	
	if (mousePos.y + size.y + 7.f >= screenSize.y) // out of range on bottom
		pivot.y = 0.f;
	else
		pivot.y = 1.f;

	img->SetPivot(pivot);
	img->setLoc(mousePos + vector{ 7.f, 7.f } * vector{ 1.f - pivot.x * 2.f, 1.f - pivot.y * 2.f });

	name->setLoc(img->getAbsoluteLoc() + 5.f);
	description->setLoc(img->getAbsoluteLoc() + vector{ 5.f, 11.f });
	other->setLoc(img->getAbsoluteLoc() + vector{ img->getSize().x - 5.f, 5.f });

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