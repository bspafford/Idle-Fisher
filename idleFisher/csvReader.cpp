#include "csvReader.h"
#include "main.h"
#include "saveData.h"

#include "debugger.h"

template <typename T> void readData(std::vector<T>& a, std::string csvName);

csvReader::csvReader() {
    readData(SaveData::data.fishData, "fishDataTable");
    readData(SaveData::data.currencyData, "currencyDataTable");
    readData(SaveData::data.autoFisherData, "autoFisherDataTable");
    readData(SaveData::data.worldData, "worldDataTable");
    readData(SaveData::data.fishingRodData, "fishingRodDataTable");
    readData(SaveData::data.achievementData, "achievementDataTable");
    readData(SaveData::data.baitData, "baitDataTable");
    readData(SaveData::data.buffData, "buffDataTable");
    readData(SaveData::data.goldenFishData, "goldenFishDataTable");
    readData(SaveData::data.mechanicStruct, "mechanicDataTable");
    readData(SaveData::data.petData, "petDataTable");
    readData(SaveData::data.rebirthData, "rebirthDataTable");
    readData(SaveData::data.upgradeData, "upgradeDataTable");
    readData(SaveData::data.vaultUnlockData, "vaultUnlocksDataTable");
}

template <typename T> void readData(std::vector<T>& a, std::string csvName) {
    std::ifstream colFile("./data/dataBase/" + csvName + ".csv");
    if (colFile.is_open()) {
        std::string line, word;
        std::getline(colFile, line); // removes first line because its a descriptor
        while (colFile.good()) {
            std::string line, word;
            std::getline(colFile, line);

            std::vector<std::string> row;
            std::stringstream s(line);

            bool pOpen = false;
            std::string listString = "";
            while (std::getline(s, word, ',')) {
                if (word.size() >= 3 && word[0] == '"' && word[1] == '(') { // makes sure string looks like '"(1'
                    pOpen = true;
                    listString.append(word + ",");
                } else if (word.size() >= 3 && word[word.size() - 1] == '"' && word[word.size() - 2] == ')') { // makes sure string looks like '1)"'
                    pOpen = false;
                    listString.append(word);
                    // remove first and last char because they are "
                    listString.erase(0, 1);
                    listString.erase(listString.size() - 1, 1);

                    row.push_back(listString);
                } else if (pOpen) {
                    listString.append(word + ",");
                } else {
                    row.push_back(word);
                }
            }

            if (row.size() == 0)
                return;

            T data;
            data.parseData(row);
            a.push_back(data);
        }
    }
}