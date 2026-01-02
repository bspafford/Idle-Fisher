#pragma once

#include "math.h"
#include "AStar.h"
#include "path.h"

class Shader;
struct FpetStruct;
struct FsavePetStruct;

class Apet {
public:
	Apet(FsavePetStruct* pet, vector loc);
	~Apet();
	void draw(Shader* shaderProgram);
	void update(float deltaTime);
	void setLoc(vector loc);
	FpetStruct* getPetStruct();
private:
	std::unique_ptr<AStar> Astar;

	float speed = 70;

	FpetStruct* pet;

	std::unique_ptr<Image> img;

	vector loc;
};