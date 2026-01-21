#include "AStar.h"
#include "main.h"
#include "collision.h"
#include "pathfinding.h"
#include "path.h"
#include "saveData.h"
#include "Rectangle.h"

#include "debugger.h"

#include "debugger.h"

AStar::AStar() {
	stopPathFinding();
}

void AStar::init() {
	gridWorldSize = { 400, 175 };
	nodeSize = 7;

	gridSize.x = round(gridWorldSize.x / nodeSize * stuff::pixelSize);
	gridSize.y = round(gridWorldSize.y / nodeSize * stuff::pixelSize);

	grid.reserve(gridSize.x * gridSize.y);

	gridOffset = { 250, 150 };
	// create grid
	for (int y = 0; y < gridSize.y; y++) {
		for (int x = 0; x < gridSize.x; x++) {
			// do col test
			vector loc = vector{ nodeSize * x, nodeSize * y } + gridOffset;

			std::vector<vector> nodeList = std::vector<vector>(4);
			nodeList[0] = loc;
			nodeList[1] = loc + vector{ nodeSize, 0 };
			nodeList[2] = loc + vector{ nodeSize, nodeSize };
			nodeList[3] = loc + vector{ 0, nodeSize };

			std::unique_ptr<Fcollision> col = std::make_unique<Fcollision>(nodeList, ' ');

			bool walkable = true;
			std::vector<Fcollision*> collisionList = collision::getCollisionList();
			for (int i = 0; i < collisionList.size(); i++) {
				if (collision::isCloseEnough(col.get(), collisionList[i])) {
					vector normal;
					float depth;
					if (collision::intersectPolygons(nodeList, collisionList[i]->GetPoints(), normal, depth)) {
						walkable = false;
					}
				}
			}

			grid.emplace_back(walkable, loc, x, y);
		}
	}
}

void AStar::Deconstructor() {
	grid.clear();
}

void AStar::drawBoard(Shader* shaderProgram) {
	if (path.get())
		path.get()->draw(shaderProgram);

	// draw grid
	for (int y = 0; y < gridSize.y; y++) {
		for (int x = 0; x < gridSize.x; x++) {
			vector loc = grid[getIndex(x, y)].loc;
			if (!grid[getIndex(x, y)].walkable) {
				URectangle rect(nullptr, loc, vector(nodeSize, nodeSize), true, glm::vec4(255, 0, 0, 50));
				rect.draw(shaderProgram);
			}
		}
	}

	URectangle rect(nullptr, targetPos, vector(nodeSize, nodeSize), true, glm::vec4(1, 1, 0, 1));
	rect.draw(shaderProgram);
}

node* AStar::nodeFromWorldPoint(vector worldPos) {
	float percentX = (worldPos.x - gridOffset.x) / gridWorldSize.x / stuff::pixelSize;
	float percentY = (worldPos.y - gridOffset.y) / gridWorldSize.y / stuff::pixelSize;
	percentX = math::clamp(percentX, 0, 1);
	percentY = math::clamp(percentY, 0, 1);

	int x = round((gridSize.x - 1) * percentX);
	int y = round((gridSize.y - 1) * percentY);
	return &grid[getIndex(x, y)];
}

std::vector<node*> AStar::getNeighbors(node& currNode) {
	std::vector<node*> neighbors;

	for (int y = -1; y <= 1; y++) {
		for (int x = -1; x <= 1; x++) {
			if (x == 0 && y == 0)
				continue;

			int checkX = currNode.gridX + x;
			int checkY = currNode.gridY + y;

			if (checkX >= 0 && checkX < gridSize.x && checkY >= 0 && checkY < gridSize.y) {
				neighbors.push_back(&grid[getIndex(checkX, checkY)]);
			}
		}
	}

	return neighbors;
}

void AStar::startPathFinding(vector startLoc, vector targetLoc) {
	stopped = false;
	pathIndex = 0;
	finished = false;

	startPos = startLoc;
	targetPos = targetLoc;
	pathfinding::findPath(startPos, targetPos, this);
}

void AStar::stopPathFinding() {
	pathIndex = 0;
	stopped = true;
}

void AStar::onPathFound(std::vector<vector> waypoints, bool pathSuccessful) {
	if (pathSuccessful) {
		path = std::make_unique<Path>(waypoints, startPos, turnDst);
		followingPath = true;
	}
}

vector AStar::followPath(vector loc, float deltaTime, float speed) {
	if (!followingPath || stopped)
		return { 0, 0 };

	vector pos = loc;
	if (pathIndex != path.get()->finishLineIndex && path.get()->turnBoundaries.size() > 0) {
		while (path.get()->turnBoundaries[pathIndex]->hasCrossedLine(pos)) {
			if (pathIndex == path.get()->finishLineIndex) {
				break;
			} else
				pathIndex++;
		}
		
		if (followingPath) {
			vector diff = math::normalize(path.get()->lookPoints[pathIndex] - pos) * deltaTime;
			return diff;
		}
		// distance from current pos to end,		how far the character moves each frame
	} else if (math::distance(targetPos, pos) >= math::length(math::normalize(targetPos - pos) * deltaTime * speed)) { // if outside of target position range
		// go to end point
		vector diff = math::normalize(targetPos - pos) * deltaTime;
		return diff;
	} else { // if at target position
		followingPath = false;
		finished = true;

		// return diff to put character at target pos
		return (targetPos - pos) / speed;
	}
}

int AStar::getIndex(int x, int y) {
	return y * gridSize.x + x;
}