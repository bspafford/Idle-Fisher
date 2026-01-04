#include "pathfinding.h"
#include "AStar.h"
#include "node.h"

#include "debugger.h"

pathfinding::pathfinding() {

}

void pathfinding::findPath(vector startPos, vector targetPos, AStar* AStarRef) {
	node* startNode = AStarRef->nodeFromWorldPoint(startPos);
	node* targetNode = AStarRef->nodeFromWorldPoint(targetPos);

	std::vector<node*> openSet;
	//std::unordered_set<node> closedSet;
	std::vector<node*> closedSet;
	openSet.push_back(startNode);

	while (openSet.size() > 0) {
		node* currentNode = openSet[0];
		for (int i = 1; i < openSet.size(); i++) {
			if (openSet[i]->fCost() < currentNode->fCost() || openSet[i]->fCost() == currentNode->fCost() && openSet[i]->hCost < currentNode->hCost) {
				currentNode = openSet[i];
			}
		}

		eraseFromList(openSet, currentNode);
		closedSet.push_back(currentNode);

		if (currentNode == targetNode) { // finished
			std::vector<vector> waypoints = retracePath(startNode, targetNode, AStarRef);
			AStarRef->onPathFound(waypoints, true);
			return;
		}

		for (node* neighbor : AStarRef->getNeighbors(*currentNode)) {
			if (!neighbor->walkable || std::find(closedSet.begin(), closedSet.end(), neighbor) != closedSet.end())
				continue;

			int newMovementCostToNeighbor = currentNode->gCost + getDistance(currentNode, neighbor);
			if (newMovementCostToNeighbor < neighbor->gCost || std::find(openSet.begin(), openSet.end(), neighbor) == openSet.end()) {
				neighbor->gCost = newMovementCostToNeighbor;
				neighbor->hCost = getDistance(neighbor, targetNode);
				neighbor->parent = currentNode;
				if (std::find(openSet.begin(), openSet.end(), neighbor) == openSet.end())
					openSet.push_back(neighbor);
			}
		}
	}
}

std::vector<vector> pathfinding::retracePath(node* startNode, node* endNode, AStar* AStarRef) {
	std::vector<node> path;
	node* currentNode = endNode;

	while (currentNode->loc != startNode->loc) {
		path.push_back(*currentNode);
		currentNode = currentNode->parent;
	}
	std::vector<vector> waypoints = simplifyPath(path);
	std::reverse(waypoints.begin(), waypoints.end());
	AStarRef->nodePath = path;

	return waypoints;
}

std::vector<vector> pathfinding::simplifyPath(std::vector<node> path) {
	std::vector<vector> waypoints;
	vector directionOld = vector{ 0, 0 };

	for (int i = 1; i < path.size(); i++) {
		vector directionNew = vector(path[i - 1].gridX - path[i].gridX, path[i - 1].gridY - path[i].gridY);
		if (directionNew.x != directionOld.x || directionNew.y != directionOld.y) {
			waypoints.push_back(path[i].loc);
		}
		directionOld = directionNew;
	}
	return waypoints;
}

int pathfinding::getDistance(node* a, node* b) {
	return math::distance(a->loc, b->loc);
}

void pathfinding::eraseFromList(std::vector<node*> &list, node* removeNode) {
	for (int i = 0; i < list.size(); i++) {
		if (list[i] == removeNode) {
			list.erase(list.begin() + i);
			return;
		}
	}

}