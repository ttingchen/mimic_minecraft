#pragma once
#include "Include/GLM/glm/gtc/type_ptr.hpp"
#include "Include/GLEW/glew.h"

using namespace std;
using namespace glm;
typedef struct _TextureData
{
	_TextureData(void) :
		width(0),
		height(0),
		data(0)
	{
	}

	int width;
	int height;
	unsigned char* data;
} TextureData;

GLuint loadTexture(const char* const pngFilepath);
TextureData loadPNG(const char* const pngFilepath);

