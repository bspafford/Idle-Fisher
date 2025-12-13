#pragma once

#include "math.h"
#include "Image.h"
#include "shaderClass.h"

struct Fcollision;

class Abuilding {
public:
	Abuilding(vector loc);
	~Abuilding();
	virtual void draw(Shader* shaderProgram);
	virtual void setLoc(vector loc);

	// gets the furthest left and right point of the collision
	// then makes a line that if the character has a great y at that x location, then the character is behind
	bool calcInFront();
	void setupCollision(std::vector<vector> pointsList);
protected:
	std::unique_ptr<Image> buildingImg;
	vector loc;
	std::vector<std::unique_ptr<Fcollision>> cols;
	vector leftPoint, rightPoint;
private:
};