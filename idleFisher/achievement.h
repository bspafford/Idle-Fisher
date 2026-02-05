#pragma once

#include <functional>
#include <unordered_map>
#include <unordered_set>

enum class AchievementTrigger {
    Click = 1,
    FishCaught = 2,
    PetPurchased = 3,
    CurrencyEarned = 4,
    CurrencyPerSecond = 5,
    PurchaseAutoFisher = 6,
    UpgradeAutoFisher = 7,
    FishSchool = 8,
    BaitPurchased = 9,
    PremiumBuffs = 10,
    FishTransporter = 11,
};

// achievement Class
class Achievements {
private:
    static inline std::unordered_map<uint32_t, std::function<bool()>> achievements;
    static inline std::unordered_map<AchievementTrigger, std::unordered_set<uint32_t>> achievementsPerTrigger;

    static void NotifyPlayer(uint32_t id);

    static void AddAchievement(uint32_t id, std::function<bool()> condition);

    static double CalcMoneyPerSecond();
    static bool CheckQuality(double caughtNum, int quality);
    static bool CaughtSpecialFish(double caughtNum);
    
public:
    static void Init();
    static void CheckGroup(AchievementTrigger trigger);
};