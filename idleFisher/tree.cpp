#include "tree.h"

#include "animation.h"
#include "Image.h"
#include "character.h"

Atree::Atree(vector loc, bool isTree) {
	this->isTree = isTree;

	if (isTree) {
		if (!treeSpriteSheet)
			treeSpriteSheet = std::make_shared<Image>("images/worlds/demo/treeSpriteSheet.png", vector{ 0, 0 }, true);
		if (!leafSpriteSheet)
			leafSpriteSheet = std::make_shared<Image>("images/landscape/leafParticles.png", vector{ 0, 0 }, true);

		std::unordered_map<std::string, animDataStruct> treeData;
		treeData.insert({ "tree", animDataStruct({0, 0}, {29, 0}, true) });
		vector cellSize(80, 135);
		this->loc = loc - vector{ cellSize.x / 2, cellSize.y };
		treeAnim = std::make_shared<animation>(treeSpriteSheet, cellSize.x, cellSize.y, treeData, true, this->loc);
		treeAnim->setAnimation("tree");
		treeAnim->start();
		std::unordered_map<std::string, animDataStruct> leafData;
		leafData.insert({ "leaf", animDataStruct({0, 0}, {42, 0}, true) });
		leafAnim = std::make_shared<animation>(leafSpriteSheet, 128, 69, leafData, true, this->loc + vector{ 8, 36 });
		leafAnim->setAnimation("leaf");
		leafAnim->start();

		// set to random frame
		treeAnim->SetCurrFrameLoc({ round(math::randRange(0.f, 29.f)), -1 });
		leafAnim->SetCurrFrameLoc({ round(math::randRange(0.f, 42.f)), -1 });
	} else {
		if (!bushSpriteSheet)
			bushSpriteSheet = std::make_shared<Image>("images/worlds/demo/bushSpriteSheet.png", vector{ 0, 0 }, true);

		std::unordered_map<std::string, animDataStruct> bushData;
		bushData.insert({ "bush", animDataStruct({0, 0}, {29, 0}, true) });
		vector cellSize(37, 34);
		this->loc = loc - vector{ cellSize.x / 2, cellSize.y };
		treeAnim = std::make_shared<animation>(bushSpriteSheet, cellSize.x, cellSize.y, bushData, true, this->loc);
		treeAnim->setAnimation("bush");
		treeAnim->start();

		treeAnim->SetCurrFrameLoc({ round(math::randRange(0.f, 29.f)), -1 });
	}
}

void Atree::draw(Shader* shaderProgram) {
	if (!treeAnim)
		return;

	treeAnim->draw(shaderProgram);
	if (leafAnim && isTree)
		leafAnim->draw(shaderProgram);
}

bool Atree::calcIfPlayerInfront() {
	if (treeAnim)
		return (GetCharacter()->getCharLoc().y < loc.y);
	return false;
}