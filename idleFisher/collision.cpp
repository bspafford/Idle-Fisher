#include "collision.h"

#include <string>
#include <fstream>

#include "main.h"
#include "Input.h"
#include "saveData.h"
#include "character.h"
#include "shaderClass.h"
#include "camera.h"
#include "Cursor.h"

#include "debugger.h"

collision::collision() {

}

Fcollision::Fcollision(std::vector<vector> worldPoints, std::string identifier) {
	isCircle = false;
	this->points = worldPoints;
	this->identifier = identifier;

	minX = INFINITY, maxX = -INFINITY, minY = INFINITY, maxY = -INFINITY;

	// world loc
	for (int i = 0; i < worldPoints.size(); i++) {
		if (points[i].x < minX)
			minX = points[i].x;
		else if (points[i].x > maxX)
			maxX = points[i].x;

		if (points[i].y < minY)
			minY = points[i].y;
		else if (points[i].y > maxY)
			maxY = points[i].y;
	}
}

Fcollision::Fcollision(vector center, float radius, std::string identifier) {
	isCircle = true;
	this->radius = radius;
	this->identifier = identifier;

	points.push_back(center);

	minX = center.x - radius;
	maxX = center.x + radius;
	minY = center.y - radius;
	maxY = center.y + radius;
}

void collision::getCollisionObjects() {
	std::lock_guard<std::mutex> lock(mutex);

	// clears all collision
	collisionStorage.clear();
	allCollision.clear();

	std::vector<std::vector<vector>> coords;

	std::ifstream colFile("./data/collision.col");

	int lineNum = 0;
	std::string currWorldName = "";
	if (colFile.is_open()) {
		while (colFile.good()) {
			std::string line;
			std::getline(colFile, line);

			removeSpaces(line);

			if (line[0] == '!') {
				if (currWorldName != "") // returns if passed the world
					return;

				// then set the world name parameter
				line.erase(line.begin());
				if (line == Scene::getCurrWorldName())
					currWorldName = line;

				// only puts collision in the list if its part of that world
			} else if (line[0] != '/' && line != "" && currWorldName != "") {

				std::string identifier = getIdentifier(line);

				line.erase(0, identifier.size());

				std::vector<std::string> stringList;
				for (int i = 0; i < line.size(); i++) {
					// if char == "(" then break into a string
					if (line[i] == '(') {
						std::string temp(1, line[i]);
						stringList.push_back(temp);
					} else {
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

				vector multiplier = vector{ 1, -1 };
				vector offset = { 500, 500 };// { 1500, 1500 };
				for (int i = 0; i < coords[lineNum].size(); i++)
					coords[lineNum][i] = coords[lineNum][i] * multiplier + offset;

				std::unique_ptr<Fcollision>& col = collisionStorage.emplace_back(std::make_unique<Fcollision>(coords[lineNum], identifier));
				allCollision.push_back(col.get());

				lineNum++;
			}
		}
	}
}

void collision::removeCollisionObject(Fcollision* collision) {
	std::lock_guard<std::mutex> lock(mutex);

	if (!Main::IsRunning())
		return;

	auto it = std::find(allCollision.begin(), allCollision.end(), collision);
	if (it != allCollision.end()) {
		allCollision.erase(it);
	}

	// remove it from collisionStorage too
	collisionStorage.erase(
		std::remove_if(collisionStorage.begin(), collisionStorage.end(),
			[collision](auto& col) { return col.get() == collision; }
		),
		collisionStorage.end()
	);
}

bool collision::mouseOverWater(vector mousePos, std::vector<Fcollision*> allCollision) {
	std::lock_guard<std::mutex> lock(mutex);

	mousePos = math::screenToWorld(mousePos);

	// mousePos
	std::unique_ptr<Fcollision> aCol = std::make_unique<Fcollision>(mousePos, 1, "");

	vector normal;
	float depth;
	for (int i = 0; i < allCollision.size(); i++) {
		if (allCollision[i]->identifier == "w" && isCloseEnough(aCol.get(), allCollision[i]) && intersectCirclePolygon(mousePos, 1, allCollision[i]->points, normal, depth))
			return true;
	}

	return false;
}

bool collision::intersectCirclePolygon(vector circleCenter, float circleRadius, std::vector<vector> vertices, vector& normal, float& depth) {
	normal = { 0, 0 };
	depth = INFINITY;

	for (int i = 0; i < vertices.size(); i++) {
		vector va = vertices[i];
		vector vb = vertices[(i + 1) % vertices.size()];

		vector edge = va - vb;
		vector axis = { -edge.y, edge.x };
		axis = math::normalize(axis);

		float minA, maxA;
		float minB, maxB;
		projectVertices(vertices, axis, minA, maxA);
		projectCircle(circleCenter, circleRadius, axis, minB, maxB);

		if (minA >= maxB || minB >= maxA) {
			return false;
		}

		float axisDepth = math::min(maxB - minA, maxA - minB);
		if (axisDepth < depth) {
			depth = axisDepth;
			normal = axis;
		}
	}

	int cpIndex = findClosestPointOnPolygon(circleCenter, vertices);
	vector cp = vertices[cpIndex];

	vector axis = cp - circleCenter;
	axis = math::normalize(axis);

	float minA, maxA;
	float minB, maxB;
	projectVertices(vertices, axis, minA, maxA);
	projectCircle(circleCenter, circleRadius, axis, minB, maxB);

	if (minA >= maxB || minB >= maxA) {
		return false;
	}

	float axisDepth = math::min(maxB - minA, maxA - minB);
	if (axisDepth < depth) {
		depth = axisDepth;
		normal = axis;
	}

	vector polygonCenter = findArithmeticMean(vertices);

	vector direction = polygonCenter - circleCenter;

	if (math::dot(direction, normal) < 0) // its the wrong direction
		normal = { -normal.x, -normal.y };

	return true;
}

int collision::findClosestPointOnPolygon(vector circleCenter, std::vector<vector> vertices) {
	int result = -1;
	float minDistance = INFINITY;

	for (int i = 0; i < vertices.size(); i++) {
		vector v = vertices[i];
		float distance = math::distance(v, circleCenter);

		if (distance < minDistance) {
			minDistance = distance;
			result = i;
		}
	}

	return result;
}

void collision::projectCircle(vector center, float radius, vector axis, float& min, float& max) {
	vector direction = math::normalize(axis);
	vector directionAndRadius = direction * radius;

	vector p1 = center + directionAndRadius;
	vector p2 = center - directionAndRadius;

	min = math::dot(p1, axis);
	max = math::dot(p2, axis);

	if (min > max) {
		// swaps the min and max values
		float t = min;
		min = max;
		max = t;
	}
}

bool collision::intersectPolygons(std::vector<vector> verticesA, std::vector<vector> verticesB, vector& normal, float& depth) {
	normal = { 0, 0 };
	depth = INFINITY;

	for (int i = 0; i < verticesA.size(); i++) {
		vector va = verticesA[i];
		vector vb = verticesA[(i + 1) % verticesA.size()];

		vector edge = va - vb;
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

		vector edge = vb - va;
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

	vector direction = centerB - centerA;

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

	return { sumX / (float)vertices.size(), sumY / (float)vertices.size() };
}

bool collision::intersectCircles(vector centerA, float radiusA, vector centerB, float radiusB, vector& normal, float& depth) {
	normal = { 0, 0 };
	depth = 0;

	float distance = math::distance(centerA, centerB);
	float radii = radiusA + radiusB;

	if (distance >= radii)
		return false;

	normal = math::normalize(centerB - centerA);
	depth = radii - distance;

	return true;
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

std::string collision::getIdentifier(std::string str) {
	std::string identifier;
	for (int i = 0; i < str.size(); i++) {
		if (str[i] != '(') {
			std::string temp(1, str[i]);
			identifier.append(temp);
		} else {
			return identifier;
		}
	}

	return identifier;
}

void collision::showCollisionBoxes(Shader* shaderProgram) {
	std::lock_guard<std::mutex> lock(mutex);

	shaderProgram->setInt("useWorldPos", true);
	shaderProgram->setInt("isRectangle", true);

	for (int i = 0; i < allCollision.size(); i++) {
		if (!allCollision[i]->isCircle) {
			for (int j = 0; j < allCollision[i]->points.size(); j++) {
				int point1 = j;
				int point2 = (j + 1) % allCollision[i]->points.size();
				vector temp1 = allCollision[i]->points[point1] * stuff::pixelSize;
				vector temp2 = allCollision[i]->points[point2] * stuff::pixelSize;

				float lineVertices[] = {
					temp1.x, temp1.y,  // Start point
					temp2.x, temp2.y   // End point
				};

				unsigned int VAO, VBO;
				glCreateVertexArrays(1, &VAO);
				glCreateBuffers(1, &VBO);

				glBindVertexArray(VAO);
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);

				glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
				glEnableVertexAttribArray(0);

				// Draw line
				if (allCollision[i]->identifier == "w") // if water make blue
					shaderProgram->setVec4("color", glm::vec4(0.f, 0.f, 1.f, 1.f));
				else // if else make blue
					shaderProgram->setVec4("color", glm::vec4(glm::vec3(0.f), 1.f));

				glBindVertexArray(VAO);
				glLineWidth(2.5f);
				glDrawArrays(GL_LINES, 0, 2);
				glBindVertexArray(0);

				glDeleteVertexArrays(1, &VAO);
				glDeleteBuffers(1, &VBO);
			}
		} else { // show circle collisions
			int circlePointsNum = 12;

			vector center = allCollision[i]->points[0] * stuff::pixelSize;
			float radius = allCollision[i]->radius * stuff::pixelSize;

			// calculate all points on circle
			std::vector<vector> points;
			for (int i = 0; i < circlePointsNum; i++) {
				float angle = (2 * M_PI / circlePointsNum) * i;
				vector point;
				point.x = center.x + radius * cos(angle);
				point.y = center.y + radius * sin(angle);
				points.push_back(point);
			}

			// draw lines
			for (int i = 0; i < circlePointsNum; i++) {
				vector point1 = points[i];
				vector point2 = points[(i + 1) % circlePointsNum];

				float lineVertices[] = {
					point1.x, point1.y,  // Start point
					point2.x, point2.y   // End point
				};

				unsigned int VAO, VBO;
				glCreateVertexArrays(1, &VAO);
				glCreateBuffers(1, &VBO);

				glBindVertexArray(VAO);
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);

				glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
				glEnableVertexAttribArray(0);

				// Draw line
				glBindVertexArray(VAO);
				glLineWidth(2.5f);
				glDrawArrays(GL_LINES, 0, 2);
				glBindVertexArray(0);

				glDeleteVertexArrays(1, &VAO);
				glDeleteBuffers(1, &VBO);
			}
		}
	}

	// draw player collision
	std::vector<std::vector<float>> stuff;
	float radius = Acharacter::col->radius;
	vector point = Acharacter::col->points[0] * stuff::pixelSize;
	stuff.push_back({ point.x + radius, point.y + radius, point.x + radius, point.y - radius });
	stuff.push_back({ point.x + radius, point.y - radius, point.x - radius, point.y - radius });
	stuff.push_back({ point.x - radius, point.y - radius, point.x - radius, point.y + radius });
	stuff.push_back({ point.x - radius, point.y + radius, point.x + radius, point.y + radius });
	for (int i = 0; i < stuff.size(); i++) {
		unsigned int VAO, VBO;
		glCreateVertexArrays(1, &VAO);
		glCreateBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(stuff[i]), stuff[i].data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// Draw line
		glBindVertexArray(VAO);
		glLineWidth(2.5f);
		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);

		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}


	// draw mouse collision
	std::vector<std::vector<float>> mousePosList;
	float mouseRadius = .5;
	vector mousePos = math::screenToWorld(Input::getMousePos());
	mousePosList.push_back({ mousePos.x + mouseRadius, mousePos.y + mouseRadius, mousePos.x + mouseRadius, mousePos.y - mouseRadius });
	mousePosList.push_back({ mousePos.x + mouseRadius, mousePos.y - mouseRadius, mousePos.x - mouseRadius, mousePos.y - mouseRadius });
	mousePosList.push_back({ mousePos.x - mouseRadius, mousePos.y - mouseRadius, mousePos.x - mouseRadius, mousePos.y + mouseRadius });
	mousePosList.push_back({ mousePos.x - mouseRadius, mousePos.y + mouseRadius, mousePos.x + mouseRadius, mousePos.y + mouseRadius });
	for (int i = 0; i < mousePosList.size(); i++) {
		unsigned int VAO, VBO;
		glCreateVertexArrays(1, &VAO);
		glCreateBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(mousePosList[i]), mousePosList[i].data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// Draw line
		glBindVertexArray(VAO);
		glLineWidth(2.5f);
		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);

		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}

	shaderProgram->setInt("isRectangle", false);
	shaderProgram->setVec4("color", glm::vec4(1.f));

}



void collision::testCollisions(Fcollision* playerCol, std::vector<Fcollision*> allCollision) {
	std::lock_guard<std::mutex> lock(mutex);

	temp.clear();

	// mousePos
	vector mousePos = math::screenToWorld(Input::getMousePos());
	std::unique_ptr<Fcollision> aCol = std::make_unique<Fcollision>(mousePos, 1, "");
	Cursor::setMouseOverWater(false);

	for (int i = 0; i < allCollision.size(); i++) {
		// player col shouldn't exist any more, can replace it by converting the cirlce to a square
		vector normal = { 0, 0 };
		float depth = 0;
		if (GetCharacter()->getCanMove() && (GetCharacter()->moveDir.x != 0 || GetCharacter()->moveDir.y != 0)) {
			//std::cout << allCollision[i]->points[0] << " | " << allCollision[i]->points[1] << " | " << allCollision[i]->points[2] << " | " << allCollision[i]->points[3] << " | " << std::endl;
			//std::cout << "iscloseenough: " << isCloseEnough(playerCol, allCollision[i]) << ", i: " << i << std::endl;
			if (isCloseEnough(playerCol, allCollision[i])) {
				if ((allCollision[i]->isCircle && intersectCircles(playerCol->points[0], playerCol->radius, allCollision[i]->points[0], allCollision[i]->radius, normal, depth)) || intersectCirclePolygon(playerCol->points[0], playerCol->radius, allCollision[i]->points, normal, depth)) {
					// pushes back player from collision
					SaveData::saveData.playerLoc.x += -normal.x * depth;
					SaveData::saveData.playerLoc.y += -normal.y * depth;
				}
			}
		}

		if (!Cursor::getMouseOverWater() && allCollision[i]->identifier == "w" && isCloseEnough(aCol.get(), allCollision[i]) && intersectCirclePolygon(mousePos, 1, allCollision[i]->points, normal, depth))
			Cursor::setMouseOverWater(true);
	}
}

// tests if player is colliding and pushes them back
void collision::testPlayerCollision(Fcollision* playerCol, std::vector<Fcollision*> allCollision) {
	std::lock_guard<std::mutex> lock(mutex);

	temp.clear();
	if (!GetCharacter()->getCanMove())
		return;

	for (int i = 0; i < allCollision.size(); i++) {
		// player col shouldn't exist any more, can replace it by converting the cirlce to a square
		if (isCloseEnough(playerCol, allCollision[i])) {
			vector normal;
			float depth;
			if ((allCollision[i]->isCircle && intersectCircles(playerCol->points[0], playerCol->radius, allCollision[i]->points[0], allCollision[i]->radius, normal, depth)) || intersectCirclePolygon(playerCol->points[0], playerCol->radius, allCollision[i]->points, normal, depth)) {
				// pushes back player from collision
				SaveData::saveData.playerLoc.x += -normal.x * depth;
				SaveData::saveData.playerLoc.y += -normal.y * depth;
			}
		}
	}
}

// test if the player is close enough for the collision to even be tested
// this doesn't reallistically needed to be calculated every frame, could do this once at the beginning and save it in another collision list, it would contain all the min and maxs, should make a collision struct
bool collision::isCloseEnough(const Fcollision* a, const Fcollision* col) {
	if (a->minX < col->maxX && a->maxX > col->minX && a->minY < col->maxY && a->maxY > col->minY) {
		//temp.push_back(col->points);
		return true;
	}
	return false;
}

// Circle vs Polygon CCD
bool collision::sweepPointVsEdge(vector p0, vector v, vector edgeStart, vector edgeEnd, float radius, float* toiOut, vector* normalOut) {
	vector a = edgeStart;
	vector b = edgeEnd;
	vector edgeDir = b - a;
	vector edgeNormal = math::perp(math::normalize(edgeDir)); // outward normal

	// flip normal if not facing character, so it doesn't matter which edge vertex comes first
	if (math::dot(v, edgeNormal) >= 0)
		edgeNormal = -edgeNormal;

	// Project point's velocity onto normal of edge
	float denom = math::dot(v, edgeNormal);
	if (denom >= 0) return false; // Moving away or parallel


	// Compute distance from point to edge
	float distance = math::dot(p0 - a, edgeNormal) - radius; // find the closest distance, not the distance directly to the point
	float toi = -distance / denom;

	// Check that the projected hit point is within the edge segment
	vector hitPoint = p0 + v * toi;
	float tOnEdge = math::dot(hitPoint - a, edgeDir) / math::dot(edgeDir, edgeDir);
	if (tOnEdge < 0 || tOnEdge > 1) return false;

	toi /= stuff::pixelSize;

	*toiOut = toi;
	*normalOut = edgeNormal;

	return toi >= 0 && toi <= 1;
}

bool collision::sweepPointVsCircle(vector p0, vector v, vector center, float radius, float* toiOut, vector* normalOut) {
	vector m = p0 - center;
	float r = radius;

	float a = math::dot(v, v);
	float b = 2.0f * math::dot(m, v);
	float c = math::dot(m, m) - r * r;

	// Solve quadratic: a*t^2 + b*t + c = 0
	float discriminant = b * b - 4 * a * c;
	if (discriminant < 0) return false; // No collision

	float sqrtDisc = sqrt(discriminant);
	float t0 = (-b - sqrtDisc) / (2 * a);
	float t1 = (-b + sqrtDisc) / (2 * a);
	t0 /= stuff::pixelSize;
	t1 /= stuff::pixelSize;

	// We want the first positive root
	if (t1 < 0) return false;

	float toi = (t0 >= 0) ? t0 : t1;

	// Output
	vector hitPoint = p0 + v * toi;
	*toiOut = toi;
	*normalOut = math::normalize(hitPoint - center);

	return toi <= 1.0f;
}

bool collision::testCCD(Fcollision* playerCol, vector move, float deltaTime) {
	std::lock_guard<std::mutex> lock(mutex);

	temp.clear();

	const int maxIterations = 2;

	float timeRemaining = 1.0f;
	vector v = math::normalize(move) * GetCharacter()->speed * deltaTime;

	for (int iteration = 0; iteration < maxIterations && timeRemaining > 0.0f; ++iteration) {
		float minTOI = timeRemaining;
		vector hitNormal;
		bool hit = false;

		for (int i = 0; i < allCollision.size(); i++) {
			if (allCollision[i]->isCircle) {
				float toi;
				vector normal;
				if (circleVsCircle(playerCol, v, allCollision[i], &toi, &normal)) {
					if (toi < minTOI) {
						minTOI = toi;
						hitNormal = normal;
						hit = true;
					}
				}
			} else {
				for (int j = 0; j < allCollision[i]->points.size(); j++) {
					float toi;
					vector normal;

					float pointsSize = static_cast<float>(allCollision[i]->points.size());
					vector edgeEnd = { allCollision[i]->points[j].x, allCollision[i]->points[j].y };
					vector edgeStart = { allCollision[i]->points[(j + 1) % int(pointsSize)].x, allCollision[i]->points[(j + 1) % int(pointsSize)].y };

					// treat edge as line segment inflated by radius
					if (sweepPointVsEdge(Acharacter::getCharLoc(), v, edgeStart, edgeEnd, playerCol->radius, &toi, &normal)) {
						if (toi < minTOI) {
							minTOI = toi;
							hitNormal = normal;
							hit = true;
						}
					}
				}

				for (int j = 0; j < allCollision[i]->points.size(); j++) {
					float toi;
					vector normal;

					vector vertex = { allCollision[i]->points[j].x, allCollision[i]->points[j].y };

					// treat vertex as a circle of radius R
					if (sweepPointVsCircle(Acharacter::getCharLoc(), v, vertex, playerCol->radius, &toi, &normal)) {
						if (toi < minTOI) {
							minTOI = toi;
							hitNormal = normal;
							hit = true;
						}
					}
				}
			}
		}

		if (hit) {
			float safeTOI = std::min(minTOI, timeRemaining);
			safeTOI -= 0.01f;

			SaveData::saveData.playerLoc.x += v.x * safeTOI;
			SaveData::saveData.playerLoc.y += v.y * safeTOI;

			timeRemaining -= safeTOI;

			// Slide: remove velocity into the surface
			float vn = math::dot(v, hitNormal);
			v = v - hitNormal * vn; // direction along the wall
		} else {
			SaveData::saveData.playerLoc.x += v.x * timeRemaining;
			SaveData::saveData.playerLoc.y += v.y * timeRemaining;
			break;
		}
	}

	return true;
}

bool collision::circleVsCircle(Fcollision* playerCol, vector v, Fcollision* circleCol, float* toiOut, vector* normalOut) {
	vector p0 = playerCol->points[0];
	vector center = circleCol->points[0];
	float radius = circleCol->radius + playerCol->radius;


	vector m = p0 - center;
	float r = radius;

	float a = math::dot(v, v);
	float b = 2.0f * math::dot(m, v);
	float c = math::dot(m, m) - r * r;

	// Solve quadratic: a*t^2 + b*t + c = 0
	float discriminant = b * b - 4 * a * c;
	if (discriminant < 0) return false; // No collision

	float sqrtDisc = sqrt(discriminant);
	float t0 = (-b - sqrtDisc) / (2 * a);
	float t1 = (-b + sqrtDisc) / (2 * a);
	t0 /= stuff::pixelSize;
	t1 /= stuff::pixelSize;

	// We want the first positive root
	if (t1 < 0) return false;

	float toi = (t0 >= 0) ? t0 : t1;

	// Output
	vector hitPoint = p0 + v * toi;
	*toiOut = toi;
	*normalOut = math::normalize(hitPoint - center);

	return toi <= 1.0f;
}

bool collision::testMouse(vector mousePos) {
	std::lock_guard<std::mutex> lock(mutex);

	vector worldPos = math::screenToWorld(mousePos);
	Cursor::setMouseOverWater(false);

	for (int i = 0; i < allCollision.size(); i++) {
		vector normal;
		if (!Cursor::getMouseOverWater() && allCollision[i]->identifier == "w") {
			if (pointInQuad(worldPos, allCollision[i])) {
				Cursor::setMouseOverWater(true);
				return true;
			}
		}
	}
	return false;
}

bool collision::pointInQuad(vector mousePos, Fcollision* col) {
	mousePos /= stuff::pixelSize;
	vector a = col->points[0];
	vector b = col->points[1];
	vector c = col->points[2];
	vector d = col->points[3];
	return pointInTriangle(mousePos, a, b, c) || pointInTriangle(mousePos, a, c, d);
}

bool collision::pointInTriangle(const vector& pt, const vector& a, const vector& b, const vector& c) {
	auto sign = [](const vector& p1, const vector& p2, const vector& p3) {
		return (p1.x - p3.x) * (p2.y - p3.y) -
			   (p2.x - p3.x) * (p1.y - p3.y);
		};

	float d1 = sign(pt, a, b);
	float d2 = sign(pt, b, c);
	float d3 = sign(pt, c, a);

	bool has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
	bool has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

	return !(has_neg && has_pos);
}

std::vector<Fcollision*>& collision::getCollisionList() {
	std::lock_guard<std::mutex> lock(mutex);
	return allCollision;
}

void collision::addCollisionObject(Fcollision* col) {
	std::lock_guard<std::mutex> lock(mutex);
	allCollision.push_back(col);
}

void collision::replaceCollisionObject(Fcollision* oldCol, Fcollision* newCol) {
	std::lock_guard<std::mutex> lock(mutex);

	auto it = std::find(allCollision.begin(), allCollision.end(), oldCol);
	if (it != allCollision.end()) {
		int index = static_cast<int>(it - allCollision.begin());
		allCollision[index] = newCol;
	}

	// also remove from collision storage
	collisionStorage.erase(
		std::remove_if(collisionStorage.begin(), collisionStorage.end(),
			[oldCol](auto& col) { return col.get() == oldCol; }
		),
		collisionStorage.end()
	);
}