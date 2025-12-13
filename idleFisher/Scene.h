#pragma once

#include <string>

#include "loadingScreen.h"

class world;
class LoadingScreen;
class Image;
class animation;
class text;

struct worldLoc {
	// don't set the position of that character when opening level
	static inline int None = 0;
	// change the position of the character to the sailor when changing worlds
	static inline int changeWorlds = 1;
	// change the position of the character to the vault enterance when changing worlds
	static inline int leaveVault = 2;
	// set the character to position { 0, 0 }
	static inline int zero = 3;
};

class Scene {
public:
	static void openLevel(std::string worldName, int worldChangeLoc = worldLoc::changeWorlds, bool overrideIfInWorld = false);
	static int getWorldIndexFromName(std::string worldName);
	static std::string getPrevWorldName();
	static std::string getCurrWorldName();
	static void deferredChangeWorld();

	static void draw(Shader* shaderProgram);
	static void openLevelThread(std::string worldName, int worldChangeLoc, bool overrideIfInWorld);

	static bool isLoading();
private:
	static inline std::atomic<bool> loadingDone = false;
	static inline std::atomic<bool> hasFinishedLoading = false;

	static inline std::string prevWorld;
	static inline std::string currWorldName;

	static inline std::unique_ptr<LoadingScreen> loadingScreen;

	static void finishedLoading();

	static inline std::string worldName;
	static inline int worldChangeLoc;
	static inline bool overrideIfInWorld;
	static inline bool loadWorld = false;
};