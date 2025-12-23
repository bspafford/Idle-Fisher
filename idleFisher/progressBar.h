#pragma once

#include "widget.h"
#include "Rectangle.h"

struct FprogressBarDir {
	static inline int left = 0;
	static inline int right = 1;
	static inline int up = 2;
	static inline int down = 3;
};

class UprogressBar : public widget {
public:
	UprogressBar(widget* parent, vector size, bool useCharLoc, int dir = FprogressBarDir::right, bool reverse = false);
	// background color, foreground color, direction (left, up, reverse), width, height
	void setBackgroundColor(glm::vec4 color);
	// id is when the progress bar has mutliple preogress bars
	void setForegroundColor(glm::vec4 color, int id = 0);
	void draw(Shader* shaderProgram);
	// id is when the progress bar has mutliple preogress bars
	void setPercent(float percent, int id = 0);
	// if this is the first time adding, make sure to update the weight of the first progress bar
	void addProgressBar(float percent, float weight, glm::vec4 color);

	float calcTotalWeight();

	virtual void setLoc(vector loc) override;
	virtual void setSize(vector size) override;

private:
	std::vector<float> percent = { 0 };
	std::vector<float> weight = { 1 };
	bool useWorldLoc;
	int dir;
	bool reverse;
	
	std::vector<std::unique_ptr<URectangle>> foregrounds;
	std::unique_ptr<URectangle> background;
};
