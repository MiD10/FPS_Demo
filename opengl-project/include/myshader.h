#ifndef MYSHADER_H
#define MYSHADER_H

#include "glad\glad.h"
#include "GLFW\glfw3.h"

#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective


#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


class MyShader
{
public:
	GLint ID;

	// 构造器读取并构建着色器
	MyShader(const GLchar* VertexShaderPath, const GLchar* FragmentShaderPath);
	MyShader(const char* vertexPath, const char* fragmentPath, const char *geometryPath);

	// 使用/激活程序
	void use();
	// uniform工具函数
	void setBool(const std::string name, bool value) const;
	void setInt(const std::string name, int value) const;
	void setFloat(const std::string name, float value) const;
	void setMat4(const std::string name, glm::mat4 value) const;
	void setVec3(const std::string name, glm::vec3 pos) const;
	void setVec4(const std::string name, glm::vec4 pos) const;
};

#endif