#include "upgrades.h"
#include "main.h"
#include "character.h"
#include "petBuffs.h"
#include "baitBuffs.h"
#include "achievementBuffs.h"

void Upgrades::Init() {
	// setup modifiers per stat
	for (auto& [id, modNode] : SaveData::data.modifierData) {
		for (auto& [stat, modData] : modNode.stats) {
			modifiersPerStat[stat].insert(id);
		}
	}

	// initial calculation of all stats
	for (auto& [stat, mods] : modifiersPerStat)
		Recalculate(stat);
}

double Upgrades::Get(const StatContext& statCtx) {
	switch(statCtx.stat) {
	case Stat::FishPrice: {
		FfishData& fishData = SaveData::data.fishData.at(statCtx.id);
		double qualityMultiplier = 1 + (statCtx.value * 0.1);
		return GetBaseStat(statCtx.stat) * fishData.basePrice * qualityMultiplier;

	} case Stat::FishComboSpeed: {
		// fishing power affects fish combo speed
		FfishData& fishData = SaveData::data.fishData.at(statCtx.id);
		double qualityMultiplier = 1.0 + (statCtx.value * 0.1);
		double speed = (0.1 * GetCharacter()->GetCombo() + 0.9) * static_cast<double>(fishData.fishSpeed) * qualityMultiplier; // increases speed
		double finalSpeed = math::max(speed / GetBaseStat(statCtx.stat), 0.1); // decreases speed, and makes sure that it gives the fish a min speed, so it doesn't get stuck
		return finalSpeed;

	} case Stat::ComboMin: {
		return GetBaseStat(Stat::ComboMax) * GetBaseStat(statCtx.stat); // min is a percentage of max

	} case Stat::ComboReset: {
		double currCombo = statCtx.value;
		return std::max(currCombo / 2.f, GetBaseStat(Stat::ComboMin)); // reset is a percentage of current combo

	} case Stat::ComboIncrease: {
		double comboMax = GetBaseStat(Stat::ComboMax);
		return math::clamp(comboMax * GetBaseStat(statCtx.stat), 1.0, comboMax);

	} case Stat::PremiumCatchChance: {
		FfishData& fishData = SaveData::data.fishData.at(1u); // get premium fish data
		return fishData.probability * GetBaseStat(statCtx.stat); // increases chance to catch premium fish based on premium fish probability
	} case Stat::CatchNum: {
		double goldenFishVal = (GetBaseStat(Stat::PremiumBuff) + 1);
		return GetBaseStat(statCtx.stat) * goldenFishVal * GetCharacter()->GetCombo();
	} case Stat::GreenComboSize: {
		FfishData& fishData = SaveData::data.fishData.at(statCtx.id);
		return Upgrades::GetBaseStat(statCtx.stat) / 100.f * (Upgrades::Get(Stat::Power) / fishData.greenDifficulty) * (-0.1 * GetCharacter()->GetCombo() + 1.1);
	} case Stat::YellowComboSize: {
		FfishData& fishData = SaveData::data.fishData.at(statCtx.id);
		return Upgrades::GetBaseStat(statCtx.stat) / 100.f * (Upgrades::Get(Stat::Power) / fishData.yellowDifficulty) * (-0.1 * GetCharacter()->GetCombo() + 1.1);
	} default:
		return GetBaseStat(statCtx.stat);
	}
}

double Upgrades::Get(Stat stat) {
	return Get(StatContext(stat));
}

double Upgrades::GetBaseStat(Stat s) {
	auto it = cachedValues.find(s);
	if (it != cachedValues.end())
		return it->second;

	return Recalculate(s);
}

bool Upgrades::LevelUp(uint32_t upgradeId, int levels) {
	ProgressionNode& progressNode = SaveData::data.progressionData.at(upgradeId);
	SaveEntry& saveProgress = SaveData::saveData.progressionData.at(upgradeId);
	ModifierNode& modNode = SaveData::data.modifierData.at(upgradeId);

	// check if already at max level
	// maxLevel of 0 means infinite levels
	if (progressNode.maxLevel != 0 && saveProgress.level + levels > progressNode.maxLevel)
		return false;

	// check if there is already a cached price or not
	double newPrice = 0.0;
	if (levels == 1) { // if only upgrade 1 level, check cache
		auto it = cachedPrices.find(upgradeId);
		newPrice = it == cachedPrices.end() ? GetPrice(progressNode, saveProgress, levels) : it->second;
	} else {
		newPrice = GetPrice(progressNode, saveProgress, levels);
	}

	FsaveCurrencyStruct& currency = SaveData::saveData.currencyList.at(progressNode.worldId);

	if (currency.numOwned >= newPrice) { // if player has enough currency
		currency.numOwned -= newPrice; // remove currency
		Main::currencyWidget->updateList(); // update currency widget

		saveProgress.level += levels; // increase level
		// maxLevel of 0 means infinite levels
		if (progressNode.maxLevel != 0 && saveProgress.level > progressNode.maxLevel) // cap at max level
			saveProgress.level = progressNode.maxLevel;

		// update cached price
		cachedPrices[upgradeId] = GetPrice(progressNode, saveProgress, 1);

		// mark all modified stats as dirty
		for (auto& [id, modData] : modNode.stats)
			MarkDirty(modData.stat);
		return true;
	}
	return false;
}

double Upgrades::GetPrice(uint32_t upgradeId) {
	auto it = cachedPrices.find(upgradeId);
	if (it == cachedPrices.end()) { // if no cached price
		ProgressionNode& progressNode = SaveData::data.progressionData.at(upgradeId);
		SaveEntry& saveProgress = SaveData::saveData.progressionData.at(upgradeId);

		double price = GetPrice(progressNode, saveProgress);
		cachedPrices.insert({ upgradeId, price }); // add to cache
		return price;
	} else
		return it->second;
}

double Upgrades::GetPrice(const ProgressionNode& upgrade, SaveEntry& saveUpgrade, int levels) {
	// ((base + add * level) * multiply^level)^exponent
	double price = 0.0;
	for (int i = 1; i <= levels; ++i) // start i at 1 to skip current level price
		price += (upgrade.cost.base + upgrade.cost.add * (saveUpgrade.level + i)) * std::pow(upgrade.cost.mul, (saveUpgrade.level + i));
	return price;
}

double Upgrades::GetCached(Stat stat) {
	return cachedValues.at(stat);
}

void Upgrades::MarkDirty(Stat s) {
	if (s == Stat::None)
		return;

	dirty.insert(s);
}

void Upgrades::Update(double dt) {

}

void Upgrades::UpdateDirty() {
	for (auto stat : dirty) // loop through dirty stats
		Recalculate(stat);

	dirty.clear();
}

double Upgrades::Recalculate(Stat stat) {
	double& cachedValue = cachedValues[stat];
	for (uint32_t upgradeId : modifiersPerStat.at(stat)) { // recalculate all modifiers of stat
		ModifierNode& mod = SaveData::data.modifierData.at(upgradeId);
		SaveEntry& saveMod = SaveData::saveData.progressionData.at(mod.id);

		ModData& modData = mod.stats.at(stat);
		cachedValue += std::pow((modData.effect.base + modData.effect.add * saveMod.level) * std::pow(modData.effect.mul, saveMod.level), modData.effect.exp);
	}

	return cachedValue;
}