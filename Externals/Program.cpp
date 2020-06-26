#include "Program.h"
#include <stdio.h>
using namespace std;
using namespace glm;


void Program::shaderLog()
{
	GLint isCompiled = 0;
	glGetShaderiv(this->ID, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(this->ID, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		GLchar* errorLog = new GLchar[maxLength];
		glGetShaderInfoLog(this->ID, maxLength, &maxLength, &errorLog[0]);

		printf("%s\n", errorLog);
		delete[] errorLog;
	}
}

Program::Program(const char* vsPath, const char* fsPath)
{
	char** vs_glsl = loadShaderSource(vsPath);
	char** fs_glsl = loadShaderSource(fsPath);
	this->ID = glCreateProgram();
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, vs_glsl, NULL);
	glCompileShader(vs);
	shaderLog();

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, fs_glsl, NULL);
	glCompileShader(fs);
	shaderLog();

	glAttachShader(this->ID, vs);
	glAttachShader(this->ID, fs);
	//freeShaderSource(vs_glsl);
	//freeShaderSource(fs_glsl);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	glLinkProgram(this->ID);
	glGetProgramiv(this->ID, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(this->ID, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
		system("pause");
	}

}

Program::~Program() 
{

};

void Program::useProg()
{
	glUseProgram(this->ID);
}

void Program::unuseProg()
{
	glUseProgram(0);
}


char** Program::loadShaderSource(const char* file)
{
	FILE* fp = fopen(file, "rb");
	fseek(fp, 0, SEEK_END);
	long sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *src = new char[sz + 1];
	fread(src, sizeof(char), sz, fp);
	src[sz] = '\0';
	char **srcp = new char*[1];
	srcp[0] = src;
	return srcp;
}

void Program::freeShaderSource(char** srcp)
{
	delete srcp[0];
	delete srcp;
}