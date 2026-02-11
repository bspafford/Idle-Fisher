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
	exclamationPointAnimData.insert({ "anim", animDataStruct({0, 0}, {4, 0}, true) });
	exclamationPointAnim = std::make_unique<animation>("widget/npcButtons/exclamationPoint.png", 5, 13, exclamationPointAnimData, true, loc, false);
	exclamationPointAnim->setAnimation("anim");
	exclamationPointAnim->start();

	openAudio = std::make_unique<Audio>("openUI.wav", AudioType::SFX);
	npcAudio = std::make_unique<Audio>("npc1.wav", AudioType::SFX);
}

npc::~npc() {
	auto it = std::find(instances.begin(), instances.end(), this);
	if (it != instances.end())
		instances.erase(it);

	// remove col
	collision::removeCollisionObject(col.get());
}

void npc::setup(std::string npcName) {
	// calc frame num
	if (npcName != "fishTransporter") {
		widget = std::make_unique<NPCwidget>(nullptr, this, npcName);

		int fameNum = textureManager::getTexture("images/npc/" + npcName + ".png")->w / npcCellSizes[npcName].x - 1;

		std::unordered_map<std::string, animDataStruct> npcAnimData;
		npcAnimData.insert({ "idle", animDataStruct({0, 0}, {float(fameNum), 0}, true) });
		vector cellSize = npcCellSizes[npcName];
		npcAnim = std::make_unique<animation>("npc/" + npcName + ".png", cellSize.x, cellSize.y, npcAnimData, true, loc);

		npcAnim->SetUseAlpha(true);
		npcAnim->setAnimation("idle");
		npcAnim->start();
		npcAnim->SetCurrFrameLoc({ round(math::randRange(0.f, fameNum)), -1 });

		setLoc(loc);
		setupCollision();
	}
}

void npc::setupCollision() {
	// draw by feet / bottom of img
	vector offset = getOffset();
	float radius = 7;

	col = std::make_unique<Fcollision>(offset, radius, ' ');
	collision::addCollisionObject(col.get());
}

void npc::draw(Shader* shaderProgram) {
	bool prevMouseOver = bMouseOver;
	bool canReach = GetCharacter()->CanPlayerReach(loc);
	bool bMouseOver = canReach && npcAnim->IsMouseOver(true);
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

	npcAnim->setLoc(loc - npcAnim->GetCellSize() / vector{ 2.f, 1.f });
	exclamationPointAnim->setLoc(npcAnim->getLoc() + vector{npcAnim->GetCellSize().x / 2.f - exclamationPointAnim->GetCellSize().x / 2, float(npcAnim->GetCellSize().y + 1)});
}

void npc::click() {
	widget->addToViewport(nullptr);
	openAudio->Play();
	npcAudio->Play();
}

bool npc::isDiscovered() {
	if (discovered)
		return *discovered;
	return false;
}

bool npc::calcIfPlayerInfront() {
	if (npcAnim) {
		vector charLoc = GetCharacter()->getCharLoc();
		vector npcLoc = loc - vector{ 0, npcAnim->GetCellSize().y * 1.f };
		return (charLoc.y < npcLoc.y);
	}
	return false;
}

vector npc::getOffset() {
	return loc - vector{ 0, float(npcAnim->GetCellSize().y) };
}