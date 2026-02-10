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

Grass::Grass() {
	const std::vector<Fcollision*>& grassPolygons = collision::GetGroundCollision();
	std::vector<std::vector<vector>> shadowPolygons = GetShadowPolygons();

	// find min and max
	float minX = INFINITY;
	float maxX = -INFINITY;
	float minY = INFINITY;
	float maxY = -INFINITY;
	for (Fcollision* polygon : grassPolygons) {
		if (polygon->minX < minX)
			minX = polygon->minX;
		if (polygon->maxX > maxX)
			maxX = polygon->maxX;
		if (polygon->minY < minY)
			minY = polygon->minY;
		if (polygon->maxY > maxY)
			maxY = polygon->maxY;
	}

	float cellSize = 5.f;
	vector size = vector(maxX - minX, maxY - minY) / cellSize;

	for (int y = 0; y < size.y; y++) {
		for (int x = 0; x < size.x; x++) {
			vector jitter(math::randRange(0, cellSize), math::randRange(0, cellSize));
			vector loc = vector(x * cellSize + minX, y * cellSize + minY) + jitter;
			bool inside = false;
			for (auto& polygon : grassPolygons) {
				if (polygon->identifier == 'g' && collision::IsPointInsidePolygon(polygon, loc)) {
					inside = true;
					break;
				}
			}

			if (inside) {
				bool inShadow = false;
				for (auto& polygon : shadowPolygons) {
					if (collision::IsPointInsidePolygon(polygon, loc)) {
						inShadow = true;
						break;
					}
				}
				grassData.push_back(GrassInstanceData(glm::vec2(loc.x, loc.y), getGrassColor(glm::vec2(loc.x, loc.y * 2.f) / glm::vec2(300.0), grassColor1, grassColor2, grassColor3), inShadow));
			}
		}
	}

	grassNum = grassData.size();
}

void Grass::LoadGPU() {
	Texture::bindTextureToShader(Scene::grassShader, "images/worlds/demo/grass.png", "grass");
	Texture::bindTextureToShader(Scene::grassShader, "images/worlds/demo/tallGrass.png", "tallGrass");

	Scene::grassShader->Activate();
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
		{871,765}, {880,755}, {904,695}, {936,656},
		{947,643}, {978,635}, {1090,635}, {1153,623},
		{1187,620}, {1248,623}, {1292,633}, {1315,651},
		{1292,641}, {1285,643}, {1260,633}, {1252,636},
		{1237,630}, {1185,658}, {1270,711}, {1292,757},
		{1288,772}, {1183,836}, {1110,884}, {1078,850},
		{1053,834}, {1001,809}, {976,792}, {905,772}
	};

	std::vector<glm::vec2> polygon1 = {
		{805,733}, {786,724}, {760,713}, {746,698},
		{752,686}, {790,666}, {845,635}, {852,636},
		{860,628}, {925,596}, {927,629}, {897,671},
		{885,687}, {862,710}, {838,721}, {817,724}
	};

	std::vector<glm::vec2> polygon2 = {
		{956,584}, {978,564}, {1037,592}, {1317,460},
		{1639,622}, {1579,640}, {1488,677}, {1395,741},
		{1344,773}, {1314,789}, {1341,736}, {1379,716},
		{1398,693}, {1404,670}, {1401,653}, {1384,639},
		{1368,631}, {1355,628}, {1324,628}, {1312,620},
		{1310,614}, {1309,603}, {1314,591}, {1325,578},
		{1332,573}, {1344,563}, {1362,556}, {1382,545},
		{1385,538}, {1385,530}, {1382,522}, {1371,514},
		{1360,515}, {1349,515}, {1334,510}, {1291,530},
		{1294,546}, {1293,576}, {1289,610}, {1257,603},
		{1234,600}, {1185,599}, {1147,603}, {1093,613},
		{1078,616}, {1051,615}, {1022,610}, {988,598}
	};

	std::vector<glm::vec2> polygon3 = {
		{1314,653}, {1328,649}, {1347,648}, {1369,653},
		{1374,656}, {1382,663}, {1383,675}, {1377,689},
		{1371,696}, {1351,708}, {1349,679}, {1341,671}
	};

	std::vector<glm::vec2> polygon4 = {
		{950,620}, {961,617}, {976,614}, {953,605}
	};

	std::vector<glm::vec2> polygon5 = {
		{466,794}, {418,804}, {368,818}, {328,823},
		{486,789}, {540,782}, {571,774}, {629,761},
		{645,763}, {656,766}, {680,769}, {704,779},
		{707,775}, {693,767}, {706,761}, {718,757},
		{726,759}, {708,767}, {710,770}, {730,765},
		{730,759}, {741,763}, {725,771}, {726,773},
		{724,778}, {714,799}, {693,822}, {649,848},
		{672,860}, {708,837}, {733,821}, {744,813},
		{752,811}, {764,807}, {785,802}, {804,802},
		{811,800}, {826,801}, {859,814}, {873,819},
		{882,820}, {902,828}, {914,830}, {924,835},
		{931,837}, {942,841}, {961,850}, {979,855},
		{987,858}, {1000,864}, {1016,876}, {1020,882},
		{1026,884}, {1037,890}, {1044,900}, {1054,906},
		{1049,913}, {1041,917}, {1031,918}, {1008,934},
		{994,945}, {944,974}, {938,981}, {902,991},
		{873,1004}, {817,1021}, {770,1068}, {614,1115},
		{561,1116}, {170,898}, {237,850}, {317,825}
	};

	return { polygon, polygon1, polygon2, polygon3, polygon4, polygon5 };
}

std::vector<std::vector<vector>> Grass::GetShadowPolygons() {
	std::vector<vector> shadowPolygon = {
		{1026,684}, {1077,658}, {1097,658}, {1151,630},
		{1238,630}, {1191,726}, {1037,726}, {1013,713}
	};

	std::vector<vector> shadowPolygon1 = {
		{1026,684}, {1077,658}, {1097,658}, {1151,630},
		{1238,630}, {1191,726}, {1037,726}, {1013,713},
		{1300,623}, {1306,620}, {1313,619}, {1319,611},
		{1322,610}, {1331,608}, {1337,609}, {1351,601},
		{1358,601}, {1386,603}, {1393,609}, {1399,624},
		{1432,625}, {1431,630}, {1402,630}, {1406,638},
		{1405,645}, {1389,650}, {1379,647}, {1372,645},
		{1364,647}, {1345,651}, {1325,645}, {1323,639},
		{1314,639}, {1305,636}, {1300,631}, {1299,626}
	};

	std::vector<vector> shadowPolygon2 = { // bench
		{1287,556}, {1267,542}, {1274,539}, {1266,535},
		{1319,507}, {1335,517}, {1326,538}, {1294,558}
	};

	std::vector<vector> shadowPolygon3 = {
		{1440,555}, {1426,549}, {1424,541}, {1436,531},
		{1453,535}, {1462,557}
	};

	std::vector<vector> shadowPolygon4 = {
		{1555,652}, {1540,648}, {1536,642}, {1538,634},
		{1545,631}, {1556,631}, {1558,623}, {1566,618},
		{1584,620}, {1587,612}, {1598,608}, {1620,613},
		{1628,623}, {1569,654}
	};

	std::vector<vector> shadowPolygon5 = {
		{1363,764}, {1345,756}, {1341,747}, {1348,740},
		{1384,748}, {1386,743}, {1367,736}, {1361,729},
		{1362,724}, {1369,720}, {1392,721}, {1424,735},
		{1452,699}, {1429,687}, {1431,673}, {1444,669},
		{1458,671}, {1466,662}, {1484,662}, {1490,654},
		{1496,651}, {1508,651}, {1525,657}, {1500,720}
	};

	std::vector<vector> shadowPolygon6 = {
		{1165,757}, {1138,756}, {1141,774}, {1121,780},
		{1116,789}, {1098,795}, {1083,790}, {1063,797},
		{1052,799}, {1035,795}, {1030,789}, {1016,785},
		{1011,781}, {1004,772}, {992,761}, {978,756},
		{974,752}, {971,744}, {985,738}, {993,730},
		{997,729}, {1012,728}, {1021,722}, {1035,719},
		{1059,724}, {1065,733}, {1073,736}, {1085,728},
		{1091,728}, {1121,730}, {1128,737}, {1134,751},
		{1136,751}, {1168,752}
	};

	// forest trees
	std::vector<vector> shadowPolygon7 = {
		{970,890}, {945,891}, {943,905}, {919,909},
		{903,910}, {895,920}, {855,928}, {835,942},
		{816,944}, {780,942}, {772,930}, {781,909},
		{791,904}, {812,896}, {842,898}, {879,907},
		{869,899}, {855,884}, {861,871}, {885,868},
		{890,863}, {926,864}, {940,885}, {973,885}
	};

	std::vector<vector> shadowPolygon8 = {
		{876,845}, {847,844}, {849,861}, {821,864},
		{807,863}, {779,866}, {786,879}, {820,881},
		{818,887}, {789,885}, {790,902}, {768,905},
		{743,907}, {710,897}, {686,888}, {687,864},
		{672,864}, {639,853}, {622,848}, {619,834},
		{625,828}, {636,821}, {660,819}, {710,828},
		{711,834}, {743,836}, {746,841}, {716,841},
		{718,847}, {715,856}, {697,863}, {704,869},
		{713,866}, {738,858}, {769,859}, {763,854},
		{753,851}, {745,845}, {747,837}, {758,835},
		{763,830}, {777,823}, {828,818}, {844,839},
		{877,840}
	};

	std::vector<vector> shadowPolygon9 = {
		{673,783}, {645,782}, {641,799}, {627,803},
		{618,800}, {586,804}, {546,803}, {549,819},
		{535,823}, {517,820}, {508,830}, {478,830},
		{481,847}, {461,851}, {449,846}, {423,851},
		{413,850}, {399,843}, {383,837}, {376,826},
		{380,822}, {397,813}, {416,809}, {428,803},
		{441,802}, {464,785}, {500,775}, {530,777},
		{541,798}, {568,798}, {562,792}, {548,789},
		{542,782}, {546,776}, {570,762}, {603,755},
		{614,756}, {636,762}, {641,777}, {676,778}
	};

	return { shadowPolygon, shadowPolygon1, shadowPolygon2, shadowPolygon3, shadowPolygon4, shadowPolygon5, shadowPolygon6, shadowPolygon7, shadowPolygon8, shadowPolygon9 };
}