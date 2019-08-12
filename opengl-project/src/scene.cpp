#include "scene.h"

Scene::Scene(){
	collision_obj = 999; // big enough
	//nothing to do
}

Scene::Scene(string file_name){
	ifstream input_scene;
	string line;
	input_scene.open(file_name);
	while (std::getline(input_scene, line)) {
		std::cout << line << std::endl;
		Object* temp = new Object(line, objects.size());
		objects.push_back(temp);
	}
}

Scene::~Scene(){
	//release objects
	for (int i = 0; i < objects.size(); i++)
		delete objects[i];
	for (int i = 0; i < enemys.size(); i++)
		delete enemys[i];
	for (int i = 0; i < lights.size(); i++)
		delete lights[i];
}

void Scene::add_object(string file_name){
	Object* temp = new Object(file_name, objects.size());
	objects.push_back(temp);
	return;
}

void Scene::add_light(string file_name){
	MyModel* temp = new MyModel(file_name, false);
	lights.push_back(temp);
}

void Scene::add_hero(string file_name){
	hero = new Hero(file_name);
}

void Scene::init_enemy_model(string file_name){
	enemy_model = new MyModel(file_name);
}

void Scene::add_enemy(){
	MyModel* t = new MyModel(*enemy_model);
	Enemy* temp = new Enemy(t, enemys.size());
	bool flag = false;
	for (int j = 0; j < objects.size(); j++){
		if (detect_obb(temp, objects[j])){
			flag = true;
			hit_obj = j;
			break;
		}
	}
	if (flag == false){
		for (int j = 0; j < enemys.size(); j++){
			if (detect_obb(temp, enemys[j])){
				flag = true;
				break;
			}
		}
	}
	if (flag){
		delete temp;
	}
	else
		enemys.push_back(temp);
}

void Scene::add_map(string file_name, bool tm){
	Assimp::Importer import;
	const aiScene *scene = import.ReadFile(file_name, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
		cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
		return;
	}
	string directory = file_name.substr(0, file_name.find_last_of('/'));

	processNode(scene->mRootNode, scene, directory, tm);
}

void Scene::processNode(aiNode *node, const aiScene *scene, string directory, bool tm){
	MyModel* p = new MyModel();
	p->directory = directory;
	p->gammaCorrection = true;
	for (unsigned int i = 0; i < node->mNumMeshes; i++)	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		p->meshes.push_back(p->processMesh(mesh, scene));
	}
	if (node->mNumMeshes){
		Object* temp = new Object(p, objects.size(), tm);
		objects.push_back(temp);
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++){
		processNode(node->mChildren[i], scene, directory, tm);
	}
}

void Scene::Draw(MyShader& omyshader, MyShader& deathshader, MyShader& tShader, MyShader& light_shader, MyCamera& camera){
	for (int i = 0; i < lights.size(); i++){
		light_shader.use();
		light_shader.setMat4("model", glm::mat4(1.0f));
		setCamera(light_shader, camera);
		lights[0]->Draw(light_shader);
	}
	
	for (int i = 0; i < enemys.size(); i++){
		enemys[i]->Draw(omyshader, deathshader, tShader, camera, lights[0]);
	}

	switch (test_mode){
	case 0:
		for (int i = 0; i < objects.size(); i++){
			objects[i]->Draw(omyshader, tShader, camera);
		}
		break;
	case 1:
		for (int i = 0; i < objects.size(); i++){
			if (objects[i]->id == hit_obj)
				objects[i]->Draw(omyshader, tShader, camera);
		}
		break;
	case 2:
		for (int i = 0; i < objects.size(); i++){
			if (i == collision_obj)
				objects[i]->Draw(omyshader, tShader, camera);
		}
		break;
	default:
		printf("wrong test mode!\n");
		system("pause");
		exit(0);
	}
}

void Scene::hero_move(GLFWwindow *window, double xpos, double ypos){
	//save the present condition
	hero_move_store();
	//move & detect collision
	if (xpos == 0 && ypos == 0){
		hero->move(window, xpos, ypos);
		if (hero_move_is_collide()){
			hero_move_restore();
		}
	}
	else{
		//可以判断的更细
		///////////////////////////////////////////////////////
		// to be improved
		///////////////////////////////////////////////////////
		hero->move(window, xpos, hero->lastY);
		if (hero_move_is_collide()){
			hero_move_restore();
		}
		hero->move(window, hero->lastX, ypos);
		if (hero_move_is_collide()){
			hero_move_restore();
		}
	}
}

bool Scene::hero_move_is_collide(void){
	bool flag = false;
	for (int i = 0; i < objects.size(); i++){
		if (detect_obb(hero, objects[i])){
			collision_obj = i;
			flag = true;
			break;
		}
	}
	float min;
	bool cflag = false;
	hero->update_cam_pos();
	for (int i = 0; i < objects.size(); i++){
		if (i == 70 || i == 192 || i == 214){
			if (detect_ray_obj_obb(hero->position - hero->cool, glm::vec3(0.0f, -1.0f, 0.0f), objects[i])){
				if (abs(hero->position.y - objects[i]->pModel->maxy) <= 1.0){
					cflag = true;
				}
			}
			if (detect_ray_obj_obb(hero->position + hero->cool, glm::vec3(0.0f, -1.0f, 0.0f), objects[i])){
				if (abs(hero->position.y - objects[i]->pModel->maxy) <= 1.0){
					cflag = true;
				}
			}
		}
	}
	if (cflag){
		hero->stand = true;
	}
	else{
		hero->stand = false;
	}
	
	return flag;
}

void Scene::hero_move_store(void){
	old_position = hero->position;
	old_front = hero->front;
	old_camera_front = hero->camera.cameraFront;
	old_camera_position = hero->camera.cameraPos;
	for (int i = 0; i < 8; i++){
		old_vertex[i] = hero->vertex[i];
	}
	for (int i = 0; i < 3; i++){
		old_surface[i] = hero->surface[i];
	}
}

void Scene::hero_move_restore(void){
	//printf("collide!\n");
	hero->position = old_position;
	hero->front = old_front;
	hero->camera.cameraFront = old_camera_front;
	hero->camera.cameraPos = old_camera_position;
	for (int i = 0; i < 8; i++){
		hero->vertex[i] = old_vertex[i];
	}
	for (int i = 0; i < 3; i++){
		hero->surface[i] = old_surface[i];
	}
}

void Scene::hero_shoot(void){
	//first check the scene
	tobe.clear();
	for (int i = 0; i < objects.size(); i++){
		if (detect_ray_obj_obb(hero->camera.cameraPos, hero->camera.cameraFront, objects[i])){
			tobe.push_back(i);
		}
	}
	float t;
	float min;
	int flag = 1;
	bool is_enemy = false;
	int min_num = 0;
	hero->update_cam_pos();
	for (int i = 0; i < tobe.size(); i++){
		if (detect_ray_obj_tri(hero->camera.cameraPos, hero->camera.cameraFront, objects[tobe[i]], &t)){
			if (flag){
				min = t;
				min_num = objects[tobe[i]]->id;
				flag = 0;
			}
			else{
				if (t < min){
					min = t;
					min_num = objects[tobe[i]]->id;
				}
			}
		}
	}
	//then check the enemy
	//hitting process here:bad implementation
	tobe.clear();
	for (int i = 0; i < enemys.size(); i++){
		if (detect_ray_obj_obb(hero->camera.cameraPos, hero->camera.cameraFront, enemys[i])){
			tobe.push_back(i);
		}
	}
	for (int i = 0; i < tobe.size(); i++){
		if (detect_ray_enemy_tri(hero->camera.cameraPos, hero->camera.cameraFront, enemys[tobe[i]], &t)){
			if (t < min){
				min = t;
				min_num = tobe[i];
				is_enemy = true;
			}
		}
	}
	if (flag){
		printf("hit nothing!\n");
	}
	else{
		if (is_enemy){
			enemys[min_num]->hit();
		}
		/*enemys[0]->hit();*/
		hit_obj = min_num;
		printf("hit:%d\n", hit_obj);
	}
}

void Scene::enemy_update(){
	int col_obj;
	for (int i = 0; i < enemys.size(); i++){
		if (enemys[i]->live){
			//angle = ...
			//position = ...
			Enemy* temp = new Enemy(enemys[i]);
			if ((rand() % 10 == 9)){
				temp->angle += (rand() % 3 - 2) * 2;
				temp->front = glm::vec3(
					cos(glm::radians((float)temp->angle)),
					0.0f,
					sin(glm::radians((float)temp->angle)));
			}
			//sometimes need more speed to run out of 'traps'
			temp->position.x = temp->position.x - temp->front.x * enemys[i]->speed * 3;
			temp->position.z = temp->position.z - temp->front.z * enemys[i]->speed * 3;
			temp->obb_dirty = 1;
			bool flag = false;
			for (int j = 0; j < objects.size(); j++){
				if (detect_obb(temp, objects[j])){
					flag = true;
					enemys[i]->move = false;
					break;
				}
			}
			if (flag == FALSE){
				for (int j = 0; j < enemys.size(); j++){
					if (j == i)
						continue;
					if (detect_obb(temp, enemys[j])){
						flag = true;
						enemys[i]->move = false;
						break;
					}
				}
			}
			if (flag){
				//do nothing
				enemys[i]->angle += 2;
				enemys[i]->front = glm::vec3(
					cos(glm::radians((float)enemys[i]->angle)),
					0.0f,
					sin(glm::radians((float)enemys[i]->angle)));
				enemys[i]->obb_dirty = 1;
			}
			else{
				enemys[i]->front = temp->front;
				enemys[i]->position.x -= enemys[i]->front.x * enemys[i]->speed;
				enemys[i]->position.z -= enemys[i]->front.z * enemys[i]->speed;
				enemys[i]->obb_dirty = 1;
				enemys[i]->move = true;
			}
			delete temp;
		}
		else{
			if (enemys[i]->dead_animation()){
				//do the animation
			}
			else{
				vector<Enemy*>::iterator it = enemys.begin() + i;
				enemys.erase(it);
			}
		}
	}
}

void Scene::hero_update(){
	if (hero->stand){
		drop_t = 0; 
	}
	else{
		hero->position.y -= 0.005 * drop_t++;
		hero->obb_dirty = 1;
	}
}