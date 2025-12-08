#include "npc.h"
#include "Input.h"
#include "character.h"
#include "animation.h"
#include "textureManager.h"
#include "Image.h"

#include "debugger.h"

npc::npc(vector loc) {
	instances.push_back(this);
	this->loc = loc;

	std::unordered_map<std::string, animDataStruct> exclamationPointAnimData;
	exclamationPointAnimData.insert({ "anim", {{0, 0}, {4, 0}, .1, true} });
	exclamationPointAnim = std::make_unique<animation>("widget/npcButtons/exclamationPoint.png", 5, 13, exclamationPointAnimData, true, loc);
	exclamationPointAnim->setAnimation("anim");
	exclamationPointAnim->start();
}

npc::~npc() {
	auto it = std::find(instances.begin(), instances.end(), this);
	if (it != instances.end())
		instances.erase(it);

	// remove col
	collision::removeCollisionObject(col.get());
}

void npc::setup(std::string npcName) {
	std::unordered_map<std::string, vector> npcCellSizes;
	npcCellSizes.insert({ "atm", { 28, 47 } });
	npcCellSizes.insert({ "decorator", { 29, 54 } });
	npcCellSizes.insert({ "fisherman", { 34, 54 } });
	npcCellSizes.insert({ "mechanic", { 32, 58 } });
	npcCellSizes.insert({ "merchant", { 19, 49 } });
	npcCellSizes.insert({ "petSeller", { 36, 51 } });
	npcCellSizes.insert({ "sailor", { 21, 49 } });
	npcCellSizes.insert({ "scuba", { 18, 45 } });
	npcCellSizes.insert({ "fishGod", { 514, 479 } });

	// calc frame num
	if (npcName != "fishTransporter") {
		widget = std::make_unique<NPCwidget>(nullptr, this, npcName);

		int fameNum = textureManager::getTexture("./images/npc/" + npcName + ".png")->w / npcCellSizes[npcName].x - 1;

		std::unordered_map<std::string, animDataStruct> npcAnimData;
		npcAnimData.insert({ "idle", {{0, 0}, {float(fameNum), 0}, .1, true} });
		vector cellSize = npcCellSizes[npcName];
		npcAnim = std::make_unique<animation>("npc/" + npcName + ".png", cellSize.x, cellSize.y, npcAnimData, true, loc);

		npcAnim->spriteSheet->setUseAlpha(true);
		npcAnim->setAnimation("idle");
		npcAnim->start();
		npcAnim->currFrameLoc.x = round(math::randRange(0, fameNum));

		setLoc(loc);
		setupCollision();
	}
}

void npc::setupCollision() {
	// draw by feet / bottom of img
	vector offset = getOffset();
	float radius = 7;

	col = std::make_unique<Fcollision>(offset, radius, "");
	collision::addCollisionObject(col.get());
}

void npc::draw(Shader* shaderProgram) {
	bool prevMouseOver = bMouseOver;
	bool bMouseOver = npcAnim->spriteSheet->isMouseOver(true);
	if (bMouseOver && Input::getMouseButtonDown(MOUSE_BUTTON_LEFT))
		Input::setLeftClick(this, &npc::click);
	if (bMouseOver)
		IHoverable::setHoveredItem(this);

	npcAnim->draw(shaderProgram);

	if (!isDiscovered())
		exclamationPointAnim->draw(shaderProgram);
}

void npc::setLoc(vector loc) {
	this->loc = loc;

	npcAnim->setLoc(loc - vector{ npcAnim->cellWidth / 2.f, float(npcAnim->cellHeight) });
	exclamationPointAnim->setLoc(npcAnim->getLoc() + vector{npcAnim->cellWidth / 2.f - exclamationPointAnim->cellWidth / 2, float(npcAnim->cellHeight + 1 * stuff::pixelSize) });
}

void npc::click() {
	widget->addToViewport(true);
}

bool npc::isDiscovered() {
	if (discovered)
		return discovered;
	return false;
}

bool npc::calcIfPlayerInfront() {
	if (npcAnim) {
		vector charLoc = Acharacter::getCharLoc();
		vector npcLoc = loc - vector{ 0, npcAnim->cellHeight * 1.f };
		return (charLoc.y < npcLoc.y);
	}
	return false;
}

vector npc::getOffset() {
	return loc - vector{ 0, float(npcAnim->cellHeight) };
}

std::weak_ptr<Image> npc::getCharImg() {
	return npcAnim->spriteSheet;
}