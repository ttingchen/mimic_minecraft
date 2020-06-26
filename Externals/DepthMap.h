#pragma once
#include "Include/GLM/glm/gtc/type_ptr.hpp"
#include "Include/GLEW/glew.h"
#include "Program.h"

#define depthTex_width 1024
#define depthTex_height 1024

using namespace std;
using namespace glm;

class DepthMap {
public:
	DepthMap();
	~DepthMap();

private:
	Program* depthMapProg;
	GLuint fbo;
	GLuint tex;
 };