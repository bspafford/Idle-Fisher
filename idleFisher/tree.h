#pragma once

#include "math.h"

class animation;
class Shader;

class Atree {
public:
	Atree(vector loc, bool isTree);
	void draw(Shader* shaderProgram);

	bool calcIfPlayerInfront();

	vector loc;
private:
	std::shared_ptr<animation> treeAnim;
	std::shared_ptr<animation> leafAnim;

	bool isTree = false;

	static inline std::shared_ptr<Image> treeSpriteSheet;
	static inline std::shared_ptr<Image> leafSpriteSheet;
	static inline std::shared_ptr<Image> bushSpriteSheet;
};