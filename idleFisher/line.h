#pragma once

#include "math.h"

class Shader;

struct Line {
	const float verticalLineGradient = 1e1f; // 1e5f

	float gradient;
	float y_intercept;
	vector pointOnLine_1;
	vector pointOnLine_2;
	
	float gradientPerpendicular;

	bool approachSize;

	Line(vector pointOnLine, vector pointPerpendicularToLine) {
		float dx = pointOnLine.x - pointPerpendicularToLine.x;
		float dy = pointOnLine.y - pointPerpendicularToLine.y;

		if (dx == 0)
			gradientPerpendicular = verticalLineGradient;
		else
			gradientPerpendicular = dy / dx;

		if (gradientPerpendicular == 0)
			gradient = verticalLineGradient;
		else
			gradient = -1 / gradientPerpendicular;

		y_intercept = pointOnLine.y - gradient * pointOnLine.x;
		pointOnLine_1 = pointOnLine;
		pointOnLine_2 = pointOnLine + vector{ 1, gradient };

		approachSize = getSide(pointPerpendicularToLine);
	}

	bool getSide(vector p) {
		return (p.x - pointOnLine_1.x) * (pointOnLine_2.y - pointOnLine_1.y) > (p.y - pointOnLine_1.y) * (pointOnLine_2.x - pointOnLine_1.x);
	}

	bool hasCrossedLine(vector p) {
		return getSide(p) != approachSize;
	}

	void draw(Shader* shaderProgram) {

	}
};