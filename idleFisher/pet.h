#pragma once

#include "math.h"
#include "AStar.h"
#include "path.h"

class Shader;
struct ModifierNode;

class Apet {
public:
	Apet(ModifierNode* pet, vector loc);
	~Apet();
	void draw(Shader* shaderProgram);
	void update(float deltaTime);
	void setLoc(vector loc);
	ModifierNode* getPetStruct();
private:
	std::unique_ptr<AStar> Astar;

	float speed = 70;

	ModifierNode* pet;

	std::unique_ptr<Image> img;

	vector loc;
};