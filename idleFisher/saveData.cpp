#include "saveData.h"

#include <iostream>
#include <alpaca/alpaca.h>

#include "main.h"
#include "timer.h"

#include "debugger.h"

void SaveData::save() {
	// make sure the program has loaded data before saving
	// otherwise it can cause a new save to overwrite the load
	if (!hasLoaded)
		return;

	const auto filename = GetSaveDataPath();

	long long currEpochMS = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	// check if file exists
	if (!std::filesystem::exists(filename)) {
		SaveData::saveData.startTime = currEpochMS;
		startTime = std::chrono::system_clock::time_point{ std::chrono::milliseconds(SaveData::saveData.startTime) };
	}
	SaveData::saveData.lastPlayed = currEpochMS;

	// Serialize to file
	std::ofstream os;
	os.open(filename, std::ios::out | std::ios::binary);
	
	if (os.is_open()) {
		std::vector<uint8_t> bytes = nlohmann::json::to_bson(SaveData::saveData);
		os.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
		os.close();
	}
}

void SaveData::load() {
	recalcLists();

	const auto filename = GetSaveDataPath();

	// if save file doesn't exist, make one and setup default values
	if (!std::filesystem::exists(filename)) {
		hasLoaded = true;
		save();
		return;
	}

	std::ifstream is;
	is.open(filename, std::ios::in | std::ios::binary);
	if (is.is_open()) {
		// read in the whole file
		std::string file_contents(std::istreambuf_iterator<char>(is), {});
		// bytes to json
		nlohmann::json data = nlohmann::json::from_bson(file_contents);
		// json to struct
		SaveData::saveData = data.get<FsaveData>();

		is.close();
	}

	lastPlayed = std::chrono::system_clock::time_point{std::chrono::milliseconds(SaveData::saveData.lastPlayed) };
	startTime = std::chrono::system_clock::time_point{ std::chrono::milliseconds(SaveData::saveData.startTime) };

	// setup auto saving
	autoSaveTimer = CreateDeferred<Timer>();
	autoSaveTimer->addCallback(SaveData::autoSave);
	autoSaveTimer->start(autoSaveInterval);

	hasLoaded = true;
}

void SaveData::autoSave() {
	save();
	autoSaveTimer->start(autoSaveInterval);
}

template <typename T1, typename T2> static void recalcList(std::vector<T1>& data, std::vector<T2>& saveData) {
	// return if both lists are the same size
	if (saveData.size() == data.size())
		return;

	std::vector<T2> tempSaveData;
	for (int i = 0; i < data.size(); i++) {
		if (i <= (int)saveData.size() - 1) { // if saveData has that index then keep it
			tempSaveData.push_back(saveData[i]);
		} else { // else make new
			T2 temp;
			temp.id = data[i].id;
			tempSaveData.push_back(temp);
		}
	}

	saveData = tempSaveData;
}

void SaveData::recalcLists() {
	recalcList(data.fishData, saveData.fishData);
	for (int i = 0; i < saveData.fishData.size(); i++) {
		saveData.fishData[i].numOwned = std::vector<double>(4);
		saveData.fishData[i].totalNumOwned = std::vector<double>(4);
	}

	recalcList(data.currencyData, saveData.currencyList);
	recalcList(data.upgradeData, saveData.upgradeList);
	recalcList(data.worldData, saveData.worldList);
	recalcList(data.mechanicStruct, saveData.mechanicStruct);
	recalcList(data.autoFisherData, saveData.autoFisherList);
	recalcList(data.petData, saveData.petList);
	recalcList(data.vaultUnlockData, saveData.vaultUnlockList);
	recalcList(data.buffData, saveData.buffList);
	recalcList(data.rebirthData, saveData.rebirthList);
	recalcList(data.achievementData, saveData.achievementList);
	// npcSave
	recalcList(data.worldData, saveData.npcSave);
	recalcList(data.currencyData, saveData.currencyConversionList);

	saveData.worldList[0].unlocked = true; // world 1 is always unlocked
	saveData.currencyList[1].unlocked = true; // currency 1 is always unlocked
}

void SaveData::saveSettings() {
	const auto filename = GetSettingsDataPath();

	// Serialize to file
	std::ofstream os;
	os.open(filename, std::ios::out | std::ios::binary);

	if (os.is_open()) {
		std::vector<uint8_t> bytes = nlohmann::json::to_bson(SaveData::settingsData);
		os.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
		os.close();
	}
}

void SaveData::loadSettings() {
	const std::string filename = GetSettingsDataPath();

	if (!std::filesystem::exists(filename)) {
		saveSettings(); // makes save file if there isn't one
		return;
	}

	std::ifstream is;
	is.open(filename, std::ios::in | std::ios::binary);
	if (is.is_open()) {
		// read in the whole file
		std::string file_contents(std::istreambuf_iterator<char>(is), {});
		// bytes to json
		nlohmann::json data = nlohmann::json::from_bson(file_contents);
		// json to struct
		SaveData::settingsData = data.get<FsettingsData>();

		is.close();
	}
}

std::string SaveData::GetSaveDataPath() {
	return "./data/saves/save.save";
}

std::string SaveData::GetSettingsDataPath() {
	return "./data/saves/settings.save";
}