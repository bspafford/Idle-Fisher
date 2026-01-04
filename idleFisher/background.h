#pragma once

#include "widget.h"

class URectangle;
class FBO;

class Background : public widget {
public:
	// path: the path to the folder that contains corner....png and edge....png images
	Background(widget* parent, std::string path, glm::vec4 fillColor);

	void draw(Shader* shaderProgram) override;

	void setLoc(vector loc) override;
	void setSize(vector size) override;

private:
	std::string path;

	// Top, Right, Bottom, Left
	std::vector<std::unique_ptr<Image>> edges;
	// Top Left, Top Right, Bottom Right, Bottom Left
	std::vector<std::unique_ptr<Image>> corners;
	std::unique_ptr<URectangle> fillRect;

	std::unique_ptr<FBO> fbo;
};