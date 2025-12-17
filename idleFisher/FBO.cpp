#include "FBO.h"
#include "textureManager.h"
#include "stuff.h"
#include "ShaderClass.h"

FBO::FBO(vector size, bool useWorldPos) {
	// Create FBO
	glCreateFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);

	if (handle) // delete because resizing/changing image
		glMakeTextureHandleNonResidentARB(handle);
	glDeleteTextures(1, &textID);

	glCreateTextures(GL_TEXTURE_2D, 1, &textID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textID);

	glTextureStorage2D(textID, 1, GL_RGBA8, size.x, size.y);

	handle = glGetTextureSamplerHandleARB(textID, textureManager::GetSamplerID());
	glMakeTextureHandleResidentARB(handle);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textID, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "FBO is incomplete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, GetCurrFBO());
}

FBO::~FBO() {
	if (handle)
		glMakeTextureHandleNonResidentARB(handle);
	if (ID)
		glDeleteFramebuffers(1, &ID);
	if (textID)
		glDeleteTextures(1, &textID);
}

void FBO::Draw(Shader* shader, const vector& position, const vector& size, const Rect& source, const bool& useWorldPos, const glm::vec4& color) {
	textureManager::DrawImage(shader, position, size, source, useWorldPos, glm::vec4(1), handle);
}

vector FBO::GetSize() {
	return size;
}

void FBO::BindFramebuffer(glm::vec4 clearColor) {
	// draw here, before binding a new fbo
	textureManager::FBOChanged();

	fboStack.push(FBOData(ID, size));

	glBindFramebuffer(GL_FRAMEBUFFER, ID);

	glViewport(0, 0, size.x, size.y);
	glScissor(0, 0, size.x, size.y);

	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	glClear(GL_COLOR_BUFFER_BIT);
}

void FBO::UnbindFramebuffer() {
	// draw here, before leaving the fbo
	textureManager::FBOChanged();

	fboStack.pop();
	FBOData currFBO = fboStack.empty() ? FBOData(0, stuff::screenSize) : fboStack.top();

	// Bind to previous FBO
	glBindFramebuffer(GL_FRAMEBUFFER, currFBO.ID);

	glViewport(0, 0, currFBO.fboSize.x, currFBO.fboSize.y);
	glScissor(0, 0, currFBO.fboSize.x, currFBO.fboSize.y);
}

GLuint FBO::GetCurrFBO() {
	return fboStack.empty() ? 0 : fboStack.top().ID;
}