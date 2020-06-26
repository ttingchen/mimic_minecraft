#include "DepthMap.h"
#include "Include/GLM/glm/gtc/matrix_transform.hpp"

using namespace std;
using namespace glm;

DepthMap::DepthMap()
{
	depthMapProg = new Program("../depth.vs.glsl", "../depth.fs.glsl");
	
	glGenFramebuffers(1, &fbo); //Create FBO
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &tex); //Create fboDataTexture
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F,
		depthTex_width, depthTex_height, 0,
		GL_DEPTH_COMPONENT32F, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
		GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, tex, 0);
}

DepthMap::~DepthMap()
{
	delete depthMapProg;
}