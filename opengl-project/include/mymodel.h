#ifndef MYMODEL_H
#define MYMODEL_H

#include "mymesh.h"
#include "stb-master\\stb_image.h"
#include <map>

using namespace std;

class MyModel{
public:
	int initialization;

	/*  ����   */
	MyModel();
	MyModel(const MyModel& a);
	MyModel(string path, bool gamma = false);
	void Draw(MyShader& shader);
	float minx, maxx, miny, maxy, minz, maxz;
	/*  ģ������  */
	vector<MyMesh> meshes;
	vector<Texture> textures_loaded;//����ͳ������ظ����������
	string directory;
	bool gammaCorrection;

	/*  ����   */
	void loadModel(string path);
	void processNode(aiNode *node, const aiScene *scene);
	MyMesh processMesh(aiMesh *mesh, const aiScene *scene);
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
	int TextureFromFile(const char* path, const string& directory);

	//light
	void SetLight(MyShader shader);
};



#endif