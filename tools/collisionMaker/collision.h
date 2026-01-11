#pragma once

#include "math.h"

class collision {
public:
	static void getCollisionObjects(std::vector<SDL_Point*>& allCollision, std::vector<std::string>& collisionIdentifier, std::vector<std::string>& worldName);
	static void saveCollision(std::vector<SDL_Point*>& allCollision, std::vector<std::string>& collisionIdentifier, std::vector<std::string>& worldName);
	static void showCollisionBoxes(SDL_Renderer* renderer, std::vector<SDL_Point*> allCollision, std::vector<std::string>& collisionIdentifier, std::vector<std::string>& collisionWorld);
	static std::string getIdentifier(std::string str);
	static void removeSpaces(std::string& str);
	static void moveCollisionPoint();
	static vector getClosestPoint();
	static void undo();

	static bool mouseOverWater(vector mousePos, std::vector<SDL_Point*> allCollision, std::vector<std::string>& collisionIdentifier, std::vector<std::string>& collisionWorld);
	static bool intersectPolygons(std::vector<vector> verticesA, std::vector<vector> verticesB, vector& normal, float& depth);
	static void projectVertices(std::vector<vector> vertices, vector axis, float& min, float& max);
	static vector findArithmeticMean(std::vector<vector> vertices);
	
	static void removeRect();
	static void changeIdentifier();
	static void addCollisionBox();

	static inline std::vector<vector> multiple;


	static inline std::string currWorldName;

	// i, j
	static inline vector selectedPoint = { -1, -1 };

	// {i, j}, loc
	static inline std::vector<std::vector<vector>> previousActions;
	static inline vector startLoc;

	static inline int mouseOver = -1;
};