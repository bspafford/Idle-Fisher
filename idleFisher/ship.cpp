#include "ship.h"
#include "timer.h"
#include "Image.h"

Aship::Aship(vector loc) {
	this->loc = loc;
	tempLoc = loc;

	shipImg = std::make_unique<Image>("./images/ship1.png", loc, true);
	bobTimer = CreateDeferred<Timer>();
	bobTimer->addUpdateCallback(this, &Aship::shipbob);
	bobTimer->start(5.f);
}

Aship::~Aship() {

}

void Aship::draw(Shader* shaderProgram) {
	shipImg->draw(shaderProgram);
}

void Aship::shipbob() {
	vector bobberY(0.f, roundf(sin(bobTimer->getPercent() * 2.f * M_PI) * 2.f));

	loc = tempLoc + bobberY;
	shipImg->setLoc(loc);

	if (bobTimer->IsFinished()) {
		tempLoc = loc;
		bobTimer->start(5.f);
	}
}