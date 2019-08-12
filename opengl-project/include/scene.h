#ifndef SCENE_H
#define SCENE_H

#include "objects.h"

using namespace std;

//manage to obtain the whole static scene
class Scene{
public:
	Scene();
	//input: 
	Scene(string file_name);
	~Scene();

	//input: path to obj file
	vector<Object*> objects;
	void add_object(string file_name);

	//input: light model
	vector<MyModel*> lights;
	void add_light(string file_name);

	//input: enemy model
	MyModel* enemy_model;
	vector<Enemy*> enemys;
	void init_enemy_model(string file_name);
	void add_enemy();
	void enemy_update();

	//input: hero model
	Hero* hero;
	void add_hero(string file_name);

	//input: map model: the difference is that extract each node with mesh(es) as a model
	void add_map(string file_name, bool tm);
	void processNode(aiNode *node, const aiScene *scene, string directory, bool tm);

	//draw
	void Draw(MyShader& obj_shader, MyShader& deathshader, MyShader& test_Shader, MyShader& light_shader, MyCamera& camera);

	//hero controller
	glm::vec3 old_position;
	glm::vec3 old_front;
	glm::vec3 old_camera_front;
	glm::vec3 old_camera_position;
	glm::vec3 old_vertex[8];
	glm::vec3 old_surface[3];
	void hero_move(GLFWwindow *window, double xpos, double ypos);
	bool hero_move_is_collide(void);
	void hero_move_store(void);
	void hero_move_restore(void);

	void hero_shoot(void);

	void hero_update(void);
	float drop_t;
	

	//test
	int collision_obj;
	int hit_obj;
	vector<int> tobe;
};

#endif