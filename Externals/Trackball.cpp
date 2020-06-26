#include "Trackball.h"
#include "Include/GLM/glm/gtc/matrix_transform.hpp"

using namespace std;
using namespace glm;


Trackball::Trackball(float sens, float sp)
{
	this->x_yaw = 0.0;
	this->y_pitch = 0.0;
	this->degree_x = 0.0;
	this->degree_y = 0.0;
	this->lfront = vec3(0.0, 0.0, -1.0);

	this->sensitivity = sens;
	this->speed = sp;
	this->camPos = vec3(0.0, 1.0, 1.0);
	this->camFront = vec3(0.0, 0.0, 0.0);
	this->camUp = vec3(0.0, 1.0, 0.0);
}

Trackball::~Trackball()
{

}

mat4 Trackball::udpate(movement type, int diff_x, int diff_y)
{

	switch (type)
	{
	case Tup:
		camPos += speed * vec3(0.0, 1.0, 0.0);
		break;
	case Tdown:
		camPos -= speed * vec3(0.0, 1.0, 0.0);
		break;
	case Tfront:
		camPos += speed * camFront;
		break;
	case Tback:
		camPos -= speed * camFront;
		break;
	case Tleft:
		camPos -= glm::normalize(glm::cross(camFront, camUp)) * speed;
		break;
	case Tright:
		camPos += glm::normalize(glm::cross(camFront, camUp)) * speed;
		break;
	case Tlook:
		degree_x = diff_x * sensitivity;
		degree_y = diff_y * sensitivity;
		x_yaw += degree_x;
		y_pitch += degree_y;

		if (y_pitch > 89.0f)
			y_pitch = 89.0f;
		if (y_pitch < -89.0f)
			y_pitch = -89.0f;

		lfront.x = cos(radians(y_pitch)) * cos(radians(x_yaw));
		lfront.y = sin(radians(y_pitch));
		lfront.z = cos(radians(y_pitch)) * sin(radians(x_yaw));
		camFront = normalize(lfront);
		break;
	default:
		break;
	}
	return lookAt(camPos, camPos + camFront, camUp);
}

void Trackball::setSensitivity(float s)
{
	this->sensitivity = s;
}

void Trackball::setSpeed(float s)
{
	this->speed = s;
}
