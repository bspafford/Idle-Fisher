#include "tree.h"

#include "animation.h"
#include "Image.h"

Atree::Atree(vector loc, bool isTree) {
	this->isTree = isTree;
	this->loc = loc;

	if (isTree) {
		if (!treeSpriteSheet)
			treeSpriteSheet = std::make_shared<Image>("./images/landscape/tree.png", vector{ 0, 0 }, true);
		if (!leafSpriteSheet)
			leafSpriteSheet = std::make_shared<Image>("./images/landscape/leafParticles.png", vector{ 0, 0 }, true);

		std::unordered_map<std::string, animDataStruct> treeData;
		treeData.insert({ "tree", animDataStruct({0, 0}, {29, 0}, true) });
		loc = loc - vector{ 94 / 2, 122 };
		treeAnim = std::make_shared<animation>(treeSpriteSheet, 94, 122, treeData, true, loc);
		treeAnim->setAnimation("tree");
		treeAnim->start();
		std::unordered_map<std::string, animDataStruct> leafData;
		leafData.insert({ "leaf", animDataStruct({0, 0}, {42, 0}, true) });
		leafAnim = std::make_shared<animation>(leafSpriteSheet, 128, 69, leafData, true, loc + vector{ 24, 47 });
		leafAnim->setAnimation("leaf");
		leafAnim->start();

		// set to random frame
		treeAnim->currFrameLoc.x = round(math::randRange(0.f, 29.f));
		leafAnim->currFrameLoc.x = round(math::randRange(0.f, 42.f));
	} else {
		if (!bushSpriteSheet)
			bushSpriteSheet = std::make_shared<Image>("./images/landscape/bush.png", vector{ 0, 0 }, true);

		std::unordered_map<std::string, animDataStruct> bushData;
		bushData.insert({ "bush", animDataStruct({0, 0}, {29, 0}, true) });
		loc = loc - vector{ 37 / 2, 33 };
		treeAnim = std::make_shared<animation>(bushSpriteSheet, 37, 33, bushData, true, loc);
		treeAnim->setAnimation("bush");
		treeAnim->start();

		treeAnim->currFrameLoc.x = round(math::randRange(0.f, 29.f));
	}
}

void Atree::draw(Shader* shaderProgram) {
	if (!treeAnim)
		return;

	treeAnim->draw(shaderProgram);
	if (leafAnim && isTree)
		leafAnim->draw(shaderProgram);
}