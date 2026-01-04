#include "path.h"
#include "shaderClass.h"
#include "Rectangle.h"

#include "debugger.h"

Path::Path(std::vector<vector> waypoints, vector startPos, float turnDst) {
	lookPoints = waypoints;
	turnBoundaries = std::vector<Line*>(lookPoints.size());
	finishLineIndex = turnBoundaries.size() - 1;

	vector previousPoint = startPos;
	for (int i = 0; i < lookPoints.size(); i++) {
		vector currentPoint = lookPoints[i];
		vector dirToCurrentPoint = math::normalize(currentPoint - previousPoint);
		vector turnBoundaryPoint = (i == finishLineIndex) ? currentPoint : currentPoint - dirToCurrentPoint * turnDst;
		turnBoundaries[i] = new Line(turnBoundaryPoint, previousPoint - dirToCurrentPoint * turnDst);
		previousPoint = turnBoundaryPoint;
	}
}

Path::~Path() {
	for (int i = 0; i < turnBoundaries.size(); i++) {
		delete turnBoundaries[i];
		turnBoundaries[i] = nullptr;
	}
	turnBoundaries.clear();
}

void Path::draw(Shader* shaderProgram) {
	for (vector p : lookPoints) {
		vector loc = math::worldToScreen(p);
		URectangle rect(nullptr, loc, { 4, 4 }, true);
		rect.draw(shaderProgram);
	}
}