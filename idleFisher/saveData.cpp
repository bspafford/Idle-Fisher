#include "saveData.h"
#include "main.h"
#include "timer.h"
#include "Scene.h"

#include <iostream>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#endif

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

template <typename T>
void assignId(T& data, uint32_t id) {
	data.id = id;
}

template <typename T1, typename T2>
void assignId(std::pair<T1, T2>& pairedData, uint32_t id) {
	pairedData.first.id = id;
	pairedData.second.id = id;
}

template <typename T1, typename T2> static void recalcList(std::unordered_map<uint32_t, T1>& data, std::unordered_map<uint32_t, T2>& saveData) {
	// return if both lists are the same size
	if (saveData.size() == data.size())
		return;

	saveData.reserve(data.size());
	for (auto& [id, d] : data) {
		if (saveData.find(id) == saveData.end()) {
			T2 temp{};
			assignId(temp, id);
			saveData.emplace(id, std::move(temp));
		}
	}
}

void SaveData::recalcLists() {
	recalcList(data.fishData, saveData.fishData);
	recalcList(data.currencyData, saveData.currencyList);
	recalcList(data.upgradeData, saveData.upgradeList);
	recalcList(data.worldData, saveData.worldList);
	recalcList(data.mechanicStruct, saveData.mechanicStruct);
	recalcList(data.autoFisherData, saveData.autoFisherList);
	recalcList(data.petData, saveData.petList);
	recalcList(data.vaultUnlockData, saveData.vaultUnlockList);
	recalcList(data.baitData, saveData.baitList);
	recalcList(data.rebirthData, saveData.rebirthList);
	recalcList(data.achievementData, saveData.achievementList);
	recalcList(data.worldData, saveData.npcSave);
	recalcList(data.currencyData, saveData.currencyConversionList);

	saveData.worldList.at(4u).level = true; // world 1 is always unlocked
	saveData.currencyList.at(4u).unlocked = true; // currency 1 is always unlocked
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
	const auto filename = GetSettingsDataPath();

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

void SaveData::LoadData() {
	std::ifstream input(GetJsonDataPath(), std::ios::binary | std::ios::ate);
	if (!input.is_open()) {
		std::cout << "couldn't find file: " << GetJsonDataPath();
		return;
	}

	std::streamsize size = input.tellg();
	input.seekg(0, std::ios::beg);
	std::vector<uint8_t> buffer(static_cast<size_t>(size));
	input.read(reinterpret_cast<char*>(buffer.data()), buffer.size());

	nlohmann::json j_from_bytes = nlohmann::json::from_bson(buffer.begin(), buffer.end());
	SaveData::data = j_from_bytes["data"].get<Fdata>();
	SaveData::orderedData = j_from_bytes["orderedData"].get<ForderedData>();
}

std::filesystem::path SaveData::GetSaveFolder() {
#ifdef _WIN32
	PWSTR path = nullptr;
	if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &path))) {
		std::filesystem::path base(path);
		CoTaskMemFree(path);

		std::filesystem::path dir = base / stuff::studio / stuff::name;
		std::filesystem::create_directories(dir);
		return dir;
	}
#endif
	
	std::filesystem::path dir = std::filesystem::path("data") / "saves";
	std::filesystem::create_directories(dir); // create dir if not already created
	return dir; // default: store with game if cant find or not on windows
}

std::filesystem::path SaveData::GetSaveDataPath() {
	return GetSaveFolder() / "save.save";
}

std::filesystem::path SaveData::GetSettingsDataPath() {
	return GetSaveFolder() / "settings.save";
}

std::filesystem::path SaveData::GetDataPath() {
	return std::filesystem::path("data");
}

std::filesystem::path SaveData::GetJsonDataPath() {
	return GetDataPath() / "data.bson";
}

FfishData FfishData::GetCheapestFishInWorld(uint32_t worldId) {
	if (worldId == 0)
		worldId = Scene::GetCurrWorldId();

	FfishData cheapestFish;
	cheapestFish.currencyNum = std::numeric_limits<int>::max();
	for (auto& [key, value] : SaveData::data.fishData) {
		if (value.worldId == worldId) {
			if (value.currencyNum < cheapestFish.currencyNum)
				cheapestFish = value;
		}
	}
	return cheapestFish;
}