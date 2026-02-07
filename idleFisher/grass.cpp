#include "grass.h"
#include "Texture.h"
#include "Scene.h"

#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "FBO.h"

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

Grass::Grass() {
	std::vector<std::vector<glm::vec2>> grassPolygons = GetGrassPolygons();
	std::vector<std::vector<glm::vec2>> shadowPolygons = GetShadowPolygons();

	// find min and max
	float minX = INFINITY;
	float maxX = -INFINITY;
	float minY = INFINITY;
	float maxY = -INFINITY;
	for (auto& polygon : grassPolygons) {
		for (glm::vec2 point : polygon) {
			if (point.x < minX)
				minX = point.x;
			if (point.x > maxX)
				maxX = point.x;
			if (point.y < minY)
				minY = point.y;
			if (point.y > maxY)
				maxY = point.y;
		}
	}

	float cellSize = 5.f;
	glm::vec2 size = glm::vec2(maxX - minX, maxY - minY) / cellSize;

	for (int y = 0; y < size.y; y++) {
		for (int x = 0; x < size.x; x++) {
			glm::vec2 jitter(math::randRange(0, cellSize), math::randRange(0, cellSize));
			glm::vec2 loc = glm::vec2(x * cellSize + minX, y * cellSize + minY) + jitter;
			bool inside = false;
			for (auto& polygon : grassPolygons) {
				if (isPointInsidePolygon(polygon, loc)) {
					inside = true;
					break;
				}
			}

			if (inside) {
				bool inShadow = false;
				for (auto& polygon : shadowPolygons) {
					if (isPointInsidePolygon(polygon, loc)) {
						inShadow = true;
						break;
					}
				}
				grassData.push_back(GrassInstanceData(loc, getGrassColor(glm::vec2(loc.x, loc.y * 2.f) / glm::vec2(300.0), grassColor1, grassColor2, grassColor3), inShadow));
			}
		}
	}

	grassNum = grassData.size();
}

void Grass::LoadGPU() {
	Texture::bindTextureToShader(Scene::grassShader, "images/worlds/demo/grass.png", "grass");
	Texture::bindTextureToShader(Scene::grassShader, "images/worlds/demo/tallGrass.png", "tallGrass");

	Scene::grassShader->Activate();
	Scene::grassShader->setVec3("grassColor1", grassColor1);
	Scene::grassShader->setVec3("grassColor2", grassColor2);
	Scene::grassShader->setVec3("grassColor3", grassColor3);
	Scene::grassShader->setVec3("grassHighlight1", grassHighlight1);
	Scene::grassShader->setVec3("grassHighlight2", grassHighlight2);
	glm::vec2 grassSize = glm::vec2(20, 9);
	Scene::grassShader->setVec2("grassSize", grassSize);
	Scene::grassShader->setVec2("tallGrassSize", glm::vec2(10, 14));

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

	grassDataVBO = std::make_unique<VBO>(grassData.data(), grassData.size() * sizeof(GrassInstanceData));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);

	fbo = std::make_unique<FBO>(stuff::screenSize, true, FBOType::DepthOnly);
}

void Grass::DrawDepth() {
	fbo->Bind();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);

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
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, grassNum);
	glDisable(GL_DEPTH_TEST);

	// bind texture for 2dShader to use
	Scene::twoDShader->Activate();
	Texture* tex = fbo->GetDepthTexture();
	tex->Bind();
	tex->texUnit(Scene::twoDShader, "grassDepthTex");
}

void Grass::ResizeScreen() {
	fbo->ResizeTexture(stuff::screenSize);
}

std::vector<std::vector<glm::vec2>> Grass::GetGrassPolygons() {
	std::vector<glm::vec2> polygon = {
		glm::vec2(871,765) , glm::vec2(880,755) , glm::vec2(904,695) , glm::vec2(936,656)
		, glm::vec2(947,643) , glm::vec2(978,635) , glm::vec2(1090,635) , glm::vec2(1153,623)
		, glm::vec2(1187,620) , glm::vec2(1248,623) , glm::vec2(1292,633) , glm::vec2(1315,651)
		, glm::vec2(1292,641) , glm::vec2(1285,643) , glm::vec2(1260,633) , glm::vec2(1252,636)
		, glm::vec2(1237,630) , glm::vec2(1185,658) , glm::vec2(1270,711) , glm::vec2(1292,757)
		, glm::vec2(1288,772) , glm::vec2(1183,836) , glm::vec2(1110,884) , glm::vec2(1078,850)
		, glm::vec2(1053,834) , glm::vec2(1001,809) , glm::vec2(976,792) , glm::vec2(905,772)
	};

	std::vector<glm::vec2> polygon1 = {
		glm::vec2(805,733) , glm::vec2(786,724) , glm::vec2(760,713) , glm::vec2(746,698)
		, glm::vec2(752,686) , glm::vec2(790,666) , glm::vec2(845,635) , glm::vec2(852,636)
		, glm::vec2(860,628) , glm::vec2(925,596) , glm::vec2(927,629) , glm::vec2(897,671)
		, glm::vec2(885,687) , glm::vec2(862,710) , glm::vec2(838,721) , glm::vec2(817,724)
	};

	std::vector<glm::vec2> polygon2 = {
		glm::vec2(956,584) , glm::vec2(978,564) , glm::vec2(1037,592) , glm::vec2(1317,460)
		, glm::vec2(1639,622) , glm::vec2(1579,640) , glm::vec2(1488,677) , glm::vec2(1395,741)
		, glm::vec2(1344,773) , glm::vec2(1314,789) , glm::vec2(1341,736) , glm::vec2(1379,716)
		, glm::vec2(1398,693) , glm::vec2(1404,670) , glm::vec2(1401,653) , glm::vec2(1384,639)
		, glm::vec2(1368,631) , glm::vec2(1355,628) , glm::vec2(1324,628) , glm::vec2(1312,620)
		, glm::vec2(1310,614) , glm::vec2(1309,603) , glm::vec2(1314,591) , glm::vec2(1325,578)
		, glm::vec2(1332,573) , glm::vec2(1344,563) , glm::vec2(1362,556) , glm::vec2(1382,545)
		, glm::vec2(1385,538) , glm::vec2(1385,530) , glm::vec2(1382,522) , glm::vec2(1371,514)
		, glm::vec2(1360,515) , glm::vec2(1349,515) , glm::vec2(1334,510) , glm::vec2(1291,530)
		, glm::vec2(1294,546) , glm::vec2(1293,576) , glm::vec2(1289,610) , glm::vec2(1257,603)
		, glm::vec2(1234,600) , glm::vec2(1185,599) , glm::vec2(1147,603) , glm::vec2(1093,613)
		, glm::vec2(1078,616) , glm::vec2(1051,615) , glm::vec2(1022,610) , glm::vec2(988,598)
	};

	std::vector<glm::vec2> polygon3 = {
		glm::vec2(1314,653) , glm::vec2(1328,649) , glm::vec2(1347,648) , glm::vec2(1369,653)
		, glm::vec2(1374,656) , glm::vec2(1382,663) , glm::vec2(1383,675) , glm::vec2(1377,689)
		, glm::vec2(1371,696) , glm::vec2(1351,708) , glm::vec2(1349,679) , glm::vec2(1341,671)
	};

	std::vector<glm::vec2> polygon4 = {
		glm::vec2(950,620), glm::vec2(961,617), glm::vec2(976,614), glm::vec2(953,605)
	};

	std::vector<glm::vec2> polygon5 = {
		glm::vec2(466,794) , glm::vec2(418,804) , glm::vec2(368,818) , glm::vec2(328,823)
		, glm::vec2(486,789) , glm::vec2(540,782) , glm::vec2(571,774) , glm::vec2(629,761)
		, glm::vec2(645,763) , glm::vec2(656,766) , glm::vec2(680,769) , glm::vec2(704,779)
		, glm::vec2(707,775) , glm::vec2(693,767) , glm::vec2(706,761) , glm::vec2(718,757)
		, glm::vec2(726,759) , glm::vec2(708,767) , glm::vec2(710,770) , glm::vec2(730,765)
		, glm::vec2(730,759) , glm::vec2(741,763) , glm::vec2(725,771) , glm::vec2(726,773)
		, glm::vec2(724,778) , glm::vec2(714,799) , glm::vec2(693,822) , glm::vec2(649,848)
		, glm::vec2(672,860) , glm::vec2(708,837) , glm::vec2(733,821) , glm::vec2(744,813)
		, glm::vec2(752,811) , glm::vec2(764,807) , glm::vec2(785,802) , glm::vec2(804,802)
		, glm::vec2(811,800) , glm::vec2(826,801) , glm::vec2(859,814) , glm::vec2(873,819)
		, glm::vec2(882,820) , glm::vec2(902,828) , glm::vec2(914,830) , glm::vec2(924,835)
		, glm::vec2(931,837) , glm::vec2(942,841) , glm::vec2(961,850) , glm::vec2(979,855)
		, glm::vec2(987,858) , glm::vec2(1000,864) , glm::vec2(1016,876) , glm::vec2(1020,882)
		, glm::vec2(1026,884) , glm::vec2(1037,890) , glm::vec2(1044,900) , glm::vec2(1054,906)
		, glm::vec2(1049,913) , glm::vec2(1041,917) , glm::vec2(1031,918) , glm::vec2(1008,934)
		, glm::vec2(994,945) , glm::vec2(944,974) , glm::vec2(938,981) , glm::vec2(902,991)
		, glm::vec2(873,1004) , glm::vec2(817,1021) , glm::vec2(770,1068) , glm::vec2(614,1115)
		, glm::vec2(561,1116) , glm::vec2(170,898) , glm::vec2(237,850) , glm::vec2(317,825)
	};

	return { polygon, polygon1, polygon2, polygon3, polygon4, polygon5 };
}

std::vector<std::vector<glm::vec2>> Grass::GetShadowPolygons() {
	std::vector<glm::vec2> shadowPolygon = {
		glm::vec2(1026,684) , glm::vec2(1077,658) , glm::vec2(1097,658) , glm::vec2(1151,630)
		, glm::vec2(1238,630) , glm::vec2(1191,726) , glm::vec2(1037,726) , glm::vec2(1013,713)
	};

	std::vector<glm::vec2> shadowPolygon1 = {
		glm::vec2(1026,684) , glm::vec2(1077,658) , glm::vec2(1097,658) , glm::vec2(1151,630)
		, glm::vec2(1238,630) , glm::vec2(1191,726) , glm::vec2(1037,726) , glm::vec2(1013,713)
		, glm::vec2(1300,623) , glm::vec2(1306,620) , glm::vec2(1313,619) , glm::vec2(1319,611)
		, glm::vec2(1322,610) , glm::vec2(1331,608) , glm::vec2(1337,609) , glm::vec2(1351,601)
		, glm::vec2(1358,601) , glm::vec2(1386,603) , glm::vec2(1393,609) , glm::vec2(1399,624)
		, glm::vec2(1432,625) , glm::vec2(1431,630) , glm::vec2(1402,630) , glm::vec2(1406,638)
		, glm::vec2(1405,645) , glm::vec2(1389,650) , glm::vec2(1379,647) , glm::vec2(1372,645)
		, glm::vec2(1364,647) , glm::vec2(1345,651) , glm::vec2(1325,645) , glm::vec2(1323,639)
		, glm::vec2(1314,639) , glm::vec2(1305,636) , glm::vec2(1300,631) , glm::vec2(1299,626)
	};

	std::vector<glm::vec2> shadowPolygon2 = { // bench
		glm::vec2(1287,556) , glm::vec2(1267,542) , glm::vec2(1274,539) , glm::vec2(1266,535)
		, glm::vec2(1319,507) , glm::vec2(1335,517) , glm::vec2(1326,538) , glm::vec2(1294,558)
	};

	std::vector<glm::vec2> shadowPolygon3 = {
		glm::vec2(1440,555) , glm::vec2(1426,549) , glm::vec2(1424,541) , glm::vec2(1436,531)
		, glm::vec2(1453,535) , glm::vec2(1462,557)
	};

	std::vector<glm::vec2> shadowPolygon4 = {
		glm::vec2(1555,652) , glm::vec2(1540,648) , glm::vec2(1536,642) , glm::vec2(1538,634)
		, glm::vec2(1545,631) , glm::vec2(1556,631) , glm::vec2(1558,623) , glm::vec2(1566,618)
		, glm::vec2(1584,620) , glm::vec2(1587,612) , glm::vec2(1598,608) , glm::vec2(1620,613)
		, glm::vec2(1628,623) , glm::vec2(1569,654)
	};

	std::vector<glm::vec2> shadowPolygon5 = {
		glm::vec2(1363,764) , glm::vec2(1345,756) , glm::vec2(1341,747) , glm::vec2(1348,740)
		, glm::vec2(1384,748) , glm::vec2(1386,743) , glm::vec2(1367,736) , glm::vec2(1361,729)
		, glm::vec2(1362,724) , glm::vec2(1369,720) , glm::vec2(1392,721) , glm::vec2(1424,735)
		, glm::vec2(1452,699) , glm::vec2(1429,687) , glm::vec2(1431,673) , glm::vec2(1444,669)
		, glm::vec2(1458,671) , glm::vec2(1466,662) , glm::vec2(1484,662) , glm::vec2(1490,654)
		, glm::vec2(1496,651) , glm::vec2(1508,651) , glm::vec2(1525,657) , glm::vec2(1500,720)
	};

	std::vector<glm::vec2> shadowPolygon6 = {
		glm::vec2(1165,757) , glm::vec2(1138,756) , glm::vec2(1141,774) , glm::vec2(1121,780)
		, glm::vec2(1116,789) , glm::vec2(1098,795) , glm::vec2(1083,790) , glm::vec2(1063,797)
		, glm::vec2(1052,799) , glm::vec2(1035,795) , glm::vec2(1030,789) , glm::vec2(1016,785)
		, glm::vec2(1011,781) , glm::vec2(1004,772) , glm::vec2(992,761) , glm::vec2(978,756)
		, glm::vec2(974,752) , glm::vec2(971,744) , glm::vec2(985,738) , glm::vec2(993,730)
		, glm::vec2(997,729) , glm::vec2(1012,728) , glm::vec2(1021,722) , glm::vec2(1035,719)
		, glm::vec2(1059,724) , glm::vec2(1065,733) , glm::vec2(1073,736) , glm::vec2(1085,728)
		, glm::vec2(1091,728) , glm::vec2(1121,730) , glm::vec2(1128,737) , glm::vec2(1134,751)
		, glm::vec2(1136,751) , glm::vec2(1168,752)
	};

	// forest trees
	std::vector<glm::vec2> shadowPolygon7 = {
		glm::vec2(970,890) , glm::vec2(945,891) , glm::vec2(943,905) , glm::vec2(919,909)
		, glm::vec2(903,910) , glm::vec2(895,920) , glm::vec2(855,928) , glm::vec2(835,942)
		, glm::vec2(816,944) , glm::vec2(780,942) , glm::vec2(772,930) , glm::vec2(781,909)
		, glm::vec2(791,904) , glm::vec2(812,896) , glm::vec2(842,898) , glm::vec2(879,907)
		, glm::vec2(869,899) , glm::vec2(855,884) , glm::vec2(861,871) , glm::vec2(885,868)
		, glm::vec2(890,863) , glm::vec2(926,864) , glm::vec2(940,885) , glm::vec2(973,885)
	};

	std::vector<glm::vec2> shadowPolygon8 = {
		glm::vec2(876,845) , glm::vec2(847,844) , glm::vec2(849,861) , glm::vec2(821,864)
		, glm::vec2(807,863) , glm::vec2(779,866) , glm::vec2(786,879) , glm::vec2(820,881)
		, glm::vec2(818,887) , glm::vec2(789,885) , glm::vec2(790,902) , glm::vec2(768,905)
		, glm::vec2(743,907) , glm::vec2(710,897) , glm::vec2(686,888) , glm::vec2(687,864)
		, glm::vec2(672,864) , glm::vec2(639,853) , glm::vec2(622,848) , glm::vec2(619,834)
		, glm::vec2(625,828) , glm::vec2(636,821) , glm::vec2(660,819) , glm::vec2(710,828)
		, glm::vec2(711,834) , glm::vec2(743,836) , glm::vec2(746,841) , glm::vec2(716,841)
		, glm::vec2(718,847) , glm::vec2(715,856) , glm::vec2(697,863) , glm::vec2(704,869)
		, glm::vec2(713,866) , glm::vec2(738,858) , glm::vec2(769,859) , glm::vec2(763,854)
		, glm::vec2(753,851) , glm::vec2(745,845) , glm::vec2(747,837) , glm::vec2(758,835)
		, glm::vec2(763,830) , glm::vec2(777,823) , glm::vec2(828,818) , glm::vec2(844,839)
		, glm::vec2(877,840)
	};

	std::vector<glm::vec2> shadowPolygon9 = {
		glm::vec2(673,783) , glm::vec2(645,782) , glm::vec2(641,799) , glm::vec2(627,803)
		, glm::vec2(618,800) , glm::vec2(586,804) , glm::vec2(546,803) , glm::vec2(549,819)
		, glm::vec2(535,823) , glm::vec2(517,820) , glm::vec2(508,830) , glm::vec2(478,830)
		, glm::vec2(481,847) , glm::vec2(461,851) , glm::vec2(449,846) , glm::vec2(423,851)
		, glm::vec2(413,850) , glm::vec2(399,843) , glm::vec2(383,837) , glm::vec2(376,826)
		, glm::vec2(380,822) , glm::vec2(397,813) , glm::vec2(416,809) , glm::vec2(428,803)
		, glm::vec2(441,802) , glm::vec2(464,785) , glm::vec2(500,775) , glm::vec2(530,777)
		, glm::vec2(541,798) , glm::vec2(568,798) , glm::vec2(562,792) , glm::vec2(548,789)
		, glm::vec2(542,782) , glm::vec2(546,776) , glm::vec2(570,762) , glm::vec2(603,755)
		, glm::vec2(614,756) , glm::vec2(636,762) , glm::vec2(641,777) , glm::vec2(676,778)
	};

	return { shadowPolygon, shadowPolygon1, shadowPolygon2, shadowPolygon3, shadowPolygon4, shadowPolygon5, shadowPolygon6, shadowPolygon7, shadowPolygon8, shadowPolygon9 };
}