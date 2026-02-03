#include "upgrades.h"
#include "main.h"
#include "character.h"

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
	case Stat::None: {
		std::cout << "someone called none\n";
		abort();
		return 0.0;
	} case Stat::FishPrice: {
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

	} case Stat::ComboMax: {
		bool isUnlocked = SaveData::saveData.progressionData.at(4u).level; // is combo unlocked
		if (!isUnlocked)
			return 1.f; // max combo is 1 if not unlocked
		return GetBaseStat(Stat::ComboMax); // will return 2 at level 0 since base is 2
	} case Stat::ComboMin: {
		return math::max(GetBaseStat(Stat::ComboMax) * GetBaseStat(statCtx.stat), 1.0); // min is a percentage of max

	} case Stat::ComboReset: {
		double currCombo = statCtx.value;
		return std::max(currCombo / 2.f, GetBaseStat(Stat::ComboMin)); // reset is a percentage of current combo

	} case Stat::ComboIncrease: {
		double comboMax = GetBaseStat(Stat::ComboMax);
		return math::clamp(comboMax * GetBaseStat(statCtx.stat), 1.0, comboMax);

	} case Stat::PremiumCatchChance: {
		FfishData& fishData = SaveData::data.fishData.at(1u); // get premium fish data
		return fishData.probability * (GetBaseStat(statCtx.stat) + 1); // increases chance to catch premium fish based on premium fish probability

	} case Stat::PremiumCoolDownTime: {
		double base = 15 * 60; // temp
		return base;

	} case Stat::CatchNum: {
		double goldenFishVal = Get(Stat::PremiumBuff);

		double multiplier = 1.f;
		if (GetCharacter()->IsFishingInSchool())
			multiplier *= 1.25f;

		return GetBaseStat(statCtx.stat) * goldenFishVal * GetCharacter()->GetCombo() * multiplier;

	} case Stat::PremiumBuff: {
		// calc all active premium buffs
		double multiplier = GetBaseStat(statCtx.stat) + 1.0; // base value 1
		for (auto& premiumBuff : Main::premiumBuffList)
			multiplier *= premiumBuff->getGoldenFish().comboMultiplier;
		return multiplier;

	} case Stat::GreenComboSize: {
		FfishData& fishData = SaveData::data.fishData.at(statCtx.id);
		return (Upgrades::GetBaseStat(statCtx.stat) + 1) / 100.f * (Upgrades::Get(Stat::Power) / fishData.greenDifficulty) * (-0.1 * GetCharacter()->GetCombo() + 1.1);

	} case Stat::YellowComboSize: {
		FfishData& fishData = SaveData::data.fishData.at(statCtx.id);
		return (Upgrades::GetBaseStat(statCtx.stat) + 1) / 100.f * (Upgrades::Get(Stat::Power) / fishData.yellowDifficulty) * (-0.1 * GetCharacter()->GetCombo() + 1.1);

	} case Stat::RecastChainChance: {
		return 50.0; // temp will change once added more upgrades

	} case Stat::RecastFalloff: {
		return 0.5; // temp will change once added more upgrades

	} case Stat::AutoFisherSpeed: {
		return -(1.f / 1100.f) * SaveData::saveData.progressionData.at(statCtx.id).level + 0.100909f; // default

	} case Stat::AutoFisherMaxCapacity: {
		return SaveData::saveData.progressionData.at(statCtx.id).level * 100; // default

	} case Stat::AutoFisherPower: {
		return (SaveData::saveData.progressionData.at(statCtx.id).level / 10 + 1) * 10; // every ten levels increase power by 10

	} default:
		return GetBaseStat(statCtx.stat);
	}
}

double Upgrades::Get(Stat stat) {
	return Get(StatContext(stat));
}

double Upgrades::GetBaseStat(Stat s) {
	// make sure the cachedValue isn't dirty first
	auto dirtyIt = dirty.find(s);
	if (dirtyIt != dirty.end()) { // if stat in set
		dirty.erase(dirtyIt); // remove from dirty set
		return Recalculate(s); // recalculate and return
	}

	auto it = cachedValues.find(s);
	if (it != cachedValues.end())
		return it->second;

	return Recalculate(s);
}

bool Upgrades::LevelUp(uint32_t upgradeId, int levels, int* calculatedLevel) {
	int calcLevel = 1; // 1 by default

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
		newPrice = it == cachedPrices.end() ? GetRecalculatedPrice(progressNode, saveProgress) : it->second;
	} else {
		newPrice = GetPrice(progressNode, saveProgress, levels, &calcLevel);
		if (calculatedLevel) *calculatedLevel = calcLevel;
	}

	FsaveCurrencyStruct& currency = SaveData::saveData.currencyList.at(progressNode.worldId);

	if (currency.numOwned >= newPrice) { // if player has enough currency
		currency.numOwned -= newPrice; // remove currency
		Main::currencyWidget->updateList(); // update currency widget

		saveProgress.level += calcLevel; // increase level
		// maxLevel of 0 means infinite levels
		if (progressNode.maxLevel != 0 && saveProgress.level > progressNode.maxLevel) // cap at max level
			saveProgress.level = progressNode.maxLevel;

		// update cached price
		GetRecalculatedPrice(progressNode, saveProgress);

		// mark all modified stats as dirty
		for (auto& [id, modData] : modNode.stats)
			MarkDirty(modData.stat);
		return true;
	}
	return false;
}

double Upgrades::GetPrice(uint32_t upgradeId, int levels, int* calculatedLevel) {
	if (levels == 1) { // only cache if level == 1
		if (calculatedLevel) *calculatedLevel = levels;

		auto it = cachedPrices.find(upgradeId);
		if (it != cachedPrices.end()) { // was in cache
			return it->second;
		} else { // wasn't in cache
			return GetRecalculatedPrice(upgradeId);
		}
	} else { // -1 or >1
		ProgressionNode& progressNode = SaveData::data.progressionData.at(upgradeId);
		SaveEntry& saveProgress = SaveData::saveData.progressionData.at(upgradeId);
		return GetPrice(progressNode, saveProgress, levels, calculatedLevel);
	}
}

// ((base + add * level) * multiply^level)^exponent
double Upgrades::GetPrice(const ProgressionNode& upgrade, SaveEntry& saveUpgrade, int levels, int* calculatedLevel) {
	if (levels == 1) { // only levels == 1 is cached
		// get the cached value
		if (calculatedLevel) *calculatedLevel = levels;

		auto it = cachedPrices.find(upgrade.id);
		if (it != cachedPrices.end())
			return cachedPrices.at(upgrade.id);
		else
			return GetRecalculatedPrice(upgrade.id);
	} else if (levels > 1) {
		double price = 0.0;
		for (int i = 1; i <= levels; ++i) // start i at 1 to skip current level price
			// subtracting 1 so the base value is equal to what's in the data table
			price += PriceEquation(upgrade.cost, saveUpgrade.level, i);
		if (calculatedLevel) *calculatedLevel = levels;
		return price;
	} else if (levels == -1) {
		double currCurrency = SaveData::saveData.currencyList.at(upgrade.worldId).numOwned;
		int newLevel = 0;
		double price = 0.0;
		double levelPrice = 0.0;
		while (price < currCurrency) {
			++newLevel;
			levelPrice = PriceEquation(upgrade.cost, saveUpgrade.level, newLevel);
			price += levelPrice;
			// break on max level
			// do this after the calculations, cause it will just decrease afterwards
			if (newLevel > upgrade.maxLevel)
				break;
		}
		price -= levelPrice; // go back a level so it isn't over currency num
		--newLevel; // -1 so it isn't over currency num
		if (newLevel <= 0) {
			newLevel = 1; // set min level
			price = GetPrice(upgrade, saveUpgrade); // default price
		}

		if (calculatedLevel) *calculatedLevel = newLevel;
		return price;
	} else {
		std::cout << levels << " is not a valid level\n";
		abort();
	}
}

void Upgrades::MarkDirty(Stat stat) {
	if (stat == Stat::None)
		return;

	dirty.insert(stat);
}

void Upgrades::MarkDirty(const std::unordered_map<Stat, ModData> stats) {
	for (auto& [stat, modData] : stats)
		MarkDirty(stat);
}

void Upgrades::Update(double dt) {

}

void Upgrades::UpdateDirty() {
	for (auto stat : dirty) // loop through dirty stats
		Recalculate(stat);

	dirty.clear();
}

double Upgrades::Recalculate(Stat stat) {
	auto [it, inserted] = modifiersPerStat.emplace(stat, std::unordered_set<uint32_t>()); // adds empty
	if (inserted) // debug
		std::cout << "Stat: '" << static_cast<int>(stat) << "' was not in ModifiersPerStat map, but adding anyways\n";

	double cachedValue = 0.0;
	for (uint32_t upgradeId : it->second) { // recalculate all modifiers of stat
		ModifierNode& mod = SaveData::data.modifierData.at(upgradeId);
		SaveEntry& saveProgressNode = SaveData::saveData.progressionData.at(mod.id);

		if (!IsModifierActive(mod))
			continue; // continue if modifier is not active

		ModData& modData = mod.stats.at(stat);

		double value = std::pow((modData.effect.base + modData.effect.add * saveProgressNode.level) * std::pow(modData.effect.mul, saveProgressNode.level), modData.effect.exp);

		if (modData.buffType == ModifierType::Buff)
			cachedValue += value;
		else if (modData.buffType == ModifierType::Debuff)
			cachedValue -= value;
	}

	cachedValues[stat] = cachedValue; // add updated value to cache
	return cachedValue;
}

double Upgrades::GetRecalculatedPrice(uint32_t upgradeId) {
	ProgressionNode& data = SaveData::data.progressionData.at(upgradeId);
	SaveEntry& saveData = SaveData::saveData.progressionData.at(upgradeId);
	return GetRecalculatedPrice(data, saveData);
}

double Upgrades::GetRecalculatedPrice(const ProgressionNode& upgrade, const SaveEntry& saveUpgrade) {
	double price = PriceEquation(upgrade.cost, saveUpgrade.level, 1);
	cachedPrices[upgrade.id] = price;
	return price;
}

bool Upgrades::IsModifierActive(const ModifierNode& modifier) {
	switch (modifier.activation) {
	case ModifierActivation::Always:
		return true;
	case ModifierActivation::Equipped:
		return SaveData::saveData.equippedBaitId == modifier.id ||
			SaveData::saveData.equippedPetId == modifier.id;
	}
}

double Upgrades::PriceEquation(ScalingFormula formula, int currLevel, int levelIncrease) {
	return std::pow((formula.base + formula.add * (currLevel - 1 + levelIncrease)) * std::pow(formula.mul, (currLevel - 1 + levelIncrease)), formula.exp);
}