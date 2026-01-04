#include "fishGod.h"
#include "rebirthWidget.h"
#include "main.h"

#include "debugger.h"

AfishGod::AfishGod(vector loc) : npc(loc) {
	setup("fishGod");
	
	rebirthWidget = std::make_unique<UrebirthWidget>(nullptr);

	int worldId = Scene::getWorldIndexFromName(Scene::getCurrWorldName());
	discovered = &discoveredFallback;
}

void AfishGod::click() {
	rebirthWidget->addToViewport(nullptr);
}