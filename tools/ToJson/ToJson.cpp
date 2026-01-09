#include "saveData.h"

#include <fstream>

template <typename T> void readData(std::vector<T>& a, std::string csvName);
void ReadCSV(Fdata& data);
void OutputToJson(Fdata& data);

int main() {
    Fdata saveData;
    ReadCSV(saveData);
    OutputToJson(saveData);
}

void ReadCSV(Fdata& data) {
    readData(data.fishData, "fishDataTable");
    readData(data.currencyData, "currencyDataTable");
    readData(data.autoFisherData, "autoFisherDataTable");
    readData(data.worldData, "worldDataTable");
    readData(data.fishingRodData, "fishingRodDataTable");
    readData(data.achievementData, "achievementDataTable");
    readData(data.baitData, "baitDataTable");
    readData(data.buffData, "buffDataTable");
    readData(data.goldenFishData, "goldenFishDataTable");
    readData(data.mechanicStruct, "mechanicDataTable");
    readData(data.petData, "petDataTable");
    readData(data.rebirthData, "rebirthDataTable");
    readData(data.upgradeData, "upgradeDataTable");
    readData(data.vaultUnlockData, "vaultUnlocksDataTable");
}

void OutputToJson(Fdata& data) {
    std::filesystem::path path("../../idleFisher/data/data.bson");

    nlohmann::json json = data;
    std::cout << json << "\n";
    std::vector<uint8_t> bytes = nlohmann::json::to_bson(json);
    std::ofstream output(path, std::ios::binary);
    if (output.is_open()) {
        output.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
        output.close();
    }
}

template <typename T> void readData(std::vector<T>& a, std::string csvName) {
    std::ifstream colFile("../../idleFisher/data/debug/dataBases/" + csvName + ".csv");
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