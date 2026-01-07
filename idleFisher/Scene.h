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
	static void Init();
	static void Destructor();
	
	// isStartup is for when the application loads up for the first time and loads into a world for the first time
	static void openLevel(std::string worldName, WorldLoc worldChangeLoc = WORLD_SET_LOC_SAILOR, bool overrideIfInWorld = false, bool isStartup = false);
	static int getWorldIndexFromName(std::string worldName);
	static std::string getPrevWorldName();
	static std::string getCurrWorldName();

	static void updateShaders(float deltaTime);

	static void deferredChangeWorld();

	static void draw(Shader* shaderProgram);
	// Async
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
	// this is called when the scene starts loading for the first time for that session
	static void StartSetup();
	// this is called after the scene is loaded for the first time for that session
	static void FinishSetup();

	static void finishedLoading();
	// runs Async
	static void LoadTextures();

	static inline std::atomic<bool> loadingDone = false;
	static inline std::atomic<bool> hasFinishedLoading = false;

	static inline std::atomic<bool> loadingTexturesDone = false;
	static inline std::atomic<bool> hasFinishedLoadingTextures = false;
	static inline std::atomic<bool> hasLoadedGPUData = false;
	static inline std::atomic<bool> waitToUploadGPUdata = true;

	static inline std::string prevWorld;
	static inline std::string currWorldName;
	// world player is switching to that frame
	// needed because of the deferred load world
	static inline std::string worldName;

	static inline std::unique_ptr<LoadingScreen> loadingScreen;

	static inline WorldLoc worldChangeLoc;
	static inline bool overrideIfInWorld;
	static inline bool loadWorld = false;

	static inline std::atomic<bool> isStartup = false;

	// water stuff
	static inline float waveSpeed = 0.03f;
	static inline float waveFactor = 0;

	// player stuff
	static inline std::unique_ptr<Acharacter> character;
	static inline std::unique_ptr<Camera> camera;

public:
	static inline std::unique_ptr<Apet> pet;

private:
	static inline std::recursive_mutex mtx;
	static inline std::mutex cvMtx;
	static inline std::condition_variable cv;
};