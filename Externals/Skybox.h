#pragma once
#include "Include/GLM/glm/gtc/type_ptr.hpp"
#include "Include/GLEW/glew.h"
#include "Program.h"
#include "Tool.h"
#include <vector>

using namespace std;
using namespace glm;

class Skybox {
public:
	
	Skybox(const char* vsPath, const char* fsPath);
	~Skybox();

	void render(mat4 &proj, mat4 &view);

	void useProg();

private:

	GLuint vao;
	mat4 pvm;
	Program* skyboxProg;
	GLuint loadCubemap();
	TextureData loadPNG(const char* const pngFilepath);
	
};
