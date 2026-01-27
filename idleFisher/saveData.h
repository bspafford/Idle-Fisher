#pragma once

#include "math.h"
#include "timer.h"
#include <chrono>
#include <string>
#include <nlohmann/json.hpp>

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
    uint32_t currencyId;
    int currencyNum;
    uint32_t worldId;
    int minSize = 1;
    int maxSize = 1;
    bool isRareFish = 0;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FfishData, id, name, description, thumbnail, probability, fishingPower, fishSpeed, greenDifficulty, yellowDifficulty, currencyId, currencyNum, worldId, minSize, maxSize, isRareFish);

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
        currencyId = std::stoul(row[9]);
        currencyNum = std::stoi(row[10]);
        worldId = std::stoul(row[11]);
        minSize = std::stoi(row[12]);
        maxSize = std::stoi(row[13]);
        isRareFish = std::stoi(row[14]);
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
    // world id
    uint32_t id;
    std::string thumbnail;
    std::string name;

    void parseData(std::vector<std::string> row) {
        id = std::stoul(row[0]);
        thumbnail = row[1];
        name = row[2];
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FcurrencyStruct, id, thumbnail, name);
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
    uint32_t id;
    uint32_t worldId;
    float xLoc;
    float yLoc;

    void parseData(std::vector<std::string> row) {
        id = std::stoul(row[0]);
        worldId = std::stoul(row[1]);
        xLoc = std::stof(row[2]);
        yLoc = std::stof(row[3]);
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FautoFisherStruct, id, worldId, xLoc, yLoc);
};

struct FsaveAutoFisherStruct {
    uint32_t id;
    double fullness = 0;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FsaveAutoFisherStruct, id, fullness);
};

struct FworldStruct {
    uint32_t id;
    std::string name;
    std::string description;
    uint32_t currencyId;
    double currencyNum;

    void parseData(std::vector<std::string> row) {
        id = std::stoul(row[0]);
        name = row[1];
        description = row[2];
        currencyId = std::stoul(row[3]);
        currencyNum = std::stod(row[4]);
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FworldStruct, id, name, description, currencyId, currencyNum);
};

struct FfishingRodStruct {
    uint32_t id;
    std::string name;
    std::string imgPath;

    void parseData(std::vector<std::string> row) {
        id = std::stoul(row[0]);
        name = row[1];
        imgPath = row[2];
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FfishingRodStruct, id, name, imgPath);
};

struct FsaveFishingRodStruct {
    SaveEntry power;
	SaveEntry speed;
	SaveEntry catchChance;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FsaveFishingRodStruct, power, speed, catchChance);
};

struct FbaitStruct {
    uint32_t id;
    std::string thumbnail;
    std::string name;
    std::string description;
    std::string buffs;
    std::string debuffs;
    uint32_t currencyId;
    double currencyNum;
    std::vector<double> buffValues;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FbaitStruct, id, thumbnail, name, description, buffs, debuffs, currencyId, currencyNum, buffValues);

    void parseData(std::vector<std::string> row) {
        id = std::stoul(row[0]);
        thumbnail = row[1];
        name = row[2];
        description = row[3];
        buffs = row[4];
        debuffs = row[5];
        currencyId = std::stoul(row[6]);
        currencyNum = std::stod(row[7]);

        // parse the list
        if (row[8].find("(") != std::string::npos) {
            std::string string = row[8];
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
        } else if (row[8] != "")
            buffValues.push_back(std::stoi(row[8]));
    }
};

struct FachievementStruct {
    uint32_t id;
    std::string name;
    std::string description;
    std::string thumbnailPath;

    void parseData(std::vector<std::string> row) {
        id = std::stoul(row[0]);
        name = row[1];
        description = row[2];
        thumbnailPath = row[3];
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FachievementStruct, id, name, description, thumbnailPath);
};

struct FupgradeStruct {
    uint32_t id;
    std::string name;
    std::string description;
    std::string thumbnailPath;
    uint32_t worldId;
    double baseValue;
    int upgradeNumMax;
    std::string upgradeFunctionName;
    std::string upgradeNumEquation;
    std::string priceEquation;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FupgradeStruct, id, name, description, thumbnailPath, worldId, baseValue, upgradeNumMax, upgradeFunctionName, upgradeNumEquation, priceEquation);

    void parseData(std::vector<std::string> row) {
        id = std::stoul(row[0]);
        name = row[1];
        description = row[2];
        thumbnailPath = row[3];
        worldId = std::stoul(row[4]);
        baseValue = std::stod(row[5]);
        upgradeNumMax = std::stoi(row[6]);
        upgradeFunctionName = row[7];
        upgradeNumEquation = row[8];
        priceEquation = row[9];
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
    std::string name;
    std::string description;
    std::string thumbnailPath;
    uint32_t currencyId;
    double currencyNum;
    float buffValue;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FpetStruct, id, name, description, thumbnailPath, currencyId, currencyNum, buffValue);

    void parseData(std::vector<std::string> row) {
        id = std::stoul(row[0]);
        name = row[1];
        description = row[2];
        thumbnailPath = row[3];
        currencyId = std::stoul(row[4]);
        currencyNum = std::stod(row[5]);
        buffValue = std::stof(row[6]);
    }
};

struct FmechanicStruct {
    // world id
    uint32_t id;
    double currencyNum;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FmechanicStruct, id, currencyNum);

    void parseData(std::vector<std::string> row) {
        id = std::stoul(row[0]);
        currencyNum = std::stod(row[1]);
    }
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

    // npc upgrades
    std::unordered_map<uint32_t, FupgradeStruct> upgradeData;
    std::unordered_map<uint32_t, FworldStruct> worldData;
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

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Fdata, fishData, goldenFishData, currencyData, upgradeData, worldData, mechanicStruct, autoFisherData, petData, vaultUnlockData, fishingRodData, baitData, achievementData, rebirthData);
};

struct ForderedData {
    std::vector<uint32_t> fishData;
    std::vector<uint32_t> goldenFishData;
    std::vector<uint32_t> currencyData;
    std::vector<uint32_t> upgradeData;
    std::vector<uint32_t> worldData;
    std::vector<uint32_t> mechanicStruct;
    std::vector<uint32_t> autoFisherData;
    std::vector<uint32_t> petData;
    std::vector<uint32_t> vaultUnlockData;
    std::vector<uint32_t> fishingRodData;
    std::vector<uint32_t> baitData;
    std::vector<uint32_t> achievementData;
    std::vector<uint32_t> rebirthData;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ForderedData, fishData, goldenFishData, currencyData, upgradeData, worldData, mechanicStruct, autoFisherData, petData, vaultUnlockData, fishingRodData, baitData, achievementData, rebirthData);
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

    // npc upgrades
    std::unordered_map<uint32_t, FsaveNPCStruct> npcSave;
    std::unordered_map<uint32_t, SaveEntry> upgradeList;
    std::unordered_map<uint32_t, SaveEntry> worldList;
    std::unordered_map<uint32_t, SaveEntry> mechanicStruct;
    std::unordered_map<uint32_t, std::pair<SaveEntry, FsaveAutoFisherStruct>> autoFisherList;
    std::unordered_map<uint32_t, SaveEntry> petList;
    uint32_t equippedPetId;
    std::unordered_map<uint32_t, FsaveVaultUnlocksStruct> vaultUnlockList;

    // upgrades
    FsaveFishingRodStruct fishingRod;
    uint32_t equippedBaitId;
    std::unordered_map<uint32_t, SaveEntry> baitList;

    // big stuff
    std::unordered_map<uint32_t, SaveEntry> achievementList;

    // rebirth stuff
    std::unordered_map<uint32_t, SaveEntry> rebirthList;
    double rebirthCurrency = 0;
    double totalRebirthCurrency = 0;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FsaveData, startTime, lastPlayed, playerLoc, currWorld, prevWorld, fishData, currencyList, currencyConversionList, npcSave, upgradeList,
        worldList, mechanicStruct, autoFisherList, petList, equippedPetId, vaultUnlockList, fishingRod, equippedBaitId, baitList, achievementList,
        rebirthList, rebirthCurrency, totalRebirthCurrency);
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
