#include "building.h"
#include "collision.h"
#include "character.h"
#include "main.h"

Abuilding::Abuilding(const std::string& path, vector loc, vector left, vector right) {
	buildingImg = std::make_unique<Image>(path, loc, true);

	this->loc = loc;
	leftPoint = loc + left;
	rightPoint = loc + right;
}

Abuilding::~Abuilding() {
	for (int i = 0; i < cols.size(); i++)
		collision::removeCollisionObject(cols[i].get());
}

void Abuilding::draw(Shader* shaderProgram) {
	buildingImg->draw(shaderProgram);
}

void Abuilding::DrawDebugLines() {
#ifdef _DEBUG
	Scene::lineShader->Activate();
	Scene::lineShader->setMat4("projection", GetMainCamera()->getProjectionMat());
	glm::vec3 camPos = GetMainCamera()->GetPosition();
	Scene::lineShader->setVec2("playerPos", camPos);

	Scene::lineShader->Activate();
	Scene::lineShader->setInt("useWorldPos", true);
	Scene::lineShader->setInt("isRectangle", true);

	float lineVertices[] = {
		leftPoint.x, leftPoint.y,
		rightPoint.x, rightPoint.y
	};

	unsigned int VAO, VBO;
	glCreateVertexArrays(1, &VAO);
	glCreateBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Draw line
	Scene::lineShader->setVec4("color", glm::vec4(1.f, 0.f, 0.f, 1.f));

	glBindVertexArray(VAO);
	glLineWidth(2.5f);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
#endif // DEBUG
}
 
void Abuilding::setLoc(vector loc) {
	this->loc = loc;
	buildingImg->setLoc(loc);
}

void Abuilding::setupDepthCheck(vector leftMost, vector rightMost) {
	leftPoint = leftMost;
	rightPoint = rightMost;
}

bool Abuilding::calcInFront() {
	vector charLoc = GetCharacter()->getCharLoc();
	vector p1 = leftPoint;
	vector p2 = rightPoint;

	float m = (p2.y - p1.y) / (p2.x - p1.x);
	float y = m * (charLoc.x - p1.x) + p1.y;

	return charLoc.y >= y;
}