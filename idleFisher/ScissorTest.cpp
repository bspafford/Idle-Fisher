#include "ScissorTest.h"
#include "textureManager.h"
#include "stuff.h"
#include "ShaderClass.h"

void ScissorTest::Enable(vector loc, vector size, glm::vec4 clearColor) {
	Enable({ loc.x, loc.y, size.x, size.y }, clearColor);
}

void ScissorTest::Enable(Rect rect, glm::vec4 clearColor) {
	// draw here, before binding a new fbo
	textureManager::ForceGPUUpload();

	scissorStack.push(rect);

	glEnable(GL_SCISSOR_TEST);
	glScissor(rect.x * stuff::pixelSize, rect.y * stuff::pixelSize, rect.w * stuff::pixelSize, rect.h * stuff::pixelSize);

	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	glClear(GL_COLOR_BUFFER_BIT);
}

void ScissorTest::Disable() {
	// draw here, before leaving the fbo
	textureManager::ForceGPUUpload();

	scissorStack.pop();
	Rect currRect = scissorStack.empty() ? Rect{ 0.f, 0.f, stuff::screenSize.x, stuff::screenSize.y } : scissorStack.top();

	if (scissorStack.empty())
		glDisable(GL_SCISSOR_TEST);
	else
		glScissor(currRect.x, currRect.y, currRect.w, currRect.h);
}