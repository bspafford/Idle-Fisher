#include "blurBox.h"
#include "Texture.h"
#include "camera.h"
#include "textureManager.h"
#include "main.h"

#include "debugger.h"

BlurBox::BlurBox(widget* parent, vector loc, vector size, int blurStrength) : widget(parent) {
	instances.push_back(this);

	this->blurStrength = blurStrength;
	
	setLocAndSize(loc, size);

	texture = std::make_unique<Texture>(stuff::screenSize);
}

BlurBox::~BlurBox() {
	auto it = std::find(instances.begin(), instances.end(), this);
	if (it != instances.end())
		instances.erase(it);
}

void BlurBox::setLoc(vector loc) {
	__super::setLoc(loc);
}

void BlurBox::setSize(vector size) {
	__super::setSize(size);
}

void BlurBox::draw() {
	Main::blurShader->Activate();
	Main::blurShader->setVec2("screenSize", glm::vec2(stuff::screenSize.x, stuff::screenSize.y));
	Main::blurShader->setInt("radius", blurStrength);
	Main::blurShader->setMat4("projection", GetMainCamera()->getProjectionMat());

	// need to render everything first
	textureManager::ForceGPUUpload();

	GenerateSubTexture(texture.get());
	textureManager::DrawImage(Main::blurShader, loc, size, { 0, 0, 1, 1 }, useWorldPos, glm::vec4(1), texture->GetHandle());
}

void BlurBox::GenerateSubTexture(Texture* texture) {
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	texture->Bind();
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, stuff::screenSize.x, stuff::screenSize.y);
	texture->Unbind();
}

void BlurBox::Init() {
	sceneFBO = std::make_unique<FBO>(stuff::screenSize / stuff::pixelSize, false);
}

void BlurBox::DrawFinal(Shader* shader) {
	glClear(GL_COLOR_BUFFER_BIT);
	shader->Activate();
	sceneFBO->Draw(shader, {0, 0}, {0, 0, 1, 1}, false, glm::vec4(1));
}

void BlurBox::BindFramebuffer() {
	sceneFBO->BindFramebuffer();
}

void BlurBox::UnbindFramebuffer() {
	sceneFBO->UnbindFramebuffer();
}

void BlurBox::OnReizeWindow() { 
	// don't need this if a create a anchor thing
	/*
	// resize the texture
	glBindTexture(GL_TEXTURE_2D, sceneTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, stuff::screenSize.x, stuff::screenSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	for (BlurBox* box : instances) {
		glBindTexture(GL_TEXTURE_2D, box->texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, stuff::screenSize.x, stuff::screenSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}
	*/
}