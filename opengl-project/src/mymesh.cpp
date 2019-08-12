#include "mymesh.h"

using namespace std;

MyMesh::MyMesh(vector<Vertex> v, vector<unsigned int> i, Material m){
	vertices = v;
	indices = i;
	material = m;
	setupMesh();
}

void MyMesh::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, avgNormal));
	glEnableVertexAttribArray(3);

	glBindVertexArray(0);
}

void MyMesh::Draw(MyShader& omyShader){
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;

	//应该这样考虑 ：如果有纹理贴图 那么按照纹理贴图来 ，如果没有，就按照颜色来。
	if (material.text)
	{
		omyShader.setInt("material.text", 1);
		for (unsigned int i = 0; i < material.textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			string number;
			string name = material.textures[i].type;
			if (name == "texture_diffuse")
				number = to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = to_string(specularNr++);
			else if (name == "texture_normal")
				number = std::to_string(normalNr++);
			else if (name == "texture_height")
				number = std::to_string(heightNr++);

			omyShader.setInt(("material." + name + number).c_str(), i);
			glBindTexture(GL_TEXTURE_2D, material.textures[i].id);
		}
		glActiveTexture(GL_TEXTURE0);
	}
	else
	{
		omyShader.setInt("material.text", 0);

		omyShader.setVec3("material.ambient", material.ambient);
		omyShader.setVec3("material.diffuse", material.diffuse);
		omyShader.setVec3("material.specular", material.specular);
		omyShader.setVec3("material.emissive", material.emissive);
		omyShader.setFloat("material.shininess", material.shininess);
	}
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}