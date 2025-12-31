#include "FBO.h"
#include "textureManager.h"
#include "stuff.h"
#include "ShaderClass.h"
#include "Texture.h"
#include "ScissorTest.h"

FBO::FBO(vector size, bool useWorldPos) {
	this->size = size;

	// Create FBO
	glCreateFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);

	texture = std::make_unique<Texture>(size * stuff::pixelSize);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->GetID(), 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "FBO is incomplete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, GetCurrFBO().ID);
}

FBO::~FBO() {
	if (ID)
		glDeleteFramebuffers(1, &ID);
}

void FBO::Draw(Shader* shader, const vector& position, const Rect& source, const bool& useWorldPos, const glm::vec4& color) {
	textureManager::DrawImage(shader, position, size, source, useWorldPos, color, texture->GetHandle());
}

vector FBO::GetSize() {
	return size;
}

void FBO::Bind(glm::vec4 clearColor) {
	// draw here, before binding a new fbo
	textureManager::ForceGPUUpload();

	fboStack.push(FBOData(ID, size));

	glBindFramebuffer(GL_FRAMEBUFFER, ID);

	glViewport(0, 0, size.x * stuff::pixelSize, size.y * stuff::pixelSize);
	glScissor(0, 0, size.x * stuff::pixelSize, size.y * stuff::pixelSize);

	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	glClear(GL_COLOR_BUFFER_BIT);
}

void FBO::Unbind() {
	// draw here, before leaving the fbo
	textureManager::ForceGPUUpload();

	fboStack.pop();
	FBOData currFBO = fboStack.empty() ? FBOData(0, stuff::screenSize / stuff::pixelSize) : fboStack.top();

	// Bind to previous FBO
	glBindFramebuffer(GL_FRAMEBUFFER, currFBO.ID);

	glViewport(0, 0, currFBO.fboSize.x * stuff::pixelSize, currFBO.fboSize.y * stuff::pixelSize);

	Rect scissorRect = ScissorTest::GetCurrRect();
	glScissor(scissorRect.x * stuff::pixelSize, scissorRect.y * stuff::pixelSize, scissorRect.w * stuff::pixelSize, scissorRect.h * stuff::pixelSize);
}

FBOData FBO::GetCurrFBO() {
	return fboStack.empty() ? FBOData(0, stuff::screenSize / stuff::pixelSize) : fboStack.top();
}