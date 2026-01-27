#pragma once

#include "math.h"
#include "Hoverable.h"

class Shader;
class text;
class animation;
struct Fcollision;

class buyAutoFisher : public IHoverable {
public:
	buyAutoFisher(vector loc);
	~buyAutoFisher();

	void draw(Shader* shaderProgram);

	bool bMouseOver;

	void updateLoc();

	void calcIfPlayerInFront();
	bool inFrontPlayer = false;

private:
	bool calcMaxAutoFishers();
	void spawnAutoFisher();

	std::unique_ptr<animation> plusAnim;
	std::unique_ptr<text> priceText;
	double price;

	double calcPrice();

	// put in some sort of helper function class
	bool hasCurrency();

	std::vector<vector> collisionPoints = std::vector<vector>{ {13, 0}, { 26, 7 }, { 13, 14 }, { 0, 7 } };
	void setupCollision();

	std::unique_ptr<Fcollision> collision;

	uint32_t calcAutoFisherId();
};