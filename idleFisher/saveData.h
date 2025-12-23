#pragma once

#include "math.h"
#include <chrono>
#include <string>

struct FfishData {
    int id;
    std::string name;
    std::string description;
    std::string thumbnail;
    float probability;
    float fishingPower;
    float fishSpeed;
    float greenDifficulty;
    float yellowDifficulty;
    int currencyId;
    int currencyNum;
    std::string levelName;
    int minSize = 1;
    int maxSize = 1;
    bool isRareFish = 0;

    void parseData(std::vector<std::string> row) {
        id = std::stoi(row[0]);
        name = row[1];
        description = row[2];
        thumbnail = row[3];
        probability = std::stof(row[4]);
        fishingPower = std::stof(row[5]);
        fishSpeed = std::stof(row[6]);
        greenDifficulty = std::stof(row[7]);
        yellowDifficulty = std::stof(row[8]);
        currencyId = std::stoi(row[9]);
        currencyNum = std::stoi(row[10]);
        levelName = row[11];
        minSize = std::stoi(row[12]);
        maxSize = std::stoi(row[13]);
        isRareFish = std::stoi(row[14]);
    }
};

struct FsaveFishData {
    int id = -1;
    bool unlocked = false;
    // 0 = normal, 1 = bronze, 2 = silver, 3 = gold
    std::vector<double> numOwned = std::vector<double>(0);
    // 0 = normal, 1 = bronze, 2 = silver, 3 = gold
    std::vector<double> totalNumOwned = std::vector<double>(0);
    int biggestSizeCaught = 0;

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
    int id = -1;
    std::string thumbnail;
    std::string name;

    void parseData(std::vector<std::string> row) {
        id = std::stoi(row[0]);
        thumbnail = row[1];
        name = row[2];
    }
};

struct FsaveCurrencyStruct {
    int id = -1;
    double numOwned = 0;
    double totalNumOwned = 0;
    bool unlocked = false;
};

struct FautoFisherStruct {
    int id = -1;
    std::string worldName;
    float xLoc;
    float yLoc;

    void parseData(std::vector<std::string> row) {
        id = std::stoi(row[0]);
        worldName = row[1];
        xLoc = std::stof(row[2]);
        yLoc = std::stof(row[3]);
    }
};

struct FsaveAutoFisherStruct {
    int id = -1;
    bool unlocked = false;
    int level = 1;
    double fullness = 0;
};

struct FworldStruct {
    int id = -1;
    std::string worldName;
    std::string name;
    std::string description;
    int currencyId;
    double currencyNum;

    void parseData(std::vector<std::string> row) {
        id = std::stoi(row[0]);
        worldName = row[1];
        name = row[2];
        description = row[3];
        currencyId = std::stoi(row[4]);
        currencyNum = std::stod(row[5]);
    }
};

struct FsaveWorldStruct {
    int id = -1;
    bool unlocked = false;
};

struct FfishingRodStruct {
    int id = -1;
    std::string name;
    std::string imgPath;

    void parseData(std::vector<std::string> row) {
        id = std::stoi(row[0]);
        name = row[1];
        imgPath = row[2];
    }
};

struct FsaveFishingRodStruct {
    int id = -1;
    bool unlocked = false;
    int powerLevel = 1;
    int speedLevel = 1;
    int catchChanceLevel = 1;
};

struct FbaitStruct {
    int id = -1;
    std::string thumbnail;
    std::string name;
    std::string description;
    std::string buffs;
    std::string debuffs;
    int currencyId;
    double currencyNum;
    std::vector<double> buffValues;

    void parseData(std::vector<std::string> row) {
        id = std::stoi(row[0]);
        thumbnail = row[1];
        name = row[2];
        description = row[3];
        buffs = row[4];
        debuffs = row[5];
        currencyId = std::stoi(row[6]);
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

struct FsaveBaitStruct {
    int id = -1;
    bool unlocked = false;
};

struct FachievementStruct {
    int id = -1;
    std::string name;
    std::string description;
    std::string thumbnailPath;

    void parseData(std::vector<std::string> row) {
        id = std::stoi(row[0]);
        name = row[1];
        description = row[2];
        thumbnailPath = row[3];
    }
};

struct FsaveAchievementStruct {
    int id = -1;
    bool unlocked = false;
};

struct FupgradeStruct {
    int id = -1;
    std::string name;
    std::string description;
    std::string thumbnailPath;
    int currencyId;
    double baseValue;
    int upgradeNumMax;
    std::string upgradeFunctionName;
    std::string upgradeNumEquation;
    std::string priceEquation;
    std::string levelName;

    void parseData(std::vector<std::string> row) {
        id = std::stoi(row[0]);
        name = row[1];
        description = row[2];
        thumbnailPath = row[3];
        currencyId = std::stoi(row[4]);
        baseValue = std::stod(row[5]);
        upgradeNumMax = std::stoi(row[6]);
        upgradeFunctionName = row[7];
        upgradeNumEquation = row[8];
        priceEquation = row[9];
        levelName = row[10];
    }
};

struct FsaveUpgradeStruct {
    int id = -1;
    int upgradeLevel = 0;
    double value = 0;
    double price = 0;
};

struct FrebirthStruct {
    int id = -1;
    std::string name;
    std::string description;
    std::string thumbnailPath;
    std::string functionName;
    double rebirthValue;
    std::vector<int> prerequisites;
    double currencyNum;
    vector loc;

    void parseData(std::vector<std::string> row) {
        id = std::stoi(row[0]);
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

            std::string num;
            std::string delimiter = ",";
            size_t pos;
            while (string != "") {
                pos = string.find(delimiter);
                if (pos == std::string::npos)
                    pos = string.size();

                num = string.substr(0, pos);
                string.erase(0, pos + delimiter.length());
                prerequisites.push_back(std::stoi(num));
            }
        } else if (row[6] != "")
            prerequisites.push_back(std::stoi(row[6]));

        currencyNum = std::stod(row[7]);
        loc = { std::stof(row[8]), std::stof(row[9]) };
    }
};

struct FsaveRebirthStruct {
    int id = -1;
    bool unlocked = false;
};

struct FpetStruct {
    int id = -1;
    std::string name;
    std::string description;
    std::string thumbnailPath;
    int currencyId;
    double currencyNum;
    float buffValue;

    void parseData(std::vector<std::string> row) {
        id = std::stoi(row[0]);
        name = row[1];
        description = row[2];
        thumbnailPath = row[3];
        currencyId = std::stoi(row[4]);
        currencyNum = std::stod(row[5]);
        buffValue = std::stof(row[6]);
    }
};

struct FsavePetStruct {
    int id = -1;
    bool unlocked = false;
};

struct FmechanicStruct {
    int id = -1;
    double currencyNum;
    std::string levelName;

    void parseData(std::vector<std::string> row) {
        id = std::stoi(row[0]);
        currencyNum = std::stod(row[1]);
        levelName = row[2];
    }
};

struct FsaveMechanicStruct {
    int id = -1;
    bool unlocked = false;
    int level = 0;
};

struct FgoldenFishStruct {
    int id = -1;
    std::string name;
    std::string description;
    double multiplier;
    double comboMultiplier;
    float time;
    double catchChance;

    void parseData(std::vector<std::string> row) {
        id = std::stoi(row[0]);
        name = row[1];
        description = row[2];
        multiplier = std::stod(row[3]);
        comboMultiplier = std::stod(row[4]);
        time = std::stof(row[5]);
        catchChance = std::stod(row[6]);
    }
};

struct FbuffStruct {
    int id = -1;
    std::string name;
    double buffAmount;

    void parseData(std::vector<std::string> row) {
        id = std::stoi(row[0]);
        name = row[1];
        buffAmount = std::stod(row[2]);
    }
};

struct FsaveBuffStruct {
    int id = -1;
    int buffNum = 0;
};

struct FvaultUnlocksStruct {
    int id = -1;
    int currencyId;
    double currencyNum;
    std::string name;
    std::string description;
    std::string buffFunction;
    std::string thumbnailPath;
    double buffValue;

    void parseData(std::vector<std::string> row) {
        id = std::stoi(row[0]);
        currencyId = std::stoi(row[1]);
        currencyNum = std::stod(row[2]);
        name = row[3];
        description = row[4];
        buffFunction = row[5];
        thumbnailPath = row[6];
        buffValue = std::stod(row[7]);
    }
};

struct FsaveVaultUnlocksStruct {
    int id = -1;
    bool unlocked = false;
    bool placed = false;
    vector loc;
};

struct FsaveNPCStruct {
    int id = -1;
    bool merchantDiscovered = false;
    bool sailorDiscovered = false;
    bool fishermanDiscovered = false;
    bool mechanicDiscovered = false;
    bool petSellerDiscovered = false;
    bool atmDiscovered = false;

    bool scubaDiscovered = false;
};

struct FcurrencyConversionStruct {
    int id;                 // id of currency
    float timerMax = 1;     // base conversion time
    bool converting = false;// is converting
    double price = 100;     // base input
    double yield = 1;       // base output
};

struct Fdata {
    // fish stuff
    std::vector<FfishData> fishData;
    std::vector<FgoldenFishStruct> goldenFishData;

    // currencies
    std::vector<FcurrencyStruct> currencyData;

    // npc upgrades
    std::vector<FupgradeStruct> upgradeData;
    std::vector<FworldStruct> worldData;
    std::vector<FmechanicStruct> mechanicStruct;
    std::vector<FautoFisherStruct> autoFisherData;
    std::vector<FpetStruct> petData;
    std::vector<FvaultUnlocksStruct> vaultUnlockData;

    // upgrades
    std::vector<FfishingRodStruct> fishingRodData;
    std::vector<FbaitStruct> baitData;
    std::vector<FbuffStruct> buffData;

    // big stuff
    std::vector<FachievementStruct> achievementData;
    std::vector<FrebirthStruct> rebirthData;
};

struct FsaveData {
    // last time played
    //static inline std::chrono::system_clock::time_point lastPlayed;

    // player loc
    vector playerLoc = { 200, -84 };

    std::string currWorld = "world1";
    std::string prevWorld = "world1"; // incase the player is in the vault when they exit / save

    // fish stuff
    std::vector<FsaveFishData> fishData;

    // currencies
    std::vector<FsaveCurrencyStruct> currencyList;
    std::vector<FcurrencyConversionStruct> currencyConversionList;

    // npc upgrades
    std::vector<FsaveNPCStruct> npcSave;
    std::vector<FsaveUpgradeStruct> upgradeList;
    std::vector<FsaveWorldStruct> worldList;
    std::vector<FsaveMechanicStruct> mechanicStruct;
    std::vector<FsaveAutoFisherStruct> autoFisherList;
    std::vector<FsavePetStruct> petList;
    FpetStruct equippedPet;
    std::vector<FsaveVaultUnlocksStruct> vaultUnlockList;

    // upgrades
    FsaveFishingRodStruct fishingRod;
    FbaitStruct equippedBait;
    std::vector<FsaveBaitStruct> baitList;
    std::vector<FsaveBuffStruct> buffList;

    // big stuff
    std::vector<FsaveAchievementStruct> achievementList;

    // rebirth stuff
    std::vector<FsaveRebirthStruct> rebirthList;
    double rebirthCurrency = 0;
    double totalRebirthCurrency = 0;
};

struct FsettingsData {
    // audio
    float masterVolume = 50;
    float musicVolume = 100;
    float sfxVolume = 100;
    float dialogVolume = 100;

    // graphics
    bool pixelFont = true;
    bool shortNumbers = true;
    bool showPets = true;
    bool showRain = true;
    bool cursor = true;

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

    static inline Fdata data;
    static inline FsaveData saveData;
    static inline FsettingsData settingsData;

    // keeps track of the time the save file was created
    static inline std::chrono::system_clock::time_point startTime;
    static inline std::chrono::system_clock::time_point lastPlayed;

    // used to setup default values and vector sizes for all the save stuff
    static void recalcLists();
private:

    // gets data thats put into the saveData struct
    static void getSaveInfo();
    static void getLoadInfo();
};
