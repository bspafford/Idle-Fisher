#include "collision.h"

#include <fstream>
#include <string>

#include "main.h"

// read from file, do this at start
void collision::getCollisionObjects(std::vector<SDL_Point*>& allCollision, std::vector<std::string>& collisionIdentifier, std::vector<std::string>& worldName) {

	std::vector<std::vector<vector>> coords;

	std::ifstream colFile("./data/collision.col");

	int lineNum = 0;

	if (colFile.is_open()) {
		while (colFile.good()) {
			std::string line;
			std::getline(colFile, line);

			removeSpaces(line);

			if (line[0] == '!') {
				// then set the world name parameter
				line.erase(line.begin());
				currWorldName = line;
			}
			else if (line[0] != '/' && line != "") {

				std::string identifier = getIdentifier(line);
				collisionIdentifier.push_back(identifier);
				worldName.push_back(currWorldName);

				line.erase(0, identifier.size());

				std::vector<std::string> stringList;
				for (int i = 0; i < line.size(); i++) {
					// if char == "(" then break into a new string
					if (line[i] == '(') {
						std::string temp(1, line[i]);
						stringList.push_back(temp);
					}
					else {
						std::string temp(1, line[i]);
						stringList[stringList.size() - 1].append(temp);
					}
				}

				if ((int)coords.size() - 1 < lineNum) {
					std::vector<vector> temp(stringList.size());
					coords.push_back(temp);
				}

				for (int i = 0; i < stringList.size(); i++) {
					sscanf_s(stringList[i].c_str(), "(%f, %f)", &coords[lineNum][i].x, &coords[lineNum][i].y);
				}

				lineNum++;
			}
		}

		// print test
		for (int i = 0; i < coords.size(); i++) {
			SDL_Point* points = new SDL_Point[coords[i].size()];
			for (int j = 0; j < coords[i].size(); j++) {
				points[j] = { (int)coords[i][j].x, (int)coords[i][j].y };
			}
			allCollision.push_back(points);
		}
	}
}

void collision::saveCollision(std::vector<SDL_Point*>& allCollision, std::vector<std::string>& collisionIdentifier, std::vector<std::string>& worldName) {
	std::ofstream colFile("./data/collision.col");
	std::string currWorldName = worldName[0];
	colFile << "!" << worldName[0] << std::endl;
	for (int i = 0; i < allCollision.size(); i++) {
		if (worldName[i] != "53")
			continue;

			if (worldName[i] != currWorldName)
				colFile << "!" << worldName[i] << std::endl;

			std::string line = collisionIdentifier[i];
			if (line != "")
				line.append(" ");

			for (int j = 0; j < 4; j++) {
				line.append("(" + std::to_string(allCollision[i][j].x) + "," + std::to_string(allCollision[i][j].y) + ") ");
			}
			colFile << line << std::endl;
	}
	std::cout << "collision saved successfully\n";
}

std::string collision::getIdentifier(std::string str) {
	std::string identifier;
	for (int i = 0; i < str.size(); i++) {
		if (str[i] != '(') {
			std::string temp(1, str[i]);
			identifier.append(temp);
		}
		else {
			return identifier;
		}
	}

	return identifier;
}

void collision::removeSpaces(std::string& str) {
	std::string newStr;
	for (int i = 0; i < str.size(); i++) {
		if (str[i] != ' ') {
			std::string temp(1, str[i]);
			newStr.append(temp);
		}
	}

	str = newStr;
}

void collision::showCollisionBoxes(SDL_Renderer* renderer, std::vector<SDL_Point*> allCollision, std::vector<std::string>& collisionIdentifier, std::vector<std::string>& collisionWorld) {
	// shows collision boxes
	for (int i = 0; i < allCollision.size(); i++) {
		SDL_Point* points = new SDL_Point[5];

		for (int j = 0; j < 4; j++)
			points[j] = allCollision[i][j];

		points[4] = allCollision[i][0];

		if (collisionWorld[i] == Main::currWorld) {

			//if (mouseOver == i /*collision::mouseOverRect(i, Main::mousePos, allCollision[i], collisionIdentifier[i], collisionWorld[i])*/) {
				//SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

			//} else {
				if (collisionIdentifier[i] == "")
					SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
				else if (collisionIdentifier[i] == "w") // water
					SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
				else if (collisionIdentifier[i] == "r") // river
					SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
				else if (collisionIdentifier[i] == "s") // stair
					SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
				else if (collisionIdentifier[i] == "g") // grass
					SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
				else if (collisionIdentifier[i] == "d") // dirt
					SDL_SetRenderDrawColor(renderer, 64, 41, 5, 255);
				else if (collisionIdentifier[i] == "o") // wood
					SDL_SetRenderDrawColor(renderer, 234, 208, 168, 255);
				else if (collisionIdentifier[i] == "m") // wood
					SDL_SetRenderDrawColor(renderer, 127, 127, 127, 255);

			// }


			math::pointsToWorld(points);
			SDL_RenderDrawLines(renderer, points, 5);
		}
	}
}

void collision::moveCollisionPoint() {
	if (!Main::leftMouseButtonDown)
		return;

	if (selectedPoint.x == -1) {
		selectedPoint = getClosestPoint();
		if (selectedPoint.x != -1) {
			SDL_Point temp = Main::allCollision[int(selectedPoint.x)][int(selectedPoint.y)];
			startLoc = { float(temp.x), float(temp.y) };
		}
		// std::cout << selectedPoint << std::endl;
	} else if (selectedPoint.x != -1) {
		vector mousePos = math::screenToWorld(Main::mousePos);
		Main::allCollision[int(selectedPoint.x)][int(selectedPoint.y)] = { int(mousePos.x), int(mousePos.y) };
	}
}

// finds the closest point to cursor
// if there are two points touching it will find the rotation of the points and find the closest one to the mouses rotation
vector collision::getClosestPoint() {
	vector mousePos = math::screenToWorld(Main::mousePos);

	// does at index
	multiple.clear();

	vector closest = { -1, -1 };
	float closestDist = INFINITY;
	for (int i = 0; i < Main::allCollision.size(); i++) {
		for (int j = 0; j < 4; j++) {
			SDL_Point* currPoint = &Main::allCollision[i][j];
			float dist = math::distance(mousePos.x, mousePos.y, currPoint->x, currPoint->y);
			float radius = Main::pixelSize;
			if (dist <= radius && dist < closestDist) {
				closestDist = dist;
				closest = { float(i), float(j) };
			}
		}
	}

	if (closest.x == -1)
		return { -1, -1 };

	// prolly bad to do again
	// test if theres another point thats the same distance
	for (int i = 0; i < Main::allCollision.size(); i++) {
		for (int j = 0; j < 4; j++) {
			SDL_Point* currPoint = &Main::allCollision[i][j];
			if (currPoint->x == Main::allCollision[int(closest.x)][int(closest.y)].x && currPoint->y == Main::allCollision[int(closest.x)][int(closest.y)].y) {
				multiple.push_back(vector{ float(i), float(j) });
			}
		}
	}

	if (multiple.size() == 1)
		return closest;
	
	std::vector<float> rots;
	for (int i = 0; i < collision::multiple.size(); i++) {
		float totalX = 0, totalY = 0;
		// std::cout << "multiples: " << collision::multiple[i] << std::endl;
		for (int j = 0; j < 4; j++) {
			totalX += Main::allCollision[collision::multiple[i].x][j].x; // collision::multiple[j].x;
			totalY += Main::allCollision[collision::multiple[i].x][j].y;
		}

		vector avg{ totalX / 4, totalY / 4 };

		SDL_Point temp = Main::allCollision[collision::multiple[i].x][int(collision::multiple[i].y)];
		vector currPoint = { temp.x, temp.y };
		vector normal = math::normalize(avg - currPoint);

		rots.push_back(atan2(normal.y, normal.x) * 180 / M_PI);
	}

	// vector currPoint = { temp.x, temp.y };
	vector mouseWorldPos = math::screenToWorld(Main::mousePos);
	if (multiple.size() > 0) {
		SDL_Point temp = Main::allCollision[collision::multiple[0].x][int(collision::multiple[0].y)];
		vector currPoint = { temp.x, temp.y };
		std::cout << "mouseworldpos: " << mouseWorldPos << std::endl;
		vector normal = math::normalize(mouseWorldPos - currPoint);
		float mouseRot = atan2(normal.y, normal.x) * 180 / M_PI;
		std::cout << mouseRot << std::endl;

		float lowestRotDiff = INFINITY;
		float index = 0;
		for (int i = 0; i < rots.size(); i++) {

			// rotation diff
			float rotationDiff = abs(rots[i] - mouseRot);
			if (rotationDiff < lowestRotDiff) {
				lowestRotDiff = rotationDiff;
				index = i;
			}
		}

		return multiple[index];
	}

	// shouldn't happen
	return { 0, 0 };
}

// undo function
// undo move points
// undo delted rects
// undo added rects
void collision::undo() {
	if (previousActions.size() == 0)
		return;

	std::vector<vector> indexLoc = previousActions[int(previousActions.size()) - 1];
	vector index = indexLoc[0];
	vector loc = indexLoc[1];

	Main::allCollision[index.x][int(index.y)] = { int(loc.x), int(loc.y) };

	previousActions.pop_back();
}

// if mouse is inside the collision and i right click, the delete the collision
// have a button to add collision boxes, prolly at the center of the screen
bool collision::mouseOverWater(vector mousePos, std::vector<SDL_Point*> allCollision, std::vector<std::string>& collisionIdentifier, std::vector<std::string>& collisionWorld) {

	mousePos = math::screenToWorld(mousePos.x, mousePos.y);

	// mousePos
	SDL_Point* mousePoints = new SDL_Point[4];
	mousePoints[3] = { (int)mousePos.x + 1, (int)mousePos.y + 1 };
	mousePoints[2] = { (int)mousePos.x + 1, (int)mousePos.y };
	mousePoints[1] = { (int)mousePos.x, (int)mousePos.y };
	mousePoints[0] = { (int)mousePos.x, (int)mousePos.y + 1 };

	math::pointsToWorld(mousePoints);
	std::vector<vector> a;
	for (int i = 0; i < 4; i++) {
		vector temp = { mousePoints[i].x, mousePoints[i].y };
		a.push_back(temp);
	}

	vector normal;
	float depth;

	std::vector<SDL_Point*> test = allCollision;
	for (int i = 0; i < allCollision.size(); i++) {
		std::vector<vector> b;
		math::pointsToWorld(test[i]);
		for (int j = 0; j < 4; j++) {
			vector temp = { test[i][j].x, test[i][j].y };
			b.push_back(temp);
		}

		if (collisionWorld[i] == Main::currWorld && intersectPolygons(a, b, normal, depth)) {
			mouseOver = i;
			return true;
		}
	}

	mouseOver = -1;
	return false;
}

bool collision::intersectPolygons(std::vector<vector> verticesA, std::vector<vector> verticesB, vector& normal, float& depth) {
	normal = { 0, 0 };
	depth = INFINITY;

	for (int i = 0; i < verticesA.size(); i++) {
		vector va = verticesA[i];
		vector vb = verticesA[(i + 1) % verticesA.size()];

		vector edge = { vb.x - va.x, vb.y - va.y };
		vector axis = { -edge.y, edge.x };
		axis = math::normalize(axis);

		float minA, maxA;
		float minB, maxB;
		projectVertices(verticesA, axis, minA, maxA);
		projectVertices(verticesB, axis, minB, maxB);

		if (minA >= maxB || minB >= maxA) {
			return false;
		}

		float axisDepth = math::min(maxB - minA, maxA - minB);
		if (axisDepth < depth) {
			depth = axisDepth;
			normal = axis;
		}
	}

	for (int i = 0; i < verticesB.size(); i++) {
		vector va = verticesB[i];
		vector vb = verticesB[(i + 1) % verticesB.size()];

		vector edge = { vb.x - va.x, vb.y - va.y };
		vector axis = { -edge.y, edge.x };
		axis = math::normalize(axis);

		float minA, maxA;
		float minB, maxB;
		projectVertices(verticesA, axis, minA, maxA);
		projectVertices(verticesB, axis, minB, maxB);

		if (minA >= maxB || minB >= maxA) {
			return false;
		}

		float axisDepth = math::min(maxB - minA, maxA - minB);
		if (axisDepth < depth) {
			depth = axisDepth;
			normal = axis;
		}
	}

	vector centerA = findArithmeticMean(verticesA);
	vector centerB = findArithmeticMean(verticesB);

	vector direction = { centerB.x - centerA.x, centerB.y - centerA.y };

	if (math::dot(direction, normal) < 0) // its the wrong direction
		normal = { -normal.x, -normal.y };

	return true;
}

void collision::projectVertices(std::vector<vector> vertices, vector axis, float& min, float& max) {
	min = INFINITY;
	max = -INFINITY;

	for (int i = 0; i < vertices.size(); i++) {
		vector v = vertices[i];
		float proj = math::dot(v, axis);

		if (proj < min) { min = proj; }
		if (proj > max) { max = proj; }
	}
}

vector collision::findArithmeticMean(std::vector<vector> vertices) {
	float sumX = 0;
	float sumY = 0;

	for (int i = 0; i < vertices.size(); i++) {
		vector v = vertices[i];
		sumX += v.x;
		sumY += v.y;
	}

	return { sumX / vertices.size(), sumY / vertices.size() };
}

void collision::removeRect() {
	if (mouseOver != -1) {
		Main::allCollision.erase(Main::allCollision.begin() + mouseOver);
		Main::collisionIdentifier.erase(Main::collisionIdentifier.begin() + mouseOver);
		Main::collisionWorld.erase(Main::collisionWorld.begin() + mouseOver);
	}
}

void collision::changeIdentifier() {
	if (mouseOver == -1 || Main::mouseOverButton)
		return;

	// if mouse isn't over a point
	if (selectedPoint.y != -1)
		return;

	// list
	// "" default, "w" water, "r" river, "s" stairs, "g" grass, "d" dirt, "o" wood, "m" metal
	std::vector<std::string> identifiers = { "", "w", "r", "s", "g", "d", "o", "m"};

	// get current index of current identifier
	auto it = std::find(identifiers.begin(), identifiers.end(), Main::collisionIdentifier[mouseOver]);

	int index = -1;
	if (it != identifiers.end())
		index = it - identifiers.begin();

	int next = (index + 1) % identifiers.size();

	// std::cout << "identifier: " << Main::collisionIdentifier[mouseOver] << ", next: " << next << ", index: " << index << std::endl;
	
	Main::collisionIdentifier[mouseOver] = identifiers[next];
}

void collision::addCollisionBox() {
	std::cout << "adding collision box" << std::endl;
	SDL_Point* points = new SDL_Point[4];

	int halfSize = 25;

	/*
	points[0] = { Main::screenWidth / 2 - halfSize, Main::screenHeight / 2 - halfSize };
	points[1] = { Main::screenWidth / 2 + halfSize, Main::screenHeight / 2 - halfSize };
	points[2] = { Main::screenWidth / 2 + halfSize, Main::screenHeight / 2 + halfSize };
	points[3] = { Main::screenWidth / 2 - halfSize, Main::screenHeight / 2 + halfSize };
	*/

	vector p1 = math::screenToWorld({ Main::screenWidth / 2.f - halfSize, Main::screenHeight / 2.f - halfSize });
	vector p2 = math::screenToWorld({ Main::screenWidth / 2.f + halfSize, Main::screenHeight / 2.f - halfSize });
	vector p3 = math::screenToWorld({ Main::screenWidth / 2.f + halfSize, Main::screenHeight / 2.f + halfSize });
	vector p4 = math::screenToWorld({ Main::screenWidth / 2.f - halfSize, Main::screenHeight / 2.f + halfSize });

	points[0] = { (int)p1.x, (int)p1.y };
	points[1] = { (int)p2.x, (int)p2.y };
	points[2] = { (int)p3.x, (int)p3.y };
	points[3] = { (int)p4.x, (int)p4.y };

	Main::allCollision.push_back(points);
	Main::collisionIdentifier.push_back("");
	Main::collisionWorld.push_back("53");
}