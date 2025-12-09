#include "blurBox.h"
#include "main.h"

BlurBox::BlurBox(widget* parent, vector loc, vector size, int blurStrength) : widget(parent) {
	this->blurStrength = blurStrength;
	setLocAndSize(loc, size);

	makeVerticesList();

	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(float), quadVertices.data(), GL_STATIC_DRAW);
	glGenBuffers(1, &quadEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, stuff::screenSize.x, stuff::screenSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void BlurBox::makeVerticesList() {
	vector center = stuff::screenSize / 2.f;
	vector min = loc - center;
	vector max = loc + size - center;
	vector texelSize = vector{ 1.f, 1.f } / stuff::screenSize;
	vector minCoords = loc * texelSize;
	vector maxCoords = (loc + size) * texelSize;

	quadVertices = std::vector<float>{
		// positions	// texture coordinates
		min.x,	max.y,	minCoords.x, maxCoords.y,  // top-left
		min.x,	min.y,	minCoords.x, minCoords.y,  // bottom-left
		max.x,	min.y,	maxCoords.x, minCoords.y,  // bottom-right
		max.x,	max.y,	maxCoords.x, maxCoords.y   // top-right
	};
}

void BlurBox::UpdateVertices() {
	makeVerticesList();

	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);

	void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (ptr) {
		memcpy(ptr, quadVertices.data(), quadVertices.size() * sizeof(float)); // Copy updated vertex data
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}
}

void BlurBox::setupLocs() {
	UpdateVertices();
}

void BlurBox::setLoc(vector loc) {
	__super::setLoc(loc);
	UpdateVertices();
}

void BlurBox::setSize(vector size) {
	__super::setSize(size);
	UpdateVertices();
}

void BlurBox::draw(Shader* shader) {
	UnbindFrameBuffer();
	BindShader();
	blurShader->setVec2("screenSize", glm::vec2(stuff::screenSize.x, stuff::screenSize.y));
	blurShader->setInt("radius", blurStrength);
	blurShader->setMat4("projection", Main::camera->getProjectionMat());
	blurShader->setInt("useProjection", true);

	UpdateVertices();

	BindFrameBuffer();
	GenerateSubTexture(texture);
	glBindVertexArray(quadVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void BlurBox::GenerateSubTexture(GLuint texture) {
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, stuff::screenSize.x, stuff::screenSize.y);
}

void BlurBox::Init() {
	blurShader = std::make_unique<Shader>("blurShader.vert", "blurShader.frag");

	glGenFramebuffers(1, &sceneFBO);
	glGenTextures(1, &sceneTexture);
	BindTexture();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, stuff::screenSize.x, stuff::screenSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	BindFrameBuffer();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneTexture, 0);
	UnbindFrameBuffer();

	glGenVertexArrays(1, &sceneVAO);
	glBindVertexArray(sceneVAO);
	glGenBuffers(1, &sceneVBO);
	glBindBuffer(GL_ARRAY_BUFFER, sceneVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sceneVertices), sceneVertices, GL_STATIC_DRAW);
	glGenBuffers(1, &sceneEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sceneEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
}

void BlurBox::DrawFinal() {
	glClear(GL_COLOR_BUFFER_BIT);
	BindShader();
	blurShader->setInt("radius", 0);
	blurShader->setInt("useProjection", false);
	BindTexture();
	glBindVertexArray(sceneVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void BlurBox::BindFrameBuffer() {
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevBoundFBO); // saves last active FBO
	glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
}

void BlurBox::UnbindFrameBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, prevBoundFBO); // sets to last active FBO
}

void BlurBox::BindTexture() {
	glBindTexture(GL_TEXTURE_2D, sceneTexture);
}

void BlurBox::UnbindTexture() {
	glBindTexture(GL_TEXTURE_2D, 0);

}

void BlurBox::BindShader() {
	blurShader->Activate();
}