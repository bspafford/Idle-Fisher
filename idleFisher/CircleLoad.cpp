#include "CircleLoad.h"
#include "stuff.h"
#include "main.h"
#include "timer.h"

CircleLoad::CircleLoad() : URectangle({ 0, 0 }, stuff::screenSize, false, glm::vec4(18.f / 255.f, 11.f / 255.f, 22.f / 255.f, 1.f)), widget(NULL) {
	circleTimer = std::make_unique<timer>();
	circleTimer->addCallback(this, &CircleLoad::NextCircle);
}

void CircleLoad::Start() {
	animating = true;
	circleIndex = 0;
	circleTimer->start(.1);
}

void CircleLoad::draw() {
	if (!animating)
		return;

	Main::circleShader->Activate();
	Main::circleShader->setMat4("projection", Camera::getProjectionMat());
	Main::circleShader->setVec2("screenSize", glm::vec2(stuff::screenSize.x, stuff::screenSize.y));
	Main::circleShader->setFloat("percent", circleRadius[circleIndex]);
	URectangle::draw(Main::circleShader);
}

void CircleLoad::NextCircle() {
	circleIndex++;
	
	if (circleIndex >= sizeof(circleRadius) / sizeof(float)) {
		animating = false;
		return;
	}

	circleTimer->start(.1);
}

void CircleLoad::setupLocs() {
	URectangle::setLoc({ 0, 0 });
	URectangle::setSize(stuff::screenSize);
}