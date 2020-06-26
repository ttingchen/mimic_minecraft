#pragma once
#include "Include/GLM/glm/gtc/type_ptr.hpp"
#include "Include/GLEW/glew.h"
#include "Program.h"
#include "Tool.h"
#include "Perlin.h"
#include <vector>

#define worldWidth 200
#define worldDepth 200
#define worldHalfWidth  worldWidth / 2
#define worldHalfDepth  worldDepth / 2

#define depthTex_width 4096
#define depthTex_height 4096

using namespace std;
using namespace glm;

typedef struct
{
	vector<float> Pos;
	vector<float> Texcoord;
	vector<float> Norm;
	vector<unsigned int> Count;

} Plane;

class Land {
public:
	vector<vec3> offset;

	Land(unsigned int timer_cnt);
	~Land();

	void setWindowSize(int width, int height) {
		win_width = width;
		win_height = height;
	};

	void render(mat4 um4v, mat4 um4p, vec3 eyepos, vec3 lightpos, mat4 lightpv);
	void setNormalMap();
	void setShadow();
	void setSsao();
	void renderDepthMap(mat4 um4vp_light, int window_width, int window_height);
	void renderTest(mat4 um4p);
	void renderSsaoTex(mat4 um4v_camera, mat4 um4p_camera);


private:
	int win_height, win_width;
	GLuint vao;
	GLuint vboPos;
	GLuint vboTex;
	GLuint vboNorm;
	GLuint ebo;

	int materialId;
	int indexCount;
	GLuint tex_side;
	GLuint tex_top;
	GLuint tex_sand;
	GLuint tex_side_norm;
	GLuint tex_top_norm;
	GLuint tex_sand_norm;
	bool isN;
	bool isSH;
	bool isSsao;

	float* data;
  
	Program* landProg;
	Plane plane;
	//GLuint alldiffuse_tex;
	GLuint ao_tex;
	GLuint newFbo;

	Program* depthMapProg;
	Program* test;
	GLuint testvao;
	GLuint depthMap_fbo;
	GLuint depthMap_tex;

	Program* ssaoProg;
	GLuint ssao_fbo;
	GLuint gDepth_tex;
	GLuint gNorm_tex;
	GLuint noise_map;

	GLuint kernal_ubo;

	void loadModel(const char* filename);
	void generateHeight(int width, int height);
	float getY(int x, int z);
	void generateMap();

	void initDepthMap();

	void initSsao();
	void initTest();

};