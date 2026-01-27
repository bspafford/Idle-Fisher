#pragma once

#include "widget.h"
#include "timer.h"

struct FachievementStruct;
struct SaveEntry;

class UachievementBox : public widget {
public:
	UachievementBox(widget* parent, uint32_t id);
	~UachievementBox();

	void draw(Shader* shaderProgram);

	vector getSize() override;

	void updateAchievementImage();

	uint32_t GetId() { return id; }

private:
	bool mouseOver() override;

	std::unique_ptr<Image> icon;

	uint32_t id;
	FachievementStruct* achievementStruct;
	SaveEntry* saveAchievementStruct;

	vector startSize;
	static inline float rot;

	static inline DeferredPtr<Timer> rotateTimer;
	static inline float rotateTimerSpeed;
	static void rotateUpdate();
	static void finishRotateTimer();
};