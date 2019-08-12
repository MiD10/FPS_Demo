#ifndef MYMODEL_H
#define MYMODEL_H

#include "mymesh.h"
#include "stb-master\\stb_image.h"
#include <map>

using namespace std;

class MyModel{
public:
	int initialization;

	/*  函数   */
	MyModel();
	MyModel(const MyModel& a);
	MyModel(string path, bool gamma = false);
	void Draw(MyShader& shader);
	float minx, maxx, miny, maxy, minz, maxz;
	/*  模型数据  */
	vector<MyMesh> meshes;
	vector<Texture> textures_loaded;//用来统计这个重复载入的纹理
	string directory;
	bool gammaCorrection;

	/*  函数   */
	void loadModel(string path);
	void processNode(aiNode *node, const aiScene *scene);
	MyMesh processMesh(aiMesh *mesh, const aiScene *scene);
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
	int TextureFromFile(const char* path, const string& directory);

	//light
	void SetLight(MyShader shader);
};



#endif