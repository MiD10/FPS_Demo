#ifndef MYINCLUDE_H
#define MYINCLUDE_H

#include <cmath>
#include <stdio.h>
#include <assert.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "glad\glad.h"
#include "GLFW\glfw3.h"

#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective


#include "stb-master\\stb_image.h"

#include <iostream>

// settings
unsigned int SCR_WIDTH = 1024;
unsigned int SCR_HEIGHT = 768;
const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
bool shadows = true;
bool shadowsKeyPressed = false;
unsigned int HW;
unsigned int HH;

//opengl
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
int draw_opengl(void);

#endif