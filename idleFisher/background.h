#pragma once

#include "widget.h"
#include "FBO.h"

class URectangle;

struct Padding {
	float top;
	float right;
	float bottom;
	float left;
};

class Background : public widget {
public:
	// path: the path to the folder that contains corner....png and edge....png images
	Background(widget* parent, std::string path, glm::vec4 fillColor);

	void draw(Shader* shaderProgram) override;

	void setLoc(vector loc) override;
	void setSize(vector size) override;
	// returns how thick the borders are
	// Top, Right, Bottom, Left
	Padding GetBorderPadding();

private:
	std::string path;
	Padding borderPadding;

	// Top, Right, Bottom, Left
	std::vector<std::unique_ptr<Image>> edges;
	// Top Left, Top Right, Bottom Right, Bottom Left
	std::vector<std::unique_ptr<Image>> corners;
	std::unique_ptr<URectangle> fillRect;

	std::unique_ptr<FBO> fbo;
};