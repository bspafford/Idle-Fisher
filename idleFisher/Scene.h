#pragma once

#include <string>

#include "loadingScreen.h"
#include "character.h"
#include "camera.h"
#include "pet.h"

class world;
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
	static void Destructor();
	
	// isStartup is for when the application loads up for the first time and loads into a world for the first time
	static void openLevel(std::string worldName, WorldLoc worldChangeLoc = WORLD_SET_LOC_SAILOR, bool overrideIfInWorld = false, bool isStartup = false);
	static int getWorldIndexFromName(std::string worldName);
	static std::string getPrevWorldName();
	static std::string getCurrWorldName();

	static void updateShaders(float deltaTime);

	static void deferredChangeWorld();

	static void draw(Shader* shaderProgram);
	static void openLevelThread(std::string worldName, WorldLoc worldChangeLoc, bool overrideIfInWorld);

	static bool isLoading();

	// shaders
	static inline Shader* shaderProgram;
	static inline Shader* shadowMapProgram;
	static inline Shader* twoDShader;
	static inline Shader* twoDWaterShader;
	static inline Shader* circleShader;
	static inline Shader* blurShader;
	static inline Shader* fishingLineShader;

private:
	static void CreateShaders();
	// load required things to make the loading screen work
	// this is called before the program moves to worker thread
	static void LoadRequired();
	// this is called when the scene starts loading for the first time for that session
	static void StartSetup();
	// this is called after the scene is loaded for the first time for that session
	static void FinishSetup();

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

	static inline bool isStartup;

	// water stuff
	static inline float waveSpeed = 0.03f;
	static inline float waveFactor = 0;

	// player stuff
	static inline std::unique_ptr<Acharacter> character;
	static inline std::unique_ptr<Camera> camera;
public:
	static inline std::unique_ptr<Apet> pet;
};