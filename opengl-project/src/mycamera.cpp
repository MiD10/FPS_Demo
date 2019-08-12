#include "mycamera.h"

MyCamera::MyCamera() :cameraPos(glm::vec3(0.0f, 0.5f, 3.0f)),
cameraFront(glm::vec3(0.0f, 0.0f, -1.0f)),
cameraUp(glm::vec3(0.0f, 1.0f, 0.0f)),
WorldUp(glm::vec3(0.0f,1.0f,0.0f)),
lastX((float)SCR_WIDTH / 2.0), lastY((float)SCR_HEIGHT / 2.0),
yaw(-90), pitch(0),
MouseSensitivity(0.01), CameraSensitivity(0.1),cameraSpeed(5.0f),
firstMouse(true){
}

void MyCamera::KeyBoardMovement(int Direction){
	cameraSpeed = 10 * deltaTime;
	switch (Direction){
	case FORWARD: cameraPos += cameraSpeed * cameraFront; break;
	case BACKWARD: cameraPos -= cameraSpeed * cameraFront; break;
	case LEFTWARD: cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed; break;
	case RIGHTWARD: cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed; break;
	default:;
	}
}

void MyCamera::MouseMovement(double xpos, double ypos){
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // 注意这里是相反的，因为y坐标是从底部往顶部依次增大的
	lastX = xpos;
	lastY = ypos;
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;
	yaw += xoffset;
	pitch += yoffset;
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front = glm::vec3(1.0f);
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	cameraFront = glm::normalize(front);
	cameraRight = glm::normalize(glm::cross(cameraFront, WorldUp));
	cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
}
