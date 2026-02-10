#pragma once

#include "saveData.h"
#include "character.h"
#include "worlds.h"

class RareFish {
public:
	static bool MetRequirements(const FfishData& fishData) {
		if (!fishData.isRareFish)
			return true; // not a rare fish

		switch (fishData.id) {
		case 7: // fish school fish
			return GetCharacter()->IsFishingInSchool();
		case 8: // has to be raining
			if (world::currWorld && world::currWorld->rain)
				return world::currWorld->rain->IsRaining();
			return false;
		case 9: // river
			return GetCharacter()->IsBobberInRiver();
		}

		assert(false && "Rare fish wasn't accounted for");

		return false;
	}
};