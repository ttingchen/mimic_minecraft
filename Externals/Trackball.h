#pragma once
#include "Include/GLM/glm/gtc/type_ptr.hpp"
#include "Include/GLEW/glew.h"

using namespace std;
using namespace glm;

enum movement{Tup, Tdown, Tfront, Tback, Tleft, Tright, Tlook};

class Trackball {
public:
	Trackball(float sens, float sp);
	~Trackball();

	mat4 udpate(movement type, int diff_x, int diff_y);
	void setSensitivity(float s);
	void setSpeed(float s);
	vec3 camPos;
	vec3 camFront;
	vec3 camUp;

private:
	float x_yaw, y_pitch;
	float sensitivity;
	float speed;
	vec3 lfront;
	float degree_x;
	float degree_y;

 };