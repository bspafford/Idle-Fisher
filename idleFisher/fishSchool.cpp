#include "fishSchool.h"
#include "animation.h"
#include "timer.h"
#include "worlds.h"
#include "upgrades.h"

#include "debugger.h"

AfishSchool::AfishSchool() {
	std::unordered_map<std::string, animDataStruct> animData;
	animData.insert({ "fishSchool", animDataStruct({0, 0}, {6, 0}, true) });
	anim = std::make_unique<animation>("fishSchool/fishSchool.png", 48, 26, animData, true, vector(0, 0));
	anim->setAnimation("fishSchool");
	anim->start();

	lifeTimer = CreateDeferred<Timer>();
	lifeTimer->addCallback(this, &AfishSchool::Finished);

	spawnTimer = CreateDeferred<Timer>();
	spawnTimer->addCallback(this, &AfishSchool::Start);
	spawnTimer->start(math::randRange(Upgrades::Get(Stat::MinFishSchoolSpawnInterval), Upgrades::Get(Stat::MaxFishSchoolSpawnInterval)));
}

void AfishSchool::Draw(Shader* shaderProgram) {
	if (isAlive)
		anim->draw(shaderProgram);
}

bool AfishSchool::PointInSchool(vector worldPoint) {
	if (!isAlive)
		return false;

	// Calc if in ellipse
	float h = anim->getLoc().x + anim->GetCellSize().x / 2.f;
	float k = anim->getLoc().y + anim->GetCellSize().y / 2.f;
	float a = anim->GetCellSize().x / 2.f;
	float b = anim->GetCellSize().y / 2.f;
	float y = (((worldPoint.x - h) * (worldPoint.x - h)) / (a * a)) + (((worldPoint.y - k) * (worldPoint.y - k)) / (b * b));
	return y <= 1;
}

void AfishSchool::Start() {
	isAlive = true;
	anim->setLoc(GetRandomLoc());
	lifeTimer->start(math::randRange(Upgrades::Get(Stat::MinFishSchoolLifetime), Upgrades::Get(Stat::MaxFishSchoolLifetime)));
}

void AfishSchool::Finished() {
	isAlive = false;
	spawnTimer->start(math::randRange(Upgrades::Get(Stat::MinFishSchoolSpawnInterval), Upgrades::Get(Stat::MaxFishSchoolSpawnInterval)));
}

vector AfishSchool::GetRandomLoc() {
	std::vector<Fcollision*> fishSchoolCollisions;
	std::vector<std::pair<float, float>> areaList;
	float totalArea = 0.f;
	// find all fish school collisions and sum up total area
	for (Fcollision* col : collision::getCollisionList()) {
		if (col->identifier == 'f') {
			fishSchoolCollisions.push_back(col);

			float triArea1 = std::abs(math::cross(col->points[1] - col->points[0], col->points[2] - col->points[0])) / 2.f;
			float triArea2 = std::abs(math::cross(col->points[2] - col->points[0], col->points[3] - col->points[0])) / 2.f;
			areaList.push_back(std::pair(triArea1, triArea2));
			totalArea += triArea1 + triArea2;
		}
	}

	// select random quad
	Fcollision* selectedQuad = nullptr;
	std::pair<float, float> selectedArea;
	float rand = math::randRange(0.f, totalArea);
	float cumulative = 0.f;
	for (size_t i = 0; i < areaList.size(); i++) {
		cumulative += areaList[i].first + areaList[i].second;
		if (rand <= cumulative) {
			selectedQuad = fishSchoolCollisions[i];
			selectedArea = areaList[i];
			break;
		}
	}

	// choose random triangle from quad
	float triArea1 = selectedArea.first;
	float triArea2 = selectedArea.second;
	float randTri = math::randRange(0.f, triArea1 + triArea2);
	vector A, B, C;
	if (randTri <= triArea1) {
		A = selectedQuad->points[0];
		B = selectedQuad->points[1];
		C = selectedQuad->points[2];
	} else {
		A = selectedQuad->points[0];
		B = selectedQuad->points[2];
		C = selectedQuad->points[3];
	}

	// get random loc inside quad
	float r1 = math::randRange(0.f, 1.f);
	float r2 = math::randRange(0.f, 1.f);
	if (r1 + r2 > 1.f) {
		r1 = 1.f - r1;
		r2 = 1.f - r2;
	}
	return A + (B - A) * r1 + (C - A) * r2;
}
