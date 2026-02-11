#pragma once

#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include <chrono>

#include "math.h"
#include "stuff.h"

#include "worlds.h"

#include "Image.h"
#include "pet.h"

#include "character.h"
#include "camera.h"

// widgets
#include "pauseMenu.h"
#include "fishComboWidget.h"
#include "heldFishWidget.h"
#include "currencyWidget.h"
#include "comboWidget.h"
#include "achievementWidget.h"
#include "journal.h"
#include "achievementUnlockWidget.h"
#include "UIWidget.h"
#include "idleProfitWidget.h"
#include "settings.h"
#include "premiumBuffWidget.h"

class widget;
class achievement;

class Texture;
class Shader;
class Model;
class animation;
class widget;
class AautoFisher;
class IHoverable;

struct Fcollision;

class Main {
public:
	int createWindow();
	~Main();

	static bool IsRunning();
	static GLFWwindow* GetWindow();

	// widgets
	static void setupWidgets();
	static inline std::unique_ptr<UpauseMenu> pauseMenu;
	static inline std::unique_ptr<UfishComboWidget> fishComboWidget;
	static inline std::unique_ptr<UheldFishWidget> heldFishWidget;
	static inline std::unique_ptr<UcurrencyWidget> currencyWidget;
	static inline std::unique_ptr<UcomboWidget> comboWidget;
	static inline std::unique_ptr<UachievementWidget> achievementWidget;
	static inline std::unique_ptr<Ujournal> journal;
	static inline std::unique_ptr<UachievementUnlockWidget> achievementUnlocked;
	static inline std::unique_ptr<UUIWidget> UIWidget;
	static inline std::unique_ptr<UidleProfitWidget> idleProfitWidget;
	static inline std::unique_ptr<Usettings> settingsWidget;


	static void rebirth();
	static double calcRebirthCurrency();

	static inline std::vector<std::unique_ptr<UpremiumBuffWidget>> premiumBuffList;

	static void drawWidgets(Shader* shaderProgram);

	// window stuff
	static void SetFullScreen();
	static void SetVsync();
	static void SetResolution();
	static void SetFpsLimit();
	static int GetFPSLimit();
	static void setTaskbarIcon();

private:
	static inline GLFWwindow* window;

	void Start();
	void Update(float deltaTime);
	void draw(Shader* shaderProgram);

// window stuff
	static GLFWmonitor* GetCurrentMonitor();
	static Rect GetMonitorRect();
	static void monitorCallback(GLFWmonitor* monitor, int event);

	static void windowSizeCallback(GLFWwindow* window, int width, int height);
	static void checkInputs();

	static inline int fpsCap = 0; // 0 is uncapped
	static inline bool running = true;

// 3d stuff
	// sets up things like shadow fbo, and 3d models
	void ShadowSetup();
	// draws the meshes
	void draw3D(Shader* shaderProgram);
	// draws to the fbo and draws fbo
	void DrawShadows();
	unsigned int shadowMapFBO;
	unsigned int shadowMapWidth = 2048, shadowMapHeight = 2048;
	unsigned int shadowMap;
	
	std::unique_ptr<Model> house;
	std::unique_ptr<Model> characterModel;
	
	std::unique_ptr<Audio> backgroundMusic;
};