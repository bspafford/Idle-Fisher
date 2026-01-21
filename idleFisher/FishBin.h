#pragma once

#include "math.h"
#include "Hoverable.h"

class Shader;
class Ubutton;

class FishBin : public IHoverable {
public:
	FishBin(vector loc);
	void sellFish();
	void draw(Shader* shaderProgram);
	
private:
	std::unique_ptr<Ubutton> bin;
};