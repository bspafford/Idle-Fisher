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
#include "fishUnlocked.h"
#include "UIWidget.h"
#include "idleProfitWidget.h"
#include "comboOvertimeWidget.h"
#include "newRecordWidget.h"

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

	// shaders
	static inline Shader* shaderProgram;
	static inline Shader* shadowMapProgram;
	static inline Shader* twoDShader;
	static inline Shader* twoDWaterShader;
	static inline Shader* circleShader;
	static inline Shader* blurShader;

	// widgets
	void setupWidgets();
	static inline std::unique_ptr<UpauseMenu> pauseMenu;
	static inline std::unique_ptr<UfishComboWidget> fishComboWidget;
	static inline std::unique_ptr<UheldFishWidget> heldFishWidget;
	static inline std::unique_ptr<UcurrencyWidget> currencyWidget;
	static inline std::unique_ptr<UcomboWidget> comboWidget;
	static inline std::unique_ptr<UachievementWidget> achievementWidget;
	static inline std::unique_ptr<Ujournal> journal;
	static inline std::unique_ptr<UfishUnlocked> fishUnlocked;
	static inline std::unique_ptr<UUIWidget> UIWidget;
	static inline std::unique_ptr<UidleProfitWidget> idleProfitWidget;
	static inline std::unique_ptr<UcomboOvertimeWidget> comboOvertimeWiget;
	static inline std::unique_ptr<UnewRecordWidget> newRecordWidget;

	static inline std::unique_ptr<Apet> pet;

	static void rebirth();
	static double calcRebirthCurrency();

	static inline std::vector<UpremiumBuffWidget*> premiumBuffList;

	static void drawWidgets(Shader* shaderProgram);

	// window stuff
	static void SetFullScreen();
	static void SetVsync();
	static void SetResolution();
	static void SetFpsLimit();
	static int GetFPSLimit();

private:
	static inline GLFWwindow* window;

	void Start();
	void Update(float deltaTime);
	void updateShaders(float deltaTime);
	void draw3D(Shader* shaderProgram);
	void draw(Shader* shaderProgram);

	// window stuff
	void setTaskbarIcon(GLFWwindow* window);
	static GLFWmonitor* GetCurrentMonitor();
	static Rect GetMonitorRect();
	static void monitorCallback(GLFWmonitor* monitor, int event);

	static void windowSizeCallback(GLFWwindow* window, int width, int height);
	static void checkInputs();

	static inline std::unique_ptr<Acharacter> character;

	bool renderShadows = false;

	static inline int fpsCap = 0; // 0 is uncapped
	static inline bool running = true;

	// water stuff
	static inline float waveSpeed = 0.03f;
	static inline float waveFactor = 0;

	// 3d
	std::unique_ptr<Model> house;
	std::unique_ptr<Model> characterModel;
	static inline std::unique_ptr<Camera> camera;
};