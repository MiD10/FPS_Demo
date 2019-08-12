#ifndef MYCAMERA_H
#define MYCAMERA_H

#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective

#include "glad\glad.h"
#include "GLFW\glfw3.h"

#include "math.h"

#define FORWARD 1000
#define BACKWARD 1001
#define LEFTWARD 1002
#define RIGHTWARD 1003

extern unsigned int SCR_WIDTH;
extern unsigned int SCR_HEIGHT;

extern float deltaTime;

class MyCamera{
public:
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	glm::vec3 WorldUp;
	glm::vec3 cameraRight;
	float lastX, lastY;
	float yaw, pitch;
	float MouseSensitivity;
	float CameraSensitivity;
	float cameraSpeed; // adjust accordingly
	bool firstMouse = true;
	MyCamera();
	void KeyBoardMovement(int Direction);
	void MouseMovement(double xpos, double ypos);
};

#endif