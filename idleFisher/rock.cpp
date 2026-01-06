#include "rock.h"

Arock::Arock(vector loc) : Abuilding(loc) {
	buildingImg = std::make_unique<Image>("images/landscape/rock.png", loc, true);

	setupCollision({ loc + vector{ 0, 5 }, loc + vector{ 11, 2 }, loc + vector{ 20, 6 }, loc + vector{ 10, 10 } }); // small rock
	setupCollision({ loc + vector{ 10, 10 }, loc + vector{ 32, 2 }, loc + vector{ 46, 7 }, loc + vector{ 26, 18 } }); // big rock
}

Arock::~Arock() {

}