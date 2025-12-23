#pragma once

#include <string>

#include "loadingScreen.h"

class world;
class LoadingScreen;
class Image;
class animation;
class text;

enum WorldLoc {
	// don't set the position of that character when opening level
	WORLD_SET_LOC_NONE,
	// change the position of the character to the sailor when changing worlds
	WORLD_SET_LOC_SAILOR,
	// change the position of the character to the vault enterance when changing worlds
	WORLD_SET_LOC_VAULT,
	// set the character to position { 0, 0 }
	WORLD_SET_LOC_ZERO
};

class Scene {
public:
	static void openLevel(std::string worldName, WorldLoc worldChangeLoc = WORLD_SET_LOC_SAILOR, bool overrideIfInWorld = false);
	static int getWorldIndexFromName(std::string worldName);
	static std::string getPrevWorldName();
	static std::string getCurrWorldName();
	static void deferredChangeWorld();

	static void draw(Shader* shaderProgram);
	static void openLevelThread(std::string worldName, WorldLoc worldChangeLoc, bool overrideIfInWorld);

	static bool isLoading();
private:
	static inline std::atomic<bool> loadingDone = false;
	static inline std::atomic<bool> hasFinishedLoading = false;

	static inline std::string prevWorld;
	static inline std::string currWorldName;

	static inline std::unique_ptr<LoadingScreen> loadingScreen;

	static void finishedLoading();

	static inline std::string worldName;
	static inline WorldLoc worldChangeLoc;
	static inline bool overrideIfInWorld;
	static inline bool loadWorld = false;
};