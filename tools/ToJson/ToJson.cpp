#include "saveData.h"

#include <fstream>

template <typename T> void readData(std::unordered_map<uint32_t, T>* a, std::vector<uint32_t>* orderedData, std::string csvName);
void readData(std::vector<uint32_t>* orderedData, std::string csvName);
void ReadCSV(Fdata& data, ForderedData& orderedData);
void OutputToJson(Fdata& data, ForderedData& orderedData);

int main() {
    Fdata saveData;
    ForderedData orderedData;
    ReadCSV(saveData, orderedData);
    OutputToJson(saveData, orderedData);
}

void ReadCSV(Fdata& data, ForderedData& orderedData) {
	readData(&data.progressionData, nullptr, "progressionDataTable");
    readData(&data.fishData, &orderedData.fishData, "fishDataTable");
    readData(&data.currencyData, &orderedData.currencyData, "currencyDataTable");
    readData(&data.autoFisherData, &orderedData.autoFisherData, "autoFisherDataTable");
    readData(&orderedData.worldData, "worldDataTable");
    readData(&data.fishingRodData, nullptr, "fishingRodDataTable");
    readData(&data.achievementData, &orderedData.achievementData, "achievementDataTable");
    readData(&data.baitData, &orderedData.baitData, "baitDataTable");
    readData(&data.goldenFishData, &orderedData.goldenFishData, "goldenFishDataTable");
    readData(&data.mechanicStruct, &orderedData.mechanicStruct, "mechanicDataTable");
    readData(&data.petData, &orderedData.petData, "petDataTable");
    readData(&data.rebirthData, &orderedData.rebirthData, "rebirthDataTable");
    readData(&data.upgradeData, &orderedData.upgradeData, "upgradeDataTable");
    readData(&data.vaultUnlockData, &orderedData.vaultUnlockData, "vaultUnlocksDataTable");
}

void OutputToJson(Fdata& data, ForderedData& orderedData) {
    std::filesystem::path path("../../idleFisher/data/data.bson");

    nlohmann::json json;
    json["data"] = data;
    json["orderedData"] = orderedData;
    std::cout << json << "\n";
    std::vector<uint8_t> bytes = nlohmann::json::to_bson(json);
    std::ofstream output(path, std::ios::binary);
    if (output.is_open()) {
        output.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
        output.close();
    }
}

template <typename T>
void parseData(T& data, std::vector<std::string> row) {
    data.parseData(row);
}

void readData(std::vector<uint32_t>* orderedData, std::string csvName) {
    std::ifstream colFile("../../idleFisher/data/debugData/dataBases/" + csvName + ".csv");
    if (colFile.is_open()) {
        std::string line, word;
        std::getline(colFile, line); // removes first line because its a descriptor
        while (colFile.good()) {
            std::getline(colFile, line);

            uint32_t id = std::stoul(line);
            orderedData->push_back(id);
        }
    }
}

template <typename T> void readData(std::unordered_map<uint32_t, T>* a, std::vector<uint32_t>* orderedData, std::string csvName) {
    std::ifstream colFile("../../idleFisher/data/debugData/dataBases/" + csvName + ".csv");
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

            // if starts with "//" then skip
            if (row[0].starts_with("//"))
                continue;

            T data;
            parseData(data, row);
            if (a)
                a->insert({ data.id, data });
            if (orderedData)
                orderedData->push_back(data.id);
        }
    }
}