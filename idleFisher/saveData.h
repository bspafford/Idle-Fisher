#pragma once

#include "math.h"
#include "timer.h"
#include <chrono>
#include <string>
#include <nlohmann/json.hpp>

enum class Stat;

enum Resolution {
    RES_NATIVE,
    RES_1280x720,
    RES_1920x1080,
    RES_2560x1440
};

struct SaveEntry {
    uint32_t id;
    int level = 0;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(SaveEntry, id, level);
};

struct ScalingFormula {
    // ((base + add * level) * multiply^level)^exponent
    double base = 0.0;
    double add = 0.0;
    double mul = 1.0;
    double exp = 1.0;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ScalingFormula, base, add, mul, exp);
};

struct FfishData {
    uint32_t id;
    std::string name;
    std::string description;
    std::string thumbnail;
    float probability;
    float fishingPower;
    float fishSpeed;
    float greenDifficulty;
    float yellowDifficulty;
    int currencyNum;
    uint32_t worldId;
    int minSize = 1;
    int maxSize = 1;
    bool isRareFish = 0;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FfishData, id, name, description, thumbnail, probability, fishingPower, fishSpeed, greenDifficulty, yellowDifficulty, currencyNum, worldId, minSize, maxSize, isRareFish);

    void parseData(std::vector<std::string> row) {
        id = std::stoul(row[0]);
        name = row[1];
        description = row[2];
        thumbnail = row[3];
        probability = std::stof(row[4]);
        fishingPower = std::stof(row[5]);
        fishSpeed = std::stof(row[6]);
        greenDifficulty = std::stof(row[7]);
        yellowDifficulty = std::stof(row[8]);
        currencyNum = std::stoi(row[9]);
        worldId = std::stoul(row[10]);
        minSize = std::stoi(row[11]);
        maxSize = std::stoi(row[12]);
        isRareFish = std::stoi(row[13]);
    }

    static FfishData GetCheapestFishInWorld(uint32_t worldId = 0);
};

struct FsaveFishData {
    uint32_t id;
    bool unlocked = false;
    // 0 = normal, 1 = bronze, 2 = silver, 3 = gold
    std::vector<double> numOwned = std::vector<double>(4);
    // 0 = normal, 1 = bronze, 2 = silver, 3 = gold
    std::vector<double> totalNumOwned = std::vector<double>(4);
    int biggestSizeCaught = 0;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FsaveFishData, id, unlocked, numOwned, totalNumOwned, biggestSizeCaught);

    double calcTotalCaughtFish() {
        double total = 0;
        for (int i = 0; i < totalNumOwned.size(); i++)
            total += totalNumOwned[i];
        return total;
    }

    double calcCaughtFish() {
        double caught = 0;
        for (int i = 0; i < numOwned.size(); i++)
            caught += numOwned[i];
        return caught;
    }
};

struct FcurrencyStruct {
    uint32_t id; // world / upgrade id
    std::string name;

    void parseData(std::vector<std::string> row) {
        id = std::stoul(row[0]);
        name = row[1];
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FcurrencyStruct, id, name);
};

struct FsaveCurrencyStruct {
    // world id
    uint32_t id;
    double numOwned = 0;
    double totalNumOwned = 0;
    bool unlocked = false;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FsaveCurrencyStruct, id, numOwned, totalNumOwned, unlocked);
};

struct FautoFisherStruct {
    uint32_t id; // progression id
    float xLoc;
    float yLoc;

    void parseData(std::vector<std::string> row) {
        id = std::stoul(row[0]);
        xLoc = std::stof(row[1]);
        yLoc = std::stof(row[2]);
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FautoFisherStruct, id, xLoc, yLoc);
};

struct FsaveAutoFisherStruct {
    uint32_t id;
    double fullness = 0;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FsaveAutoFisherStruct, id, fullness);
};

struct FfishingRodStruct {
    uint32_t id; // progression id
    ScalingFormula effect;

    void parseData(std::vector<std::string> row) {
        id = std::stoul(row[0]);
		effect = { std::stod(row[1]), std::stod(row[2]), std::stod(row[3]), std::stod(row[4]) };
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FfishingRodStruct, id, effect);
};

struct FsaveFishingRodStruct {
    SaveEntry power;
	SaveEntry speed;
	SaveEntry catchChance;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FsaveFishingRodStruct, power, speed, catchChance);
};

struct FbaitStruct {
    uint32_t id; // progression id
    std::string buffs;
    std::string debuffs;
    std::vector<double> buffValues;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FbaitStruct, id, buffs, debuffs, buffValues);

    void parseData(std::vector<std::string> row) {
        id = std::stoul(row[0]);
        buffs = row[1];
        debuffs = row[2];

        // parse the list
        if (row[3].find("(") != std::string::npos) {
            std::string string = row[3];
            string.erase(0, 1);
            string.erase(string.size() - 1, 1);

            std::string num;
            std::string delimiter = ",";
            size_t pos;
            while (string != "") {
                pos = string.find(delimiter);
                if (pos == std::string::npos)
                    pos = string.size();

                num = string.substr(0, pos);
                string.erase(0, pos + delimiter.length());
                buffValues.push_back(std::stoi(num));
            }
        } else if (row[3] != "")
            buffValues.push_back(std::stoi(row[3]));
    }
};

struct FachievementStruct {
    uint32_t id;
    std::string name;
    std::string description;
    Stat stat; // what they buff
	double weight; // how much they buff

    void parseData(std::vector<std::string> row) {
        id = std::stoul(row[0]);
        name = row[1];
        description = row[2];
		stat = static_cast<Stat>(std::stoi(row[3]));
		weight = std::stod(row[4]);
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FachievementStruct, id, name, description, stat, weight);
};

struct FupgradeStruct {
    uint32_t id; // progression id
    Stat stat; // what this upgrade improves
    ScalingFormula effect; // upgrade scaling

    void parseData(std::vector<std::string> row) {
        id = std::stoul(row[0]);
        stat = static_cast<Stat>(std::stoi(row[1]));
        effect = { std::stod(row[2]), std::stod(row[3]), std::stod(row[4]), std::stod(row[5]) };
	}

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FupgradeStruct, id, stat, effect);
};

// upgrades that improve/unlock stats
struct ProgressionNode {
    uint32_t id; // unique id for this upgrade
    uint32_t worldId;
    int maxLevel = 1;

    std::string name;
    std::string description;

    ScalingFormula cost;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ProgressionNode, id, worldId, maxLevel, name, description, cost);

    void parseData(std::vector<std::string> row) {
        id = std::stoul(row[0]);
        worldId = std::stoi(row[1]);
        maxLevel = std::stoi(row[2]);

        name = row[3];
        description = row[4];

        cost = { std::stod(row[5]), std::stod(row[6]), std::stod(row[7]), std::stod(row[8])};
    }
};

struct FrebirthStruct {
    uint32_t id;
    std::string name;
    std::string description;
    std::string thumbnailPath;
    std::string functionName;
    double rebirthValue;
    std::vector<uint32_t> prerequisites;
    double currencyNum;
    vector loc;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FrebirthStruct, id, name, description, thumbnailPath, functionName, rebirthValue, prerequisites, currencyNum, loc);

    void parseData(std::vector<std::string> row) {
        id = std::stoul(row[0]);
        name = row[1];
        description = row[2];
        thumbnailPath = row[3];
        functionName = row[4];
        rebirthValue = std::stod(row[5]);

        // parse the list
        if (row[6].find("(") != std::string::npos) {
            std::string string = row[6];
            string.erase(0, 1);
            string.erase(string.size() - 1, 1);

            uint32_t num;
            std::string delimiter = ",";
            size_t pos;
            while (string != "") {
                pos = string.find(delimiter);
                if (pos == std::string::npos)
                    pos = string.size();

                num = std::stoul(string.substr(0, pos));
                string.erase(0, pos + delimiter.length());
                prerequisites.push_back(num);
            }
        } else if (row[6] != "")
            prerequisites.push_back(std::stoul(row[6]));

        currencyNum = std::stod(row[7]);
        loc = { std::stof(row[8]), std::stof(row[9]) };
    }
};

struct FpetStruct {
    uint32_t id;
	Stat stat; // what this pet improves

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FpetStruct, id, stat);

    void parseData(std::vector<std::string> row) {
        id = std::stoul(row[0]);
		stat = static_cast<Stat>(std::stoi(row[1]));
    }
};

struct FmechanicStruct {
	uint32_t id; // progression id
    uint32_t worldId;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FmechanicStruct, id, worldId);
};

struct FgoldenFishStruct {
    uint32_t id;
    std::string name;
    std::string description;
    double multiplier;
    double comboMultiplier;
    float time;
    double catchChance;

    void parseData(std::vector<std::string> row) {
        id = std::stoul(row[0]);
        name = row[1];
        description = row[2];
        multiplier = std::stod(row[3]);
        comboMultiplier = std::stod(row[4]);
        time = std::stof(row[5]);
        catchChance = std::stod(row[6]);
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FgoldenFishStruct, id, name, description, multiplier, comboMultiplier, time, catchChance);
};

struct FvaultUnlocksStruct {
    uint32_t id;
    uint32_t currencyId;
    double currencyNum;
    std::string name;
    std::string description;
    std::string buffFunction;
    std::string thumbnailPath;
    double buffValue;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FvaultUnlocksStruct, id, currencyId, currencyNum, name, description, buffFunction, thumbnailPath, buffValue);

    void parseData(std::vector<std::string> row) {
        id = std::stoul(row[0]);
        currencyId = std::stoul(row[1]);
        currencyNum = std::stod(row[2]);
        name = row[3];
        description = row[4];
        buffFunction = row[5];
        thumbnailPath = row[6];
        buffValue = std::stod(row[7]);
    }
};

struct FsaveVaultUnlocksStruct {
    uint32_t id;
    bool placed = false;
    vector loc;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FsaveVaultUnlocksStruct, id, placed, loc);
};

struct FsaveNPCStruct {
    uint32_t id;
    bool merchantDiscovered = false;
    bool sailorDiscovered = false;
    bool fishermanDiscovered = false;
    bool mechanicDiscovered = false;
    bool petSellerDiscovered = false;
    bool atmDiscovered = false;
    bool scubaDiscovered = false;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FsaveNPCStruct, id, merchantDiscovered, sailorDiscovered, fishermanDiscovered, mechanicDiscovered, petSellerDiscovered, atmDiscovered, scubaDiscovered);
};

struct FcurrencyConversionStruct {
    uint32_t id;                 // id of currency
    float timerMax = 1;     // base conversion time
    bool converting = false;// is converting
    double price = 100;     // base input
    double yield = 1;       // base output

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FcurrencyConversionStruct, id, timerMax, converting, price, yield);
};

struct Fdata {
    // fish stuff
    std::unordered_map<uint32_t, FfishData> fishData;
    std::unordered_map<uint32_t, FgoldenFishStruct> goldenFishData;

    // currencies
    std::unordered_map<uint32_t, FcurrencyStruct> currencyData;

    std::unordered_map<uint32_t, ProgressionNode> progressionData;

    // npc upgrades
    std::unordered_map<uint32_t, FupgradeStruct> upgradeData;
    std::unordered_map<uint32_t, FmechanicStruct> mechanicStruct;
    std::unordered_map<uint32_t, FautoFisherStruct> autoFisherData;
    std::unordered_map<uint32_t, FpetStruct> petData;
    std::unordered_map<uint32_t, FvaultUnlocksStruct> vaultUnlockData;
    

    // upgrades
    std::unordered_map<uint32_t, FfishingRodStruct> fishingRodData;
    std::unordered_map<uint32_t, FbaitStruct> baitData;

    // big stuff
    std::unordered_map<uint32_t, FachievementStruct> achievementData;
    std::unordered_map<uint32_t, FrebirthStruct> rebirthData;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Fdata, fishData, goldenFishData, currencyData, progressionData, upgradeData, mechanicStruct, autoFisherData, petData, vaultUnlockData, fishingRodData, baitData, achievementData, rebirthData);
};

struct ForderedData {
    std::vector<uint32_t> fishData;
    std::vector<uint32_t> goldenFishData;
	std::vector<uint32_t> currencyData;     // world / upgrade ids
    std::vector<uint32_t> upgradeData;      // progression, holds merchant upgrades
    std::vector<uint32_t> worldData;        // progression ids
	std::vector<uint32_t> mechanicStruct;   // progression ids
    std::vector<uint32_t> autoFisherData;   // progression ids
    std::vector<uint32_t> petData;          // progression ids
    std::vector<uint32_t> vaultUnlockData;
    std::vector<uint32_t> baitData;         // progression ids
    std::vector<uint32_t> achievementData;
    std::vector<uint32_t> rebirthData;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ForderedData, fishData, goldenFishData, currencyData, upgradeData, worldData, mechanicStruct, autoFisherData, petData, vaultUnlockData, baitData, achievementData, rebirthData);
};

struct FsaveData {
    // stored in milliseconds
    long long startTime;
    long long lastPlayed;

    // player loc
    vector playerLoc = { 730, 490 };

    uint32_t currWorld = 4u; // world1
    uint32_t prevWorld = 4u; // world1 // incase the player is in the vault when they exit / save

    // fish stuff
    std::unordered_map<uint32_t, FsaveFishData> fishData;

    // currencies
    std::unordered_map<uint32_t, FsaveCurrencyStruct> currencyList;
    std::unordered_map<uint32_t, FcurrencyConversionStruct> currencyConversionList;

    std::unordered_map<uint32_t, SaveEntry> progressionData;

    // npc upgrades
    std::unordered_map<uint32_t, FsaveNPCStruct> npcSave;
    std::unordered_map<uint32_t, FsaveAutoFisherStruct> autoFisherList;
    uint32_t equippedPetId; // progression id
    std::unordered_map<uint32_t, FsaveVaultUnlocksStruct> vaultUnlockList;

    // upgrades
    FsaveFishingRodStruct fishingRod;
    uint32_t equippedBaitId; // progression id

    std::unordered_map<uint32_t, SaveEntry> achievementList;

    // rebirth stuff
    std::unordered_map<uint32_t, SaveEntry> rebirthList;
    double rebirthCurrency = 0;
    double totalRebirthCurrency = 0;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FsaveData, startTime, lastPlayed, playerLoc, currWorld, prevWorld, fishData, currencyList, currencyConversionList, progressionData, npcSave,
        equippedPetId, vaultUnlockList, fishingRod, equippedBaitId, achievementList, rebirthList, rebirthCurrency, totalRebirthCurrency);
};

struct FsettingsData {
    // audio
    float masterVolume = 50;
    float musicVolume = 100;
    float sfxVolume = 100;
    float dialogVolume = 100;

    // graphics
    int monitorIdx = 0; // 0 = primary
    int fullScreen = 1;
    int resolution = RES_NATIVE;
    int vsync = false;
    int fpsLimit = 0; // 0 is uncapped
    int pixelFont = true;
    int shortNumbers = true;
    int showPets = true;
    int showRain = true;
    int cursor = true;

    // Misc
    int interpMethod = 0;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FsettingsData, masterVolume, musicVolume, sfxVolume, dialogVolume, monitorIdx, fullScreen, resolution, vsync, fpsLimit, pixelFont, shortNumbers, showPets, showRain, cursor, interpMethod);

    // compares the struct byte by byte, so i don't have to manually update it
    bool operator==(const FsettingsData other) {
        size_t size = sizeof(FsettingsData);

        // compare memory bit by bit
        const unsigned char* a = reinterpret_cast<const unsigned char*>(this);
        const unsigned char* b = reinterpret_cast<const unsigned char*>(&other);
        
        for (int i = 0; i < size; i++) {
            if (a[i] != b[i])
                return false;
        }
        return true;
    }
};

class SaveData {
public:
    static void save();
    static void load();

    static void saveSettings();
    static void loadSettings();

    // loads in the static data (Fdata)
    static void LoadData();

    static inline Fdata data;
    // used to maintain the order of the data
    static inline ForderedData orderedData;
    static inline FsaveData saveData;
    static inline FsettingsData settingsData;

    // keeps track of the time the save file was created
    static inline std::chrono::system_clock::time_point startTime;
    static inline std::chrono::system_clock::time_point lastPlayed;

    // used to setup default values and vector sizes for all the save stuff
    static void recalcLists();

    static std::filesystem::path GetSaveFolder();
    static std::filesystem::path GetSaveDataPath();
    static std::filesystem::path GetSettingsDataPath();
    // data/
    static std::filesystem::path GetDataPath();
    // data/data.json
    static std::filesystem::path GetJsonDataPath();

private:
    static void autoSave();
    static inline float autoSaveInterval = 60.f; // in seconds

    static inline DeferredPtr<Timer> autoSaveTimer;

    static inline bool hasLoaded = false;
};
