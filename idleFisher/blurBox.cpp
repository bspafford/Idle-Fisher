#include "blurBox.h"
#include "Texture.h"
#include "textureManager.h"
#include "Scene.h"

#include "debugger.h"

BlurBox::BlurBox(widget* parent, vector loc, vector size, int blurStrength) : widget(parent) {
	instances.push_back(this);

	this->blurStrength = blurStrength;
	
	texture = std::make_unique<Texture>(size * stuff::pixelSize, TextureFormat::Color, true);

	setLocAndSize(loc, size);
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

	if (texture)
		texture->Resize(size * stuff::pixelSize);
}

void BlurBox::draw() {
	Scene::blurShader->Activate();
	Scene::blurShader->setVec2("screenSize", glm::vec2(stuff::screenSize.x, stuff::screenSize.y));
	Scene::blurShader->setInt("radius", blurStrength);
	// need to render everything first
	textureManager::ForceGPUUpload();
	GenerateSubTexture(texture.get());
	textureManager::DrawImage(Scene::blurShader, loc, size, { 0.f, 0.f, 1.f, 1.f }, useWorldPos, glm::vec4(1), texture->GetHandle());
}

void BlurBox::GenerateSubTexture(Texture* texture) {
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	texture->Bind();
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, size.x * stuff::pixelSize, size.y * stuff::pixelSize);
	texture->Unbind();
}

void BlurBox::Init() {
	sceneFBO = std::make_unique<FBO>(stuff::screenSize / stuff::pixelSize, false, FBOType::ColorAndDepth);
}

void BlurBox::DrawFinal(Shader* shader) {
	shader->Activate();
	sceneFBO->Draw(shader, {0, 0}, {0, 0, 1, 1}, false, glm::vec4(1));
}

void BlurBox::BindFramebuffer() {
	sceneFBO->Bind();
}

void BlurBox::UnbindFramebuffer() {
	sceneFBO->Unbind();
}

void BlurBox::ResizeScreen() {
	sceneFBO->ResizeTexture(stuff::screenSize / stuff::pixelSize);
}