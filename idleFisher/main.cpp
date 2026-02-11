#include "main.h"

#include "model.h"
#include "stuff.h"
#include "timer.h"
#include "fps.h"
#include "textureManager.h"
#include "character.h"
#include "camera.h"
#include "AautoFisher.h"
#include "collision.h"
#include "Input.h"
#include "Scene.h"
#include "Cursor.h"
#include "GPULoadCollector.h"
#include "upgrades.h"

// npc
#include "fishTransporter.h"

// widgets
#include "widget.h"
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
#include "blurBox.h"
#include "settings.h"
#include "AudioSystem.h"

#include "debugger.h"

int main(int argc, char* argv[]) {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	srand(time(0));
	math::randRange(0.f, 100.f); // steups up srand i guess? otherwise first value always the same
	Main* _main = new Main();
	_main->createWindow();

	delete _main;
	return 0;
}

Main::~Main() {
	Scene::Destructor();

	textureManager::Deconstructor();
	DeferredPtr<Timer>::BeginShutdown();

	text::Shutdown();
	AudioSystem::Shutdown();

	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
}

int Main::createWindow() {
	SaveData::loadSettings();

	// Initialize GLFW
	glfwInit();

	// Tell GLFW what version of OpenGL we are using 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	SetResolution();

	window = glfwCreateWindow(static_cast<int>(stuff::screenSize.x), static_cast<int>(stuff::screenSize.y), "Idle Fisher", NULL, NULL);
	// Error check if the window fails to create
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	SetVsync();
	SetFpsLimit();

	// set window pos based on saved monitor
	Rect monitorRect = GetMonitorRect();
	glfwSetWindowPos(window, monitorRect.x, monitorRect.y);

	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	//Load GLAD so it configures OpenGL
	if (!gladLoadGL())
		return -1;

	glViewport(0, 0, static_cast<int>(stuff::screenSize.x), static_cast<int>(stuff::screenSize.y));

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Start
	Start();

	auto lastTime = std::chrono::steady_clock::now();
	
	// Main while loop
	while (!glfwWindowShouldClose(window)) {
		auto currentTime = std::chrono::steady_clock::now();
		float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();

		if (fpsCap != 0 && deltaTime < 1.f / fpsCap)
			continue;

		lastTime = currentTime;

		//fps::showFPS(true);
		//fps::update(deltaTime);

		// process input
		Input::pollEvents();

		textureManager::StartFrame();

		checkInputs();
		Update(deltaTime);

		glClearColor(18.f / 255.f, 11.f / 255.f, 22.f / 255.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ==== DRAW 2D STUFF ====
		Scene::twoDShader->Activate();

		BlurBox::BindFramebuffer();
		draw(Scene::twoDShader);
		BlurBox::UnbindFramebuffer();
		BlurBox::DrawFinal(Scene::twoDShader);

		textureManager::EndFrame();
		Upgrades::UpdateDirty();

		DrawShadows();

		Input::fireHeldInputs();
		Scene::deferredChangeWorld();

		glfwSwapBuffers(window);

#ifdef _DEBUG
		// checks for errors
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR)
			std::cout << "OpenGL Error: " << err << std::endl;
#endif
	}

	// huge problem if player closes game before it loads the data
	SaveData::save(); // when closing window

	running = false;
	return 0;
}

void Main::Start() {
	GPULoadCollector::setMainThread(std::this_thread::get_id());

	// setup callbacks for input
	glfwSetFramebufferSizeCallback(window, windowSizeCallback);
	glfwSetKeyCallback(window, Input::keyCallback);
	glfwSetMouseButtonCallback(window, Input::mouseButtonCallback);
	glfwSetScrollCallback(window, Input::scrollCallback);
	glfwSetCursorPosCallback(window, Input::cursorPosCallback);
	glfwSetMonitorCallback(monitorCallback);

	textureManager();
	setTaskbarIcon();

	text::Init();
	AudioSystem::Init();

	Scene::Init();
	Scene::openLevel(50u, WORLD_SET_LOC_NONE, true, true);
	Scene::deferredChangeWorld();
	
	fps::fps();

	BlurBox::Init();

	ShadowSetup();

	collision::Init();

	backgroundMusic = std::make_unique<Audio>("music.wav", AudioType::Music);
	backgroundMusic->Play(true);
}

void Main::Update(float deltaTime) {
	Timer::callUpdate(deltaTime);

	Scene::updateShaders(deltaTime);
	
	Cursor::calcMouseImg();

	if (!Scene::isLoading() && world::currWorld) {
		GetCharacter()->Update(deltaTime);
		GetMainCamera()->Update(window, deltaTime);

		collision::testMouse(Input::getMousePos());

		for (int i = 0; i < world::currWorld->autoFisherList.size(); i++)
			world::currWorld->autoFisherList[i]->Update(deltaTime);
		if (fishComboWidget)
			fishComboWidget->Update(deltaTime);
		if (world::currWorld->fishTransporter)
			world::currWorld->fishTransporter->update(deltaTime);
		if (Scene::pet)
			Scene::pet->update(deltaTime);
	}
}

void Main::setupWidgets() {
	pauseMenu = std::make_unique<UpauseMenu>(nullptr);
	fishComboWidget = std::make_unique<UfishComboWidget>(nullptr);
	heldFishWidget = std::make_unique<UheldFishWidget>(nullptr);
	heldFishWidget->setLoc({ 6.f, -6.f });
	heldFishWidget->updateList(true);
	currencyWidget = std::make_unique<UcurrencyWidget>(nullptr);
	currencyWidget->updateList();
	comboWidget = std::make_unique<UcomboWidget>(nullptr);
	achievementWidget = std::make_unique<UachievementWidget>(nullptr);
	journal = std::make_unique<Ujournal>(nullptr);
	achievementUnlocked = std::make_unique<UachievementUnlockWidget>(nullptr);
	UIWidget = std::make_unique<UUIWidget>(nullptr);
	idleProfitWidget = std::make_unique<UidleProfitWidget>(nullptr);
	if (!settingsWidget)
		settingsWidget = std::make_unique<Usettings>(nullptr);
}

void Main::ShadowSetup() {
	return; // to do

	// Framebuffer for Shadow Map
	glCreateFramebuffers(1, &shadowMapFBO);
	// Texture for Shadow Map FBO
	glCreateTextures(GL_TEXTURE_2D, 1, &shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	// Prevents darkness outside the frustrum
	float clampColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
	// Needed since we don't touch the color buffer
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Load in a model for shadows
	house = std::make_unique<Model>("models/idleFisher3D/idleFisher3DNoWater.gltf");
	house->setPos(glm::vec3(-182.75f, 0.f, -504.5f));
	house->setScale(glm::vec3(1.89f));
	house->setColor(glm::vec3(255, 240, 240));
	characterModel = std::make_unique<Model>("models/character/character.gltf");

	characterModel->setScale(glm::vec3(3.f));
}

void Main::DrawShadows() {
	return; // to do

	//glEnable(GL_DEPTH_TEST);

	// Compute full light-space matrix
	float size = 300.f; // 100.f;// 35
	glm::vec3 lightPos = glm::vec3(-30.f, 100.f, 30.f) + GetMainCamera()->GetPosition() - glm::vec3(52.7046, 24.8073, 88.9249); // should follow camera pos
	glm::mat4 lightProjection = glm::ortho(-size, size, -size, size, -1.f, 300.0f);
	glm::mat4 lightView = glm::lookAt(lightPos, lightPos - glm::vec3(-1, 1, 1), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightSpaceMatrix = lightProjection * lightView; // This is what you send to shaders

	// === SHADOW PASS (Render to Shadow Map) ===
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, shadowMapWidth, shadowMapHeight);

	Scene::shadowMapProgram->Activate();
	Scene::shadowMapProgram->setMat4("lightSpaceMatrix", lightSpaceMatrix);

	// Render objects (WITHOUT camera)
	draw3D(Scene::shadowMapProgram);
	characterModel->Draw(Scene::shadowMapProgram, *GetMainCamera());

	// Unbind FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, static_cast<int>(stuff::screenSize.x), static_cast<int>(stuff::screenSize.y));

	characterModel->setPos(GetMainCamera()->GetPosition() + glm::vec3(-1.f, -.82f, -1.f) * glm::vec3(62.5f) + glm::vec3(9, 0, 9));


	Scene::shaderProgram->Activate();
	Scene::shaderProgram->setMat4("lightSpaceMatrix", lightSpaceMatrix);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	Scene::shaderProgram->setInt("shadowMap", 1);
	Scene::shaderProgram->setInt("shadowOnly", 1);
	draw3D(Scene::shaderProgram);

	//glDisable(GL_DEPTH_TEST);
}

void Main::draw3D(Shader* shaderProgram) {
	house->Draw(shaderProgram, *GetMainCamera());
}

void Main::draw(Shader* shaderProgram) {
	shaderProgram->Activate();
	Scene::draw(shaderProgram);

	if (achievementUnlocked)
		achievementUnlocked->draw(shaderProgram);

	// draw collision
	//collision::showCollisionBoxes(Scene::lineShader);
}

void Main::windowSizeCallback(GLFWwindow* window, int width, int height) {
	stuff::screenSize = { float(width), float(height) };
	glViewport(0, 0, width, height);

	widget::resizeScreen();
	BlurBox::ResizeScreen();
	if (world::currWorld && world::currWorld->grass)
		world::currWorld->grass->ResizeScreen();
}
void Main::checkInputs() {
	if (Scene::isLoading())
		return;

	if (Input::getKeyDown(GLFW_KEY_ESCAPE)) {
		if (widget::getCurrWidget())
			widget::getCurrWidget()->removeFromViewport();
		else if (Scene::GetCurrWorldId() != 50u) // title screen
			pauseMenu->addToViewport(nullptr);
	}

	uint32_t currWorld = Scene::GetCurrWorldId();
	if (Input::getKeyDown(GLFW_KEY_C) && currWorld != 50u) // title screen
		achievementWidget->addToViewport(nullptr);
	if (Input::getKeyDown(GLFW_KEY_V) && currWorld != 50u) // title screen
		journal->addToViewport(nullptr);


#ifdef _DEBUG // only for debug testing
	if (Input::getKeyDown(GLFW_KEY_K))
		SaveData::save();
	if (Input::getKeyDown(GLFW_KEY_J)) {
		SaveData::saveData.currencyList.at(53u).numOwned += 1e100;
		SaveData::saveData.currencyList.at(53u).totalNumOwned += 1e100;

		/*for (auto& [currencyId, currencyData] : SaveData::saveData.currencyList) {
			currencyData.numOwned += 1000;
			currencyData.totalNumOwned += 1000;
			currencyData.unlocked = true;
		}*/
		currencyWidget->updateList();
	}
	if (Input::getKeyDown(GLFW_KEY_L)) {
		SaveData::saveData.currencyList.at(53u).numOwned = 0;
		SaveData::saveData.currencyList.at(53u).totalNumOwned = 0;
		currencyWidget->updateList();
	}

	if (Input::getKeyDown(GLFW_KEY_O))
		Scene::openLevel(52u, WORLD_SET_LOC_SAILOR, false);
#endif
}

void Main::drawWidgets(Shader* shaderProgram) {
	if (widget::getCurrWidget() == pauseMenu.get())
		return;

	fishComboWidget->draw(shaderProgram);
	heldFishWidget->draw(shaderProgram);
	currencyWidget->draw(shaderProgram);
	comboWidget->draw(shaderProgram);
	UIWidget->draw(shaderProgram);
	GetCharacter()->DrawWidgets(shaderProgram);
}

void Main::rebirth() {
	// resest all progress
	// gives rebirth currency and total rebirth currency
	double addedRebirth = calcRebirthCurrency();
	SaveData::saveData.rebirthCurrency += addedRebirth;
	SaveData::saveData.totalRebirthCurrency += addedRebirth;

	// reset stuff
	vector playerLoc = { 200, -84 };

	// reset fish list
	for (auto& fish : SaveData::saveData.fishData)
		fish.second.numOwned = std::vector<double>(4); // resets list to all 0s

	SaveData::saveData.currencyList.clear();
	SaveData::saveData.progressionData.clear();
	SaveData::saveData.autoFisherList.clear();
	SaveData::saveData.equippedPetId = 0;
	SaveData::saveData.equippedBaitId = 0;

	Scene::pet = nullptr;

	SaveData::recalcLists();

	if (widget::getCurrWidget())
		widget::getCurrWidget()->removeFromViewport();
	if (world::currWorld)
		world::currWorld->autoFisherList.clear();
	heldFishWidget->updateList(true);
	currencyWidget->updateList();
	Scene::openLevel(53u, WORLD_SET_LOC_SAILOR, true);
}

double Main::calcRebirthCurrency() {
	double totalPoints = 0; // total amount of points, calculated based on fish
	for (auto& currency : SaveData::saveData.currencyList)
		totalPoints += currency.second.totalNumOwned; // fix, make it so it counts based on value later

	double n = pow(SaveData::saveData.totalRebirthCurrency, double(3)) * double(10);			// how many points needed for each level (level 1 = 10, level 2 = 80...)
	double tempRebirthCurrency = std::cbrt((totalPoints + n) / double(10));						// returns level with input of currency
	double pointsToNextRebirth = pow(floor(tempRebirthCurrency + double(1)), double(3)) * double(10); // points needed to get to next level
	double lastRebirthNum = pow(floor(tempRebirthCurrency), double(3)) * double(10);		// how many points the last level took so pretty much y = f(x-1)

	double progress = (totalPoints + n - lastRebirthNum) / (pointsToNextRebirth - lastRebirthNum);
	UIWidget->updateProgressBar(tempRebirthCurrency - SaveData::saveData.totalRebirthCurrency, progress, 0);

	return floor(tempRebirthCurrency - SaveData::saveData.totalRebirthCurrency);
}

void Main::setTaskbarIcon() {
	const int iconsNum = 2;
	GLFWimage iconImgs[iconsNum];
	std::vector<std::string> paths = { "icon24", "icon16" };
	std::vector<std::vector<uint8_t>> flippedList(iconsNum); // so it doesn't leave memory before I finish looping
	for (int i = 0; i < iconsNum; i++) {
		textureStruct* texture = textureManager::getTexture("images/icons/" + paths[i] + ".png");
		flippedList.push_back(texture->GetFlippedBytes());
		iconImgs[i].width = texture->w;
		iconImgs[i].height = texture->h;
		iconImgs[i].pixels = flippedList.back().data();
	}
	glfwSetWindowIcon(window, iconsNum, iconImgs);

	// polls events to update the taskbar icon
	// otherwise something like loading all the textures will cause too long of a hold
	glfwPollEvents();
}

GLFWmonitor* Main::GetCurrentMonitor() {
	int count;
	GLFWmonitor** monitors = glfwGetMonitors(&count); // for now just returns primary monitor
	if (SaveData::settingsData.monitorIdx < 0 || SaveData::settingsData.monitorIdx >= count)
		SaveData::settingsData.monitorIdx = 0; // set to primary default

	return monitors[SaveData::settingsData.monitorIdx];
}

Rect Main::GetMonitorRect() {
	int xPos, yPos, width, height;
	int count;
	GLFWmonitor* monitors = GetCurrentMonitor();
	glfwGetMonitorPos(monitors, &xPos, &yPos);
	const GLFWvidmode* mode = glfwGetVideoMode(monitors);
	stuff::screenSize = { static_cast<float>(mode->width), static_cast<float>(mode->height) };
	return Rect{ static_cast<float>(xPos), static_cast<float>(yPos), stuff::screenSize.x, stuff::screenSize.y };
}

void Main::SetFullScreen() {
	// Get the primary monitor and its current video mode
	GLFWmonitor* monitor = GetCurrentMonitor();
	Rect monitorRect = GetMonitorRect();

	switch (SaveData::settingsData.fullScreen) {
		case 0: // fullscreen
			glfwSetWindowMonitor(window, monitor, monitorRect.x, monitorRect.y, monitorRect.w, monitorRect.h, GLFW_DONT_CARE);
			break;
		case 1: // borderless
			glfwSetWindowMonitor(window, NULL, monitorRect.x, monitorRect.y, monitorRect.w, monitorRect.h, GLFW_DONT_CARE);
			break;
		case 2: // windowed
			glfwSetWindowMonitor(window, NULL, monitorRect.x + 20, monitorRect.y + 20, 800, 600, GLFW_DONT_CARE);
			break;
	}
}

void Main::SetVsync() {
	fpsCap = SaveData::settingsData.vsync ? glfwGetVideoMode(GetCurrentMonitor())->refreshRate : 0;
}

void Main::SetResolution() {
	switch(SaveData::settingsData.resolution) {
		case RES_NATIVE: {
			Rect monitorRect = GetMonitorRect();
			stuff::screenSize = { monitorRect.w, monitorRect.h };
			break;
		}
		case RES_1280x720:
			stuff::screenSize = { 1280, 720 };
			break;
		case RES_1920x1080:
			stuff::screenSize = { 1920, 1080 };
			break;
		case RES_2560x1440:
			stuff::screenSize = { 2560, 1440 };
			break;
	}

	if (window)
		glfwSetWindowSize(window, static_cast<int>(stuff::screenSize.x), static_cast<int>(stuff::screenSize.y));
}

void Main::SetFpsLimit() {
	if (SaveData::settingsData.vsync)
		return;

	switch (SaveData::settingsData.fpsLimit) {
	case 0:
		fpsCap = 0;
		break;
	case 1:
		fpsCap = 1;
		break;
	case 2:
		fpsCap = 30;
		break;
	case 3:
		fpsCap = 60;
		break;
	case 4:
		fpsCap = 120;
		break;
	case 5:
		fpsCap = 240;
		break;
	}
}

int Main::GetFPSLimit() {
	return fpsCap;
}

void Main::monitorCallback(GLFWmonitor* monitor, int event) {
	if (event == GLFW_CONNECTED || event == GLFW_DISCONNECTED)
		settingsWidget->getAllMonitors();
}

bool Main::IsRunning() {
	return running;
}

GLFWwindow* Main::GetWindow() {
	return window;
}