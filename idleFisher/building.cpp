#include "building.h"
#include "collision.h"
#include "character.h"
#include "main.h"

Abuilding::Abuilding(vector loc) {
	this->loc = loc;
}

Abuilding::~Abuilding() {
	for (int i = 0; i < cols.size(); i++)
		collision::removeCollisionObject(cols[i].get());
}

void Abuilding::draw(Shader* shaderProgram) {
	buildingImg->draw(shaderProgram);
}
 
void Abuilding::setLoc(vector loc) {
	this->loc = loc;
	buildingImg->setLoc(loc);
}

void Abuilding::setupCollision(std::vector<vector> pointsList) {
	cols.push_back(std::make_unique<Fcollision>(pointsList, ""));
	Fcollision* col = cols[cols.size() - 1].get();

	collision::addCollisionObject(col);

	float minIndex = -1, minVal = INFINITY;
	float maxIndex = -1, maxVal = -INFINITY;
	for (int i = 0; i < col->points.size(); i++) {
		if (col->points[i].x < minVal) {
			minVal = col->points[i].x;
			minIndex = i;
		} 
		
		if (col->points[i].x > maxVal) {
			maxVal = col->points[i].x;
			maxIndex = i;
		}
	}

	leftPoint = col->points[minIndex];
	rightPoint = col->points[maxIndex];
}

bool Abuilding::calcInFront() {
	vector charLoc = Acharacter::getCharLoc();
	vector p1 = leftPoint;
	vector p2 = rightPoint;

	float m = (p2.y - p1.y) / (p2.x - p1.x);
	float y = m * (charLoc.x - p1.x) + p1.y;

	return charLoc.y >= y;
}