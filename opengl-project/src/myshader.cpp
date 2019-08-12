#include "myshader.h"

using namespace std;

MyShader::MyShader(const char* vertexPath, const char* fragmentPath)
{
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;

		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	unsigned int vertex, fragment;
	int success;
	char infoLog[512];

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FALIED\n" << infoLog << std::endl;
	}

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	this->ID = glCreateProgram();
	glAttachShader(this->ID, vertex);
	glAttachShader(this->ID, fragment);
	glLinkProgram(this->ID);
	glGetProgramiv(this->ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(this->ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINGKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

MyShader::MyShader(const char *vspath, const char *fspath, const char *gspath)
{
	std::ifstream vertexFile;
	std::ifstream fragmentFile;
	std::ifstream geometryFile;
	std::string vertexCode, fragmentCode, geometryCode;

	vertexFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragmentFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	geometryFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		vertexFile.open(vspath);
		fragmentFile.open(fspath);
		geometryFile.open(gspath);

		std::stringstream vShaderStream, fShaderStream, gShaderStream;

		vShaderStream << vertexFile.rdbuf();
		fShaderStream << fragmentFile.rdbuf();
		gShaderStream << geometryFile.rdbuf();

		vertexFile.close();
		fragmentFile.close();
		geometryFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		geometryCode = gShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
	}
	const char *vShaderCode = vertexCode.c_str();
	const char *fShaderCode = fragmentCode.c_str();
	const char *gShaderCode = geometryCode.c_str();

	unsigned int vertex, fragment, geometry;
	int success;
	char infoLog[512];

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FALIED\n" << infoLog << std::endl;
	}

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	geometry = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometry, 1, &gShaderCode, NULL);
	glCompileShader(geometry);
	glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(geometry, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	this->ID = glCreateProgram();
	glAttachShader(this->ID, vertex);
	glAttachShader(this->ID, fragment);
	glAttachShader(this->ID, geometry);
	glLinkProgram(this->ID);
	glGetProgramiv(this->ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(this->ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINGKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	glDeleteShader(geometry);
}


void MyShader::use(){
	glUseProgram(ID);
}

void MyShader::setBool(const std::string name, bool value) const{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void MyShader::setInt(const std::string name, int value) const{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void MyShader::setFloat(const std::string name, float value) const{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
void MyShader::setMat4(const std::string name, glm::mat4 value) const{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void MyShader::setVec3(const std::string name, glm::vec3 pos) const{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1,&pos[0]);
}

void MyShader::setVec4(const std::string name, glm::vec4 pos) const{
	glUniform4f(glGetUniformLocation(ID, name.c_str()), pos.x, pos.y, pos.z, pos.w);
}