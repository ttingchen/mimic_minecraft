#include "Land.h"
#include "Include/GLM/glm/gtc/matrix_transform.hpp"
#include "Include/TinyOBJ/tiny_obj_loader.h"
#include "Include/GLM/glm/gtc/type_ptr.hpp"

#define deg2rad(x) ((x)*((3.1415926f)/(180.0f)))

using namespace std;
using namespace glm;

Land::Land(unsigned int timer_cnt)
{
	win_width = 1440;
	win_height = 990;

	landProg = new Program("../land.vs.glsl", "../land.fs.glsl");
	loadModel("../Assets/plane.obj");
	data = new float[worldWidth * worldDepth]; 
	srand(timer_cnt);
	generateHeight(worldWidth, worldDepth);
	generateMap();
	isN = true;
	isSH = true;
	isSsao = true;

	landProg->useProg();
	landProg->setInt("tex", 0);
	landProg->setInt("sand_tex", 1);
	landProg->setInt("tex_norm", 2);
	landProg->setInt("sand_tex_norm", 3);
	landProg->setInt("shadow_tex", 4);
	landProg->setInt("ao_tex", 5);

	landProg->setBool("isN", isN);
	landProg->setBool("isSH", isSH);
	landProg->setBool("isSsao", isSsao);

	landProg->unuseProg();

	depthMapProg = new Program("../depth.vs.glsl", "../depth.fs.glsl");
	ssaoProg = new Program("../ssao.vs.glsl", "../ssao.fs.glsl");
	test = new Program("../testDepth.vs.glsl", "../testDepth.fs.glsl");

	initDepthMap();

	//ssao
	initSsao();
	initTest();
}

Land::~Land()
{
	delete landProg;
	delete depthMapProg;
	delete test;
	delete ssaoProg;
	delete[] data;
}


void Land::initDepthMap()
{
	glGenFramebuffers(1, &depthMap_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMap_fbo);

	glGenTextures(1, &depthMap_tex);
	glBindTexture(GL_TEXTURE_2D, depthMap_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, depthTex_width, depthTex_height, 0,
		GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap_tex, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


}

//init gbuffer tex
void Land::initSsao() {
	glGenFramebuffers(1, &ssao_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, ssao_fbo);

	glGenTextures(1, &gNorm_tex);
	glBindTexture(GL_TEXTURE_2D, gNorm_tex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, win_width, win_height);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, win_width, win_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenTextures(1, &gDepth_tex);
	glBindTexture(GL_TEXTURE_2D, gDepth_tex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32, win_width, win_height);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);


	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gNorm_tex, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, gDepth_tex, 0);

	// noise map
	glGenTextures(1, &noise_map);
	glBindTexture(GL_TEXTURE_2D, noise_map);
	vec3 noiseData[16];
	for (int i = 0; i < 16; ++i)
	{
		noiseData[i] = normalize(vec3(
			rand() / (float)RAND_MAX, // 0.0 ~ 1.0
			rand() / (float)RAND_MAX, // 0.0 ~ 1.0
			0.0f
		));
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 4, 4, 0, GL_RGB, GL_FLOAT, &noiseData[0][0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	////
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Land::initTest() {
	//ssao
	glGenFramebuffers(1, &newFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, newFbo);

	glGenTextures(1, &ao_tex);
	glBindTexture(GL_TEXTURE_2D, ao_tex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, win_width, win_height);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	GLuint dd;
	glGenTextures(1, &dd);
	glBindTexture(GL_TEXTURE_2D, dd);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32, win_width, win_height);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, ao_tex, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, dd, 0);



	glGenVertexArrays(1, &testvao);
	glBindVertexArray(testvao);
	// ----- Begin Initialize Kernal UBO
	glGenBuffers(1, &kernal_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, kernal_ubo);
	vec4 kernals[32];
	//srand(time(NULL));
	for (int i = 0; i < 32; ++i)
	{
		float scale = i / 32.0;
		scale = 0.1f + 0.9f * scale * scale;
		kernals[i] = vec4(normalize(vec3(
			rand() / (float)RAND_MAX * 2.0f - 1.0f,
			rand() / (float)RAND_MAX * 2.0f - 1.0f,
			rand() / (float)RAND_MAX * 0.85f + 0.15f)) * scale,
			0.0f
		);
	}
	glBufferData(GL_UNIFORM_BUFFER, 32 * sizeof(vec4), &kernals[0][0], GL_STATIC_DRAW);
	// ----- End Initialize Kernal UBO
	
	//ssao bind position and normal tex //
	test->useProg();
	test->setInt("gDepth_tex", 0);
	test->setInt("gNorm_tex", 1);
	test->setInt("gNoise", 2);
	GLint blockIdx = glGetUniformBlockIndex(test->getID(), "Kernals");
	glUniformBlockBinding(test->getID(), blockIdx, 0);
	test->unuseProg();


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	///
}


void Land::loadModel(const char* filename)
{
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err;

	bool ret = tinyobj::LoadObj(shapes, materials, err, filename);
	if (err.size() > 0)
	{
		printf("Load Models Fail! Please check the solution path");
		return;
	}

	printf("Load Models Success ! Shapes size %d Material size %d\n", shapes.size(), materials.size());

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vboPos);
	glGenBuffers(1, &vboTex);
	glGenBuffers(1, &vboNorm);
	glGenBuffers(1, &ebo);

	plane.Pos = shapes[0].mesh.positions;
	plane.Norm = shapes[0].mesh.normals;
	plane.Texcoord = shapes[0].mesh.texcoords;
	plane.Count = shapes[0].mesh.indices;

	glBindBuffer(GL_ARRAY_BUFFER, vboPos);
	glBufferData(GL_ARRAY_BUFFER, plane.Pos.size() * sizeof(float), plane.Pos.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vboTex);
	glBufferData(GL_ARRAY_BUFFER, plane.Texcoord.size() * sizeof(float), plane.Texcoord.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vboNorm);
	glBufferData(GL_ARRAY_BUFFER, plane.Norm.size() * sizeof(float), plane.Norm.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, plane.Count.size() * sizeof(unsigned int), plane.Count.data(), GL_STATIC_DRAW);
	materialId = shapes[0].mesh.material_ids[0];
	indexCount = plane.Count.size();

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	
	this->tex_side = loadTexture("../Assets/grass_side.png");
	this->tex_top = loadTexture("../Assets/grass_top.png");
	this->tex_sand = loadTexture("../Assets/sand.png");

	this->tex_side_norm = loadTexture("../Assets/grass_side_n.png");
	this->tex_top_norm = loadTexture("../Assets/grass_top_n.png");
	this->tex_sand_norm = loadTexture("../Assets/sand_n.png");


}

void Land::generateHeight(int width,int height) {
	printf("======generateHeight======\n");

	int	size = width * height;
	
	int	quality = 2;
	float z = (float)rand() / RAND_MAX * 100;
	printf(" z = %f\n", z);

	for (int j = 0; j < 4; j++) {

		if (j == 0) 
			for (int i = 0; i < size; i++) data[i] = 0;

		for (int i = 0; i < size; i++) {

			float x = i % width, y = (i / width) | 0;
			data[i] += noise(x / quality, y / quality, z) * quality;

		}

		quality *= 4;

	}

}

float Land::getY(int x,int z) {
	int size = worldWidth * worldDepth;
	return floor( data[x + z * worldWidth] * 0.2);

}

void Land::generateMap() {
	printf("======generateMap======\n");
	int i = 0;
	float max = 0;
	float min = INFINITY;
	for (int z = 0; z < worldDepth; z++) {
		for (int x = 0; x < worldWidth; x++) {

			float h = getY(x, z);
			if (h > max) max = h;
			if (h < min) min = h;
			//printf("h = %f\n", h);
			vec3 newPos = vec3(
				x - worldHalfWidth,
				h,
				z - worldHalfDepth);

			offset.push_back(newPos);

			//printf("%d: %f, %f, %f\n", i, offset[i].x, offset[i].y, offset[i].z);

			i++;

		}
	}
	landProg->useProg();
	landProg->setFloat("sand_height", min + ((max - min) / 3));

	printf("max: %f, min: %f\n", max, min);
	printf("sand_height: %f\n", min + ((max - min) / 3));
	landProg->unuseProg();
}

void Land::setNormalMap() {
	isN = !isN;
	landProg->useProg();
	landProg->setBool("isN", isN);
	landProg->unuseProg();
}

void Land::setShadow() {
	isSH = !isSH;
	landProg->useProg();
	landProg->setBool("isSH", isSH);
	landProg->unuseProg();
}

void Land::setSsao() {
	isSsao = !isSsao;
	landProg->useProg();
	landProg->setBool("isSsao", isSsao);
	landProg->unuseProg();
}

void Land::renderDepthMap(mat4 um4vp_light, int window_width, int window_height)
{
	glViewport(0, 0, depthTex_width, depthTex_height);
	glEnable(GL_DEPTH_TEST);
	depthMapProg->useProg();

	glBindFramebuffer(GL_FRAMEBUFFER, depthMap_fbo);

	glClear(GL_DEPTH_BUFFER_BIT);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(4.0f, 4.0f);

	int size = worldDepth * worldWidth;
	depthMapProg->setMat4("um4vp", um4vp_light);
	depthMapProg->setMat4("trans", translate(mat4(1.0), vec3(0.0, 0.5, 0.0)));
	
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);

	//draw
	glBindVertexArray(vao);
	for (int j = 0; j < 5; j++) {
		if (j == 1) { //left
			depthMapProg->setMat4("trans", translate(mat4(1.0), vec3(0.0, 0.0, -0.5))*
				rotate(mat4(1.0), deg2rad(0), vec3(0.0f, 1.0f, 0.0f))*
				rotate(mat4(1.0), deg2rad(-90), vec3(1.0f, 0.0f, 0.0f)));
		}
		else if (j == 2) { //back
			depthMapProg->setMat4("trans", translate(mat4(1.0), vec3(0.5, 0.0, 0.0))*
				rotate(mat4(1.0), deg2rad(90), vec3(0.0f, 1.0f, 0.0f))*
				rotate(mat4(1.0), deg2rad(-90), vec3(1.0f, 0.0f, 0.0f)));
		}
		else if (j == 3) { //right
			depthMapProg->setMat4("trans", translate(mat4(1.0), vec3(0.0, 0.0, 0.5))*
				rotate(mat4(1.0), deg2rad(180), vec3(0.0f, 1.0f, 0.0f))*
				rotate(mat4(1.0), deg2rad(-90), vec3(1.0f, 0.0f, 0.0f)));
		}
		else if (j == 4) { //front
			depthMapProg->setMat4("trans", translate(mat4(1.0), vec3(-0.5, 0.0, 0.0))*
				rotate(mat4(1.0), deg2rad(-90), vec3(0.0f, 1.0f, 0.0f))*
				rotate(mat4(1.0), deg2rad(-90), vec3(1.0f, 0.0f, 0.0f)));
		}

		int num = 300;
		for (int i = 0; i < size; i += 300) {
			if (i + 300 > size) num = size - i;
			depthMapProg->setVec3Array("offset", offset[i], num);
			glDrawElementsInstanced(GL_TRIANGLES, indexCount,
				GL_UNSIGNED_INT, 0, num);
		}
	}

	depthMapProg->unuseProg();
	glDisable(GL_POLYGON_OFFSET_FILL);

	//glCullFace(GL_BACK);
	//glDisable(GL_CULL_FACE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Land::renderSsaoTex(mat4 um4v_camera, mat4 um4p_camera)
{
	glEnable(GL_DEPTH_TEST);
	
	glBindFramebuffer(GL_FRAMEBUFFER, ssao_fbo);
	//printf("win_width: %d, height: %d\n", win_width, win_height);
	//1440, 990
	glViewport(0, 0, win_width, win_height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(4.0f, 4.0f);
	
	ssaoProg->useProg();

	int size = worldDepth * worldWidth;
	ssaoProg->setMat4("um4v", um4v_camera);
	ssaoProg->setMat4("um4p", um4p_camera);
	ssaoProg->setMat4("trans", translate(mat4(1.0), vec3(0.0, 0.5, 0.0)));

	//draw
	glBindVertexArray(vao);
	for (int j = 0; j < 5; j++) {
		if (j == 1) { //left
			ssaoProg->setMat4("trans", translate(mat4(1.0), vec3(0.0, 0.0, -0.5))*
				rotate(mat4(1.0), deg2rad(0), vec3(0.0f, 1.0f, 0.0f))*
				rotate(mat4(1.0), deg2rad(-90), vec3(1.0f, 0.0f, 0.0f)));
		}
		else if (j == 2) { //back
			ssaoProg->setMat4("trans", translate(mat4(1.0), vec3(-0.5, 0.0, 0.0))*
				rotate(mat4(1.0), deg2rad(90), vec3(0.0f, 1.0f, 0.0f))*
				rotate(mat4(1.0), deg2rad(-90), vec3(1.0f, 0.0f, 0.0f)));
		}
		else if (j == 3) { //right
			ssaoProg->setMat4("trans", translate(mat4(1.0), vec3(0.0, 0.0, 0.5))*
				rotate(mat4(1.0), deg2rad(180), vec3(0.0f, 1.0f, 0.0f))*
				rotate(mat4(1.0), deg2rad(-90), vec3(1.0f, 0.0f, 0.0f)));
		}
		else if (j == 4) { //front
			ssaoProg->setMat4("trans", translate(mat4(1.0), vec3(0.5, 0.0, 0.0))*
				rotate(mat4(1.0), deg2rad(-90), vec3(0.0f, 1.0f, 0.0f))*
				rotate(mat4(1.0), deg2rad(-90), vec3(1.0f, 0.0f, 0.0f)));
		}


		int num = 300;
		for (int i = 0; i < size; i += 300) {
			if (i + 300 > size) num = size - i;
			ssaoProg->setVec3Array("offset", offset[i], num);
			glDrawElementsInstanced(GL_TRIANGLES, indexCount,
				GL_UNSIGNED_INT, 0, num);
		}
	}

	ssaoProg->unuseProg();
	glDisable(GL_POLYGON_OFFSET_FILL);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Land::renderTest(mat4 um4p) {
	glEnable(GL_DEPTH_TEST);
	test->useProg();

	glBindFramebuffer(GL_FRAMEBUFFER, newFbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	//ssao bind position and normal tex //
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gDepth_tex);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNorm_tex);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, noise_map);

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, kernal_ubo);
	test->setVec2("noise_scale", vec2(win_width / 4.0, win_height / 4.0));
	test->setMat4("ssao_um4p", um4p);
	
	//glDisable(GL_DEPTH_TEST);
	glBindVertexArray(testvao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	test->unuseProg();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Land::render(mat4 um4v, mat4 um4p, vec3 eyepos, vec3 lightpos, mat4 lightpv)
{
	mat4 scale_bias_matrix = translate(mat4(), vec3(0.5f, 0.5f, 0.5f)) *
		scale(mat4(), vec3(0.5f, 0.5f, 0.5f));

	mat4 shadow_matrix = scale_bias_matrix * lightpv;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT);

	int size = worldDepth * worldWidth;
	landProg->useProg();

	landProg->setMat4("um4v", um4v);
	landProg->setMat4("um4p", um4p);
	landProg->setVec3("eyepos", eyepos);
	landProg->setVec3("light_pos", lightpos);
	landProg->setMat4("shadow_matrix", shadow_matrix);
	landProg->setMat4("trans", translate(mat4(1.0), vec3(0.0, 0.5, 0.0)) );
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_top);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex_sand);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, tex_top_norm);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, tex_sand_norm);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, depthMap_tex);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, ao_tex);///////////

	glBindVertexArray(vao);
	
	//glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	for (int j = 0; j < 5; j++) {
		if (j == 1) { //left
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tex_side);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, tex_side_norm);
			landProg->setMat4("trans", translate(mat4(1.0), vec3(0.0, 0.0, -0.5))*
				rotate(mat4(1.0), deg2rad(0), vec3(0.0f, 1.0f, 0.0f))*
				rotate(mat4(1.0), deg2rad(-90), vec3(1.0f, 0.0f, 0.0f)));
		}
		else if (j == 2) { //back
			landProg->setMat4("trans", translate(mat4(1.0), vec3(-0.5, 0.0, 0.0))*
				rotate(mat4(1.0), deg2rad(90), vec3(0.0f, 1.0f, 0.0f))*
				rotate(mat4(1.0), deg2rad(-90), vec3(1.0f, 0.0f, 0.0f)) );
		}
		else if (j == 3) { //right
			landProg->setMat4("trans", translate(mat4(1.0), vec3(0.0, 0.0, 0.5))*
				rotate(mat4(1.0), deg2rad(180), vec3(0.0f, 1.0f, 0.0f))*
				rotate(mat4(1.0), deg2rad(-90), vec3(1.0f, 0.0f, 0.0f)));
		}
		else if (j == 4) { //front
			landProg->setMat4("trans", translate(mat4(1.0), vec3(0.5, 0.0, 0.0))*
				rotate(mat4(1.0), deg2rad(-90), vec3(0.0f, 1.0f, 0.0f))*
				rotate(mat4(1.0), deg2rad(-90), vec3(1.0f, 0.0f, 0.0f)));
		}

		int num = 300;
		for (int i = 0; i < size; i += 300) {
			if (i + 300 > size) num = size - i;
			landProg->setVec3Array("offset", offset[i], num);
			glDrawElementsInstanced(GL_TRIANGLES, indexCount,
				GL_UNSIGNED_INT, 0, num);
		}
	}

	landProg->unuseProg();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
