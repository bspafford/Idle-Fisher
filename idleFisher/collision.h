#pragma once

#include <string>
#include <mutex>

#include "math.h"

class Shader;

struct Fcollision {
	vector points[4];
	char identifier = ' ';
	bool isCircle = false;
	float radius = 0.f;

	float minX = 0.f;
	float maxX = 0.f;
	float minY = 0.f;
	float maxY = 0.f;

	Fcollision() {}

	// for polygon collision
	Fcollision(std::vector<vector> worldPoints, char identifier);

	// for circle collision
	Fcollision(vector center, float radius, char identifier);

	std::vector<vector> GetPoints() {
		return std::vector<vector>(points, points + GetNumPoints());
	}

	// here incase i decide to ever change it
	int GetNumPoints() {
		return 4;
	}

};

class collision {
public:
	static void Init();

	// calculates all the collision.col file information into a list
	static void LoadWorldsCollision(uint32_t worldId);
	static void removeCollisionObject(Fcollision* collision);
	static std::string getIdentifier(std::string str);

	// circles vs polygons
	static bool intersectCirclePolygon(vector circleCenter, float circleRadius, const std::vector<vector>& vertices, vector& normal, float& depth);
	static int findClosestPointOnPolygon(vector circleCenter, const std::vector<vector>& vertices);
	static void projectCircle(vector center, float radius, vector axis, float& min, float& max);

	// polygons vs polygons
	static bool intersectPolygons(const std::vector<vector>& verticesA, const std::vector<vector>& verticesB, vector& normal, float& depth);
	static void projectVertices(const std::vector<vector>& vertices, vector axis, float& min, float& max);

	// circles vs circles
	static bool intersectCircles(vector centerA, float radiusA, vector centerB, float radiusB, vector& normal, float& depth);

	static void removeSpaces(std::string& str);

	static void showCollisionBoxes(Shader* shaderProgram);

	static bool isCloseEnough(const Fcollision* a, const Fcollision* col);

	static vector findArithmeticMean(const std::vector<vector>& vertices);

	// CCD
	static bool sweepPointVsEdge(vector p0, vector v, vector edgeStart, vector edgeEnd, float radius, float& toiOut, vector& normalOut);
	static bool sweepPointVsCircle(vector p0, vector v, vector center, float radius, float& toiOut, vector& normalOut);
	static void TestCollision(Fcollision* playerCol, vector move, float deltaTime);
	static bool circleVsCircle(Fcollision* playerCol, vector v, Fcollision* circleCol, float& toiOut, vector& normalOut);

	static bool testMouse(vector mousePos);
	static bool pointInQuad(vector mousePos, Fcollision* col);
	static bool pointInTriangle(const vector& pt, const vector& a, const vector& b, const vector& c);

	static std::vector<Fcollision*>& getCollisionList();
	static void addCollisionObject(Fcollision* col);
	static void replaceCollisionObject(Fcollision* oldCol, Fcollision* newCol);

	static const std::vector<Fcollision*>& GetGroundCollision();

	static bool IsPointInsidePolygon(Fcollision* col, const vector& point);
	static bool IsPointInsidePolygon(const std::vector<vector>& polygon, const vector& point);

private:
	// used for low fps, just clamps move dir
	static float TestCCD(Fcollision* playerCol, vector move);
	// returns minimum translation vector (mtv)
	static vector TestSAT(Fcollision* playerCol);

	static inline std::mutex mutex;

	static inline std::vector<Fcollision*> allCollision;
	static inline std::vector<Fcollision*> stairCollision;
	static inline std::vector<Fcollision*> groundCollision;
	// holds unique_ptr for allCollision
	//static inline std::vector<std::unique_ptr<Fcollision>> collisionStorage;
	static inline std::unordered_map<uint32_t, std::unique_ptr<std::vector<Fcollision>>> colMap;
};
