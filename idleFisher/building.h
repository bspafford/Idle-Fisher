#pragma once

#include "math.h"
#include "Image.h"
#include "shaderClass.h"

struct Fcollision;

class Abuilding {
public:
	// left and right are relative (already does: loc + left/right)
	Abuilding(const std::string& path, vector loc, vector left, vector right);
	~Abuilding();
	virtual void draw(Shader* shaderProgram);
	virtual void setLoc(vector loc);
	void DrawDebugLines();

	// gets the furthest left and right point of the collision
	// then makes a line that if the character has a great y at that x location, then the character is behind
	bool calcInFront();
	void setupDepthCheck(vector leftMost, vector rightMost);
protected:
	std::unique_ptr<Image> buildingImg;
	vector loc;
	std::vector<std::unique_ptr<Fcollision>> cols;
	vector leftPoint, rightPoint;
private:
};