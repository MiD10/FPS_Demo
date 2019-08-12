#ifndef MYMESH_H
#define MYMESH_H

#include <vector>
#include "myshader.h"

//assimp to load model
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace std;

struct Vertex{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 avgNormal;
};

struct Texture{
	unsigned int id;
	string type;
	string path;
};

//普适性更强一点 可以适应没有贴图 只有颜色的模型
struct Material{
	//color of ambient
	glm::vec3 ambient;
	//color of diffuse;
	glm::vec3 diffuse;
	//color of specular;
	glm::vec3 specular;
	//color of emission
	glm::vec3 emissive;

	float shininess;
	vector<Texture> textures;
	bool text;
};

class MyMesh{
public:
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	Material material;
	unsigned int depthCubeMap = 0;

	MyMesh(vector<Vertex> vertices, vector<unsigned int> indices, Material material);
	void Draw(MyShader& shader);
	unsigned int VAO, VBO, EBO;
	void setupMesh();
};

#endif