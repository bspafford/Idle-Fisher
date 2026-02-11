#pragma once

#include "math.h"
#include "Hoverable.h"

class Shader;
class Ubutton;
class Image;

class FishBin : public IHoverable {
public:
	FishBin(vector loc);
	void sellFish();
	void draw(Shader* shaderProgram);
	bool calcIfPlayerInfront();
private:
	std::unique_ptr<Ubutton> bin;
	std::unique_ptr<Image> binOutline;
};