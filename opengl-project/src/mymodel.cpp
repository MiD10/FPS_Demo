#include "mymodel.h"

MyModel::MyModel():gammaCorrection(false){
	maxx = maxy = maxz = minx = miny = minz = 0;
	initialization = 1;
}

MyModel::MyModel(const MyModel& a){
	meshes = a.meshes;
	textures_loaded = a.textures_loaded;
	directory = a.directory;
	gammaCorrection = a.gammaCorrection;
	minx = a.minx, maxx = a.maxx, miny = a.miny, maxy = a.maxy, minz = a.minz, maxz = a.maxz;
}

MyModel::MyModel(string path, bool gamma):gammaCorrection(gamma){
	maxx = maxy = maxz = minx = miny = minz = 0;
	initialization = 1;
	loadModel(path);
}

void MyModel::Draw(MyShader& omyshader){
	for (int i = 0; i < meshes.size(); i++){
		meshes[i].Draw(omyshader);
	}
}

void MyModel::loadModel(string path){
	Assimp::Importer import;
	const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
		cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

void MyModel::processNode(aiNode *node, const aiScene *scene){
	for (unsigned int i = 0; i < node->mNumMeshes; i++)	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++){
		processNode(node->mChildren[i], scene);
	}
}

MyMesh MyModel::processMesh(aiMesh *mesh, const aiScene *scene){
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	Material material;
	if (initialization){
		minx = maxx = mesh->mVertices[0].x;
		miny = maxy = mesh->mVertices[0].y;
		minz = maxz = mesh->mVertices[0].z;
		initialization = 0;
	}
	glm::vec3 temp(0.0f);
	for (unsigned int i = 0; i < mesh->mNumVertices; i++){
		Vertex vertex;
		vertex.Position.x = mesh->mVertices[i].x;
		vertex.Position.y = mesh->mVertices[i].y;
		vertex.Position.z = mesh->mVertices[i].z;
		if (vertex.Position.x < minx) minx = vertex.Position.x;
		if (vertex.Position.x > maxx) maxx = vertex.Position.x;
		if (vertex.Position.y < miny) miny = vertex.Position.y;
		if (vertex.Position.y > maxy) maxy = vertex.Position.y;
		if (vertex.Position.z < minz) minz = vertex.Position.z;
		if (vertex.Position.z > maxz) maxz = vertex.Position.z;
		vertex.Normal.x = mesh->mNormals[i].x;
		vertex.Normal.y = mesh->mNormals[i].y;
		vertex.Normal.z = mesh->mNormals[i].z;
		if (mesh->mTextureCoords[0]) // 网格是否有纹理坐标？
		{
			vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
			vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		vertices.push_back(vertex);
		switch (i % 3){
		case 0:
			temp = temp + vertex.Normal;
			break;
		case 1:
			temp = temp + vertex.Normal;
			break;
		case 2:
			temp = temp + vertex.Normal;
			temp.x = temp.x / 3;
			temp.y = temp.y / 3;
			temp.z = temp.z / 3;
			vertices[i - 2].avgNormal = temp;
			vertices[i - 1].avgNormal = temp;
			vertices[i].avgNormal = temp;
			break;
		}
	}
	// 处理索引
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	// 处理材质
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];
		//可能有贴图 也可能只有颜色 问题是如何判断 是否有贴图呢？

		//这里做了一个简化 原则上应该 都判断一下 是否具有 diffuse specular emissive 等贴图 
		//然后 应该是 一一替换 diffuse_texture 覆盖 diffuse_color 这样 但这次就统一替换了

		if (mat->GetTextureCount(aiTextureType_DIFFUSE) == 0){
			//处理颜色部分
			//diffuse_color
			aiColor3D color(0.f, 0.f, 0.f);
			mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
			material.diffuse = glm::vec3(color.r, color.g, color.b);
			//ambient_color
			mat->Get(AI_MATKEY_COLOR_AMBIENT, color);
			material.ambient = glm::vec3(color.r, color.g, color.b);
			//specular_color
			mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
			material.specular = glm::vec3(color.r, color.g, color.b);
			//emissive_color
			mat->Get(AI_MATKEY_COLOR_EMISSIVE, color);
			material.emissive = glm::vec3(color.r, color.g, color.b);
			//shininess
			float shininess = 0.0f;
			mat->Get(AI_MATKEY_SHININESS, shininess);
			material.shininess = shininess;
			material.text = false;
		}
		else{
			//处理贴图部分：
			vector<Texture> textures;
			//diffuse map
			vector<Texture> diffuseMaps = loadMaterialTextures(mat, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			//specular map
			vector<Texture> specularMaps = loadMaterialTextures(mat, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
			//normal map
			std::vector<Texture> normalMaps = loadMaterialTextures(mat, aiTextureType_HEIGHT, "texture_normal");
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
			//height map
			std::vector<Texture> heightMaps = loadMaterialTextures(mat, aiTextureType_AMBIENT, "texture_height");
			textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

			material.text = true;
			material.textures = textures;
		}
	}
	return MyMesh(vertices, indices, material);
}

vector<Texture> MyModel::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
{
	vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.c_str(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}
	}
	return textures;
}

int MyModel::TextureFromFile(const char* path, const string& directory){
	string filename = string(path);
	filename = directory + '\\' + filename;
	GLuint texture;
	glGenTextures(1, &texture);
	GLint width, height, nrChannels;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
	if (data){
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else if (data == NULL){
		cout << "Can't Find Texture FILEs for " << filename << endl;
		system("pause");
		exit(0);
	}
	return texture;
}

void MyModel::SetLight(MyShader shader)
{
	shader.use();
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		string number = to_string(i);
		string name = "pointLights[" + number + "].";
		//这里设置一个点的坐标作为一个面的坐标 目标足够小的时候 可以近似
		shader.setVec3((name + "position").c_str(), meshes[i].vertices[0].Position);
		shader.setVec3((name + "ambient").c_str(), meshes[i].material.ambient);
		shader.setVec3((name + "diffuse").c_str(), meshes[i].material.diffuse);
		shader.setVec3((name + "specular").c_str(), meshes[i].material.specular);
		shader.setFloat((name + "constant").c_str(), 1.0f);
		shader.setFloat((name + "linear").c_str(), 0.09);
		shader.setFloat((name + "quadratic").c_str(), 0.032);

		shader.setInt((name + "shadowCubeMap").c_str(), 4 + i);
	}
}