#include "grass.h"
#include "Texture.h"
#include "Scene.h"

#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "FBO.h"
#include "Rectangle.h"


// Simple and fast 2D noise
float hash(glm::vec2 p) {
	return glm::fract(glm::sin(glm::dot(p, glm::vec2(127.1, 311.7))) * 43758.5453123);
}

// 2D Perlin-style noise
float noise(glm::vec2 p) {
	glm::vec2 i = floor(p);
	glm::vec2 f = fract(p);

	// Smooth interpolation
	glm::vec2 u = f * f * (glm::vec2(3.0) - glm::vec2(2.0) * f);

	// Mix 4 corners
	float a = hash(i + glm::vec2(0.0, 0.0));
	float b = hash(i + glm::vec2(1.0, 0.0));
	float c = hash(i + glm::vec2(0.0, 1.0));
	float d = hash(i + glm::vec2(1.0, 1.0));

	return glm::mix(glm::mix(a, b, u.x), glm::mix(c, d, u.x), u.y);
}

// Fractal / turbulence for more organic look
float fbm(glm::vec2 p) {
	float f = 0.0;
	f += 0.5000 * noise(p);
	f += 0.2500 * noise(p * glm::vec2(2.0));
	f += 0.1250 * noise(p * glm::vec2(4.0));
	f += 0.0625 * noise(p * glm::vec2(8.0));

	return f;
}

// Usage in fragment shader for 3-color blending
glm::vec3 getGrassColor(glm::vec2 uv, glm::vec3 grassColor1, glm::vec3 grassColor2, glm::vec3 grassColor3) {
	float n = fbm(uv * glm::vec2(5.0));  // scale noise
	float n1 = fbm((uv + glm::vec2(100.0)) * glm::vec2(5.0)); // different input for variation

	glm::vec3 color;
	if (n > 0.55) color = grassColor1;
	else if (n1 > 0.55) color = grassColor2;
	else color = grassColor3;

	return color;
}

// Helper function to check if a point is inside a polygon
bool isPointInsidePolygon(const std::vector<glm::vec2>& polygon, const glm::vec2& point) {
	int n = polygon.size();
	int count = 0;
	for (int i = 0; i < n; ++i) {
		glm::vec2 p1 = polygon[i];
		glm::vec2 p2 = polygon[(i + 1) % n];  // Wrap around to first vertex

		// Check if the point is inside the y-range of the edge (i.e., y-value of point is between p1.y and p2.y)
		if ((point.y > p1.y && point.y <= p2.y) || (point.y > p2.y && point.y <= p1.y)) {
			// Compute the x-coordinate of the intersection of the edge with the horizontal ray
			float intersectX = (point.y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y) + p1.x;
			if (intersectX > point.x) {
				count++;  // Point is to the left of the edge, increment count
			}
		}
	}
	// If the count is odd, the point is inside the polygon
	return count % 2 == 1;
}



struct GrassInstanceData {
	glm::vec2 position;
	glm::vec3 color;
};

Grass::Grass() {
	std::vector<glm::vec2> polygon = {
		glm::vec2(871,765) , glm::vec2(880,755) , glm::vec2(904,695) , glm::vec2(936,656)
, glm::vec2(947,643) , glm::vec2(978,635) , glm::vec2(1090,635) , glm::vec2(1153,623)
, glm::vec2(1187,620) , glm::vec2(1248,623) , glm::vec2(1185,658) , glm::vec2(1311,730)
, glm::vec2(1288,772) , glm::vec2(1125,878) , glm::vec2(1072,845) , glm::vec2(1038,828)
, glm::vec2(1011,814) , glm::vec2(984,797) , glm::vec2(964,788) , glm::vec2(907,772)
	};

	Texture::bindTextureToShader(Scene::grassShader, "images/worlds/demo/grass.png", "grass");
	Texture::bindTextureToShader(Scene::grassShader, "images/worlds/demo/tallGrass.png", "tallGrass");

	glm::vec3 grassColor1 = glm::vec3(68.0 / 255.0, 113.0 / 255.0, 25.0 / 255.0);
	glm::vec3 grassColor2 = glm::vec3(83.0 / 255.0, 120.0 / 255.0, 23.0 / 255.0);
	glm::vec3 grassColor3 = glm::vec3(56.0 / 255.0, 107.0 / 255.0, 4.0 / 255.0);
	glm::vec3 grassHighlight1 = glm::vec3(57.0 / 255.0, 99.0 / 255.0, 5.0 / 255.0);
	glm::vec3 grassHighlight2 = glm::vec3(96.0 / 255.0, 136.0 / 255.0, 9.0 / 255.0);

	Scene::grassShader->Activate();
	Scene::grassShader->setVec3("grassColor1", grassColor1);
	Scene::grassShader->setVec3("grassColor2", grassColor2);
	Scene::grassShader->setVec3("grassColor3", grassColor3);
	Scene::grassShader->setVec3("grassHighlight1", grassHighlight1);
	Scene::grassShader->setVec3("grassHighlight2", grassHighlight2);

	float vertices[] = {
		// positions       // texture coordinates
		0.0f, 1.0f,      0.0f, 0.0f,  // top-left
		0.0f, 0.0f,      0.0f, 1.0f,  // bottom-left
		1.0f, 0.0f,      1.0f, 1.0f,  // bottom-right
		1.0f, 1.0f,      1.0f, 0.0f   // top-right
	};

	unsigned int indices[] = {
		0, 1, 2,
		0, 2, 3
	};

	vao = std::make_unique<VAO>();
	vao->Bind();
	vbo = std::make_unique<VBO>(vertices, sizeof(vertices));
	ebo = std::make_unique<EBO>(indices, sizeof(indices));
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribDivisor(0, 0);
	glVertexAttribDivisor(1, 0);

	std::vector<GrassInstanceData> grassData;// (grassNum);
	for (int i = 0; i < 5000; ++i) {
		glm::vec2 loc(math::randRange(871.f, 1248.f), math::randRange(620.f, 878.f));
		//glm::vec2 loc(math::randRange(1, 2000.f), math::randRange(0, 2000.f));

		if (isPointInsidePolygon(polygon, loc))
			grassData.push_back(GrassInstanceData(loc, getGrassColor(glm::vec2(loc.x, loc.y * 2.f) / glm::vec2(300.0), grassColor1, grassColor2, grassColor3)));
	}
	grassNum = grassData.size();

	grassDataVBO = std::make_unique<VBO>(grassData.data(), grassData.size() * sizeof(GrassInstanceData));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);

	fbo = std::make_unique<FBO>(stuff::screenSize, true, FBOType::DepthOnly);
}

void Grass::DrawDepth() {
	fbo->Bind();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);   // WRITE depth

	Scene::grassShader->Activate();
	Scene::grassShader->setInt("isDepthPass", 1);

	// draw grass
	vao->Bind();
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, grassNum);

	// unbind fbo
	fbo->Unbind();
}

void Grass::Draw() {
	DrawDepth();

	Scene::grassShader->Activate();
	Scene::grassShader->setInt("isDepthPass", 0);
	// draw grass
	vao->Bind();
	
	glEnable(GL_DEPTH_TEST);

	Scene::grassShader->setInt("isGround", 1);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	Scene::grassShader->setInt("isGround", 0);

	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, grassNum);

	glDisable(GL_DEPTH_TEST);

	Scene::twoDShader->Activate();
	Texture* tex = fbo->GetDepthTexture();
	tex->Bind();
	tex->texUnit(Scene::twoDShader, "grassDepthTex");
}