#include "Rectangle.h"
#include "stuff.h"
#include "GPULoadCollector.h"
#include "input.h"

#include "debugger.h"

URectangle::URectangle(vector loc, vector size, bool useWorldLoc, glm::vec4 color) {
	this->loc = loc;
	this->size = size;
	this->color = color;
	this->useWorldLoc = useWorldLoc;

	GPULoadCollector::add(this);
}

void URectangle::loadGPU() {
	float vertices[] = {
		// Positions // Texture Coords
		size.x + loc.x, loc.y,           // Bottom-right
		size.x + loc.x, size.y + loc.y,  // Top-right
		loc.x, size.y + loc.y,           // Top-left
		loc.x, loc.y                     // Bottom-left
	};

	std::vector<GLuint> indices = {
		0, 1, 3, // First triangle
		3, 1, 2  // Second triangle
	};

	currVAO = new VAO();
	currVAO->Bind();

	glGenBuffers(1, &VBOId);
	glBindBuffer(GL_ARRAY_BUFFER, VBOId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	currEBO = new EBO(indices);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	setMouseHoverIcon(CURSOR_DEFAULT);

	setLoc(loc);
}

URectangle::~URectangle() {
	if (currVAO)
		currVAO->Delete();
	if (currEBO)
		currEBO->Delete();
	glDeleteBuffers(1, &VBOId);
	delete currVAO;
	delete currEBO;
	currVAO = nullptr;
	currEBO = nullptr;

	GPULoadCollector::remove(this);
}

void URectangle::draw(Shader* shaderProgram) {
	if (!currVAO || !GPULoadCollector::isOnMainThread())
		return;

	shaderProgram->Activate();
	currVAO->Bind();

	shaderProgram->setVec4("color", color);
	glUniform1i(glGetUniformLocation(shaderProgram->ID, "useWorldPos"), useWorldLoc);
	shaderProgram->setInt("isRectangle", 1);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	
	shaderProgram->setInt("isRectangle", 0);

	vector mousePos = Input::getMousePos();
	if (blockCursor && mousePos.x >= loc.x && mousePos.x <= loc.x + size.x && mousePos.y >= loc.y && mousePos.y <= loc.y + size.y)
		setHoveredItem(this);
}

void URectangle::setColor(glm::vec4 color) {
	this->color = color;
}

void URectangle::setLoc(vector loc) {
	this->loc = loc;
	if (useWorldLoc) {
		this->loc = loc;
		this->absoluteLoc = loc;
		updatePositionsList();
	} else {
		// by default anchor should be top left?

		// if left anchor to left side of screen.
		// if left and top anchor then anchor top left
		// if anchor centerX and centerY then anchor center of screen

		// should only work for screen space objects

		// right now the anchor of the image is the bottom left, but when screen anchoring to the right side of the screen, should make it subtract the image width from the image so it isn't off the screen
		//this->absoluteLoc = this->loc + vector{ 1920.f/2.f, 0 };
		vector halfScreen = stuff::screenSize / 2.f;
		vector newLoc;
		if (xAnchor == IMAGE_ANCHOR_LEFT) { // if anchor left
			newLoc.x = loc.x - halfScreen.x;
		} else if (xAnchor == IMAGE_ANCHOR_RIGHT) { // if anchor right
			newLoc.x = loc.x + halfScreen.x - size.x;
		} else if (xAnchor == IMAGE_ANCHOR_CENTER) {
			newLoc.x = loc.x - size.x / 2.f;
		}

		if (yAnchor == IMAGE_ANCHOR_TOP) { // if anchor top
			newLoc.y = loc.y - halfScreen.y + size.y;
		} else if (yAnchor == IMAGE_ANCHOR_BOTTOM) { // if anchor bottom
			newLoc.y = loc.y + halfScreen.y;
		} else if (yAnchor == IMAGE_ANCHOR_CENTER) { // if anchor bottom
			newLoc.y = loc.y + size.y / 2.f;
		}

		absoluteLoc = newLoc;

		updatePositionsList();
	}
}

vector URectangle::getLoc() {
	return loc;
}

void URectangle::setSize(vector size) {
	if (this->size == size)
		return;

	this->size = size;
	updatePositionsList();
}

vector URectangle::getSize() {
	return size;
}

void URectangle::setAnchor(ImageAnchor xAnchor, ImageAnchor yAnchor) {
	if (useWorldLoc) {
		std::cout << "This is a world object, it doesn't work";
		return;
	}

	this->xAnchor = xAnchor;
	this->yAnchor = yAnchor;
	setLoc(loc);
}

void URectangle::updatePositionsList() {
	if (!currVAO || !GPULoadCollector::isOnMainThread())
		return;

	currVAO->Bind();

	glBindBuffer(GL_ARRAY_BUFFER, VBOId);

	vector scaledLoc = absoluteLoc * vector{ 1, -1 }; // left to right is positive, bottom to top is negative, just like sdl2
	if (useWorldLoc)
		scaledLoc = { absoluteLoc.x * stuff::pixelSize, absoluteLoc.y * stuff::pixelSize };
	float positions[] = {
		// Positions						// Texture Coords
		size.x + scaledLoc.x, scaledLoc.y,
		size.x + scaledLoc.x, size.y + scaledLoc.y,
		scaledLoc.x, size.y + scaledLoc.y,
		scaledLoc.x, scaledLoc.y
	};

	// round positions list
	for (int i = 0; i < 8; i++) {
		positions[i] = floorf(positions[i]);
	}

	// updates the tex coords
	void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (ptr) {
		memcpy(ptr, positions, sizeof(positions)); // Copy updated vertex data
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}
}

void URectangle::setBlockCursor(bool val) {
	blockCursor = val;
}