#include "background.h"
#include "Rectangle.h"
#include "Scene.h"
#include "ScissorTest.h"
#include "GPULoadCollector.h"

Background::Background(widget* parent, std::string path, glm::vec4 fillColor) : widget(parent) {
	assert(GPULoadCollector::isOnMainThread());

	this->path = path;

	// load images
	std::string edgeNames[] = { "T", "R", "B", "L" };
	std::string cornerNames[] = { "TL", "TR", "BR", "BL" };
	for (int i = 0; i < 4; i++) {
		// images/ + "widget/background/background" + TL.png
		edges.push_back(std::make_unique<Image>("images/" + path + edgeNames[i] + ".png", vector{0.f, 0.f}, false));
		corners.push_back(std::make_unique<Image>("images/" + path + cornerNames[i] + ".png", vector{0.f, 0.f}, false));
	}

	fillRect = std::make_unique<URectangle>(this, vector{ 0.f, 0.f }, vector{ 0.f, 0.f }, false, fillColor);

	borderPadding = Padding(edges[0]->getSize().y, edges[1]->getSize().x, edges[2]->getSize().y, edges[3]->getSize().x);
}

void Background::draw(Shader* shaderProgram) {
	fbo->Draw(shaderProgram, absoluteLoc, Rect{ 0.f, 0.f, 1.f, 1.f, }, false, glm::vec4(1));
}

void Background::setLoc(vector loc) {
	widget::setLoc(loc);
}

void Background::setSize(vector size) {
	size = size.ToPixel();

	if (this->size == size)
		return; // already that size

	widget::setSize(size);

	if (!fbo)
		fbo = std::make_unique<FBO>(size, false, FBOType::ColorOnly);
	else
		fbo->ResizeTexture(size);

	fbo->Bind();

	// set locs
	corners[0]->setLoc(vector(0.f, size.y - corners[0]->getSize().y));	// top left
	corners[1]->setLoc( size - corners[1]->getSize());					// top right
	corners[2]->setLoc(vector(size.x - corners[2]->getSize().x, 0.f));	// bottom right
	corners[3]->setLoc({ 0, 0 });											// bottom left

	edges[0]->setLoc(vector(corners[0]->getSize().x, size.y - edges[0]->getSize().y));	// top
	edges[1]->setLoc(vector(size.x - edges[1]->getSize().x, corners[2]->getSize().y));	// right
	edges[2]->setLoc(vector(corners[3]->getSize().x, 0.f));								// bottom
	edges[3]->setLoc(vector(0.f, corners[3]->getSize().y));								// left
	
	Rect topEdge = { edges[0]->getAbsoluteLoc().x, edges[0]->getAbsoluteLoc().y, corners[1]->getAbsoluteLoc().x - (corners[0]->getAbsoluteLoc().x + corners[0]->getSize().x), edges[0]->getSize().y};	// top
	Rect rightEdge= { edges[1]->getAbsoluteLoc().x, edges[1]->getAbsoluteLoc().y, edges[1]->getSize().x, corners[1]->getAbsoluteLoc().y - (corners[2]->getAbsoluteLoc().y + corners[2]->getSize().y) };	// right
	Rect bottomEdge = { edges[2]->getAbsoluteLoc().x, edges[2]->getAbsoluteLoc().y, corners[2]->getAbsoluteLoc().x - (corners[3]->getAbsoluteLoc().x + corners[3]->getSize().x), edges[2]->getSize().y };	// bottom
	Rect leftEdge = { edges[3]->getAbsoluteLoc().x, edges[3]->getAbsoluteLoc().y, edges[3]->getSize().x, corners[0]->getAbsoluteLoc().y - (corners[3]->getAbsoluteLoc().y + corners[3]->getSize().y) };	// left
	Rect rectList[] = { topEdge, rightEdge, bottomEdge, leftEdge };

	vector min = vector{ edges[3]->getSize().x, edges[2]->getSize().y };
	vector max = vector{ edges[1]->getAbsoluteLoc().x, edges[0]->getAbsoluteLoc().y };
	fillRect->setLoc(min);
	fillRect->setSize(max - min);

	// draw to FBO
	fillRect->draw(Scene::twoDShader);
	for (int i = 0; i < 4; i++) {
		corners[i]->draw(Scene::twoDShader);

		// draw multiple of each edge to span across the gap
		// scissor test to stop drawing past bounds
		ScissorTest::Enable(rectList[i]);
		vector num = (vector{ rectList[i].w, rectList[i].h } / edges[i]->getSize()).ceil();
		int drawNum = math::max(num.x, num.y); // loop the greater number of times
		for (int n = 0; n < drawNum; n++) {
			vector axis = n == 0 ? vector{ 0.f, 0.f} : vector{ static_cast<float>((i + 1) % 2), static_cast<float>(i % 2) }; // don't add anything to loc if on first iteration
			edges[i]->setLoc(edges[i]->getLoc() + edges[i]->getSize() * axis);
			edges[i]->draw(Scene::twoDShader);
		}
		ScissorTest::Disable();

		// since an fbo is bound, and a scissor test is disabled, the scissor test resets to the other active scissor test, not to the more recent fbo size
		glScissor(0, 0, size.x * stuff::pixelSize, size.y * stuff::pixelSize);
	}

	fbo->Unbind();
}

Padding Background::GetBorderPadding() {
	return borderPadding;
}