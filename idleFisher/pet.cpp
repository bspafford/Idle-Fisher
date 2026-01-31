#include "pet.h"
#include "saveData.h"
#include "main.h"
#include "character.h"
#include "upgrades.h"

#include "debugger.h"

Apet::Apet(ModifierNode* pet, vector loc) {
	this->pet = pet;
	EquipPet(pet);
	img = std::make_unique<Image>("images/pets/" + std::to_string(pet->id) + ".png", vector{ 0, 0 }, false);
	setLoc(loc);

	Astar = std::make_unique<AStar>();
}

Apet::~Apet() {
	SaveData::saveData.equippedPetId = 0;
}

void Apet::update(float deltaTime) {

	// get if character is inside a valid spot
	if (Astar->nodeFromWorldPoint(GetCharacter()->getCharLoc())->walkable)
		Astar->startPathFinding(loc, GetCharacter()->getCharLoc());
	
	if (Astar->followingPath) {
		//vector diff = math::normalize(SaveData::saveData.playerLoc / stuff::pixelSize - loc) * deltaTime; // directly follow the player
		vector diff = Astar->followPath(loc, deltaTime, speed);
		setLoc(loc + diff * speed);
	}

	//setAnimation();
}

void Apet::draw(Shader* shaderProgram) {
	if (!SaveData::settingsData.showPets)
		return;

	//Astar->drawBoard(shaderProgram);

	img->draw(shaderProgram);
}

void Apet::setLoc(vector loc) {
	this->loc = loc;
	img->setLoc(loc - vector{ img->getSize().x / 2.f, img->getSize().y });
}

ModifierNode* Apet::getPetStruct() {
	return pet;
}

void Apet::EquipPet(ModifierNode* petData) {
	uint32_t oldPetId = SaveData::saveData.equippedPetId;

	// make sure to change id first, so MarkDirty knows is unequipped
	SaveData::saveData.equippedPetId = pet->id;

	if (oldPetId != 0) { // makes sure there was an previously equipped pet
		ModifierNode& oldPetData = SaveData::data.modifierData.at(oldPetId);
		Upgrades::MarkDirty(oldPetData.stats);
	}

	Upgrades::MarkDirty(petData->stats);
}