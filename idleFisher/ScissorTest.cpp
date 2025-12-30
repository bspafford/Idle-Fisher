#include "ScissorTest.h"
#include "textureManager.h"
#include "stuff.h"
#include "ShaderClass.h"

void ScissorTest::Enable(vector loc, vector size) {
	Enable({ loc.x, loc.y, size.x, size.y });
}

void ScissorTest::Enable(Rect rect) {
	// draw here, before activating a new scissor test
	textureManager::ForceGPUUpload();

	scissorStack.push(rect);

	glEnable(GL_SCISSOR_TEST);
	glScissor(rect.x * stuff::pixelSize, rect.y * stuff::pixelSize, rect.w * stuff::pixelSize, rect.h * stuff::pixelSize);
}

void ScissorTest::Disable() {
	// draw here, before disabling the scissor test
	textureManager::ForceGPUUpload();

	scissorStack.pop();

	if (scissorStack.empty())
		glDisable(GL_SCISSOR_TEST);
	else {
		Rect currRect = scissorStack.top();
		glScissor(currRect.x * stuff::pixelSize, currRect.y * stuff::pixelSize, currRect.w * stuff::pixelSize, currRect.h * stuff::pixelSize);
	}
}

Rect ScissorTest::GetCurrRect() {
	return scissorStack.empty() ? Rect{ 0.f, 0.f, stuff::screenSize.x / stuff::pixelSize, stuff::screenSize.y / stuff::pixelSize } : scissorStack.top();
}