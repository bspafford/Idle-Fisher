#include "hoverBox.h"
#include "Input.h"
#include "text.h"
#include "background.h"

#include "debugger.h"

UhoverBox::UhoverBox(widget* parent) : widget(parent) {
	float lineLength = 125.f;

	background = std::make_unique<Background>(this, "widget/background", glm::vec4(224.f / 255.f, 188.f / 255.f, 145.f / 255.f, 1.f));
	background->SetPivot({ 0.f, 1.f });
	name = std::make_unique<text>(this, " ", "biggerStraight", vector{0, 0});
	name->setLineLength(lineLength);
	name->SetPivot(vector(0.f, 1.f));
	description = std::make_unique<text>(this, " ", "straight", vector{ 0, 0 });
	description->setLineLength(lineLength);
	description->SetPivot(vector(0.f, 1.f));
	other = std::make_unique<text>(this, " ", "straight", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);
	other->setLineLength(lineLength);
	other->SetPivot(vector(0.f, 1.f));
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

	background->SetPivot(pivot);
	background->setLoc(mousePos + vector(7.f, 7.f) * vector(1.f - pivot.x * 2.f, 1.f - pivot.y * 2.f));
	Padding padding = background->GetBorderPadding();

	name->setLoc(background->getAbsoluteLoc() + vector{ padding.left + 1.f, background->getSize().y - padding.top - 1.f });
	description->setLoc(name->getAbsoluteLoc() - vector(0.f, textPadding));
	other->setLoc(description->getAbsoluteLoc() - vector(0.f, textPadding));

	background->draw(shaderProgram);
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

	float lineLength = name->getLineLength();
	Padding padding = background->GetBorderPadding();
	size = vector(lineLength + padding.left + padding.right, name->getSize().y + description->getSize().y + other->getSize().y + padding.top + padding.bottom + textPadding * 2.f);
	background->setSize(size);
}