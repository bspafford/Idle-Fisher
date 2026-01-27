#pragma once

#include <iostream>
#include <vector>
#include <functional>
#include <string>
#include <map>

#include "main.h"
#include "saveData.h"
#include "fishUnlocked.h"
#include "achievementWidget.h"

// achievement Class
class achievement {
private:
    FachievementStruct* achievementData;
    SaveEntry* saveAchievementData;
    std::function<bool()> condition; // Condition as a lambda function
    static inline std::vector<std::unique_ptr<achievement>> achievements;

public:
    achievement(uint32_t id, std::function<bool()> cond)
        : achievementData(&SaveData::data.achievementData.at(id)), saveAchievementData(&SaveData::saveData.achievementList.at(id)), condition(cond) {}

    void checkUnlock() {
        if (!saveAchievementData->level && condition()) {
            saveAchievementData->level = true;

            // updates the achievement widget icon for specific achievement
            Main::achievementWidget->updateAchievementIcon(saveAchievementData->id);

            notifyPlayer();
        }
    }

    void notifyPlayer() const {
        Main::fishUnlocked->start(achievementData);
    }

    bool isUnlocked() const { return saveAchievementData->level; }

    FachievementStruct* getAchievementData() const { return achievementData; }

    static void createAchievementList();
    static void checkAchievements();
};