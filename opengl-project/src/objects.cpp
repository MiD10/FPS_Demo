#include "objects.h"

///////////////////////////////////////////////////////
//	OBJECT
///////////////////////////////////////////////////////
Object::Object(){}

Object::Object(string path, int input_id) : id(input_id), model_matrix(glm::mat4(1.0f)), position(glm::vec3(0.0f, 0.0f, -1.0f)), front(glm::vec3(0.0f, 0.0f, 1.0f)), up(glm::vec3(0.0f, 1.0f, 0.0f)){
	pModel = new MyModel(path, true);
	name = path;
	state = Free;
	test_mode = true;
	//model_matrix = glm::translate(model_matrix, glm::vec3(0 - pModel->minx, 0 - pModel->miny, 0 - pModel->minz));
	obb_init();
}

Object::Object(MyModel* pointer, int input_id, bool test_m) :test_mode(test_m), id(input_id){
	pModel = pointer;
	state = Statical;
	obb_init();
}

Object::~Object(){
	delete pModel;
}

void Object::Draw(MyShader& omyshader, MyShader& tShader, MyCamera& camera){
	draw_test_obb(glm::mat4(1.0f), tShader, camera);
}

void Object::obb_init(){
	float refer[6] = {
		pModel->minx, pModel->maxx,
		pModel->miny, pModel->maxy,
		pModel->minz, pModel->maxz
	};
	float v[24] = {
		refer[0], refer[2], refer[4],
		refer[1], refer[2], refer[4],
		refer[1], refer[3], refer[4],
		refer[0], refer[3], refer[4],
		refer[0], refer[2], refer[5],
		refer[1], refer[2], refer[5],
		refer[1], refer[3], refer[5],
		refer[0], refer[3], refer[5]
	};
	int num[36]{
		0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7,
			0, 1, 5, 0, 5, 4, 2, 6, 3, 3, 6, 7,
			1, 2, 5, 2, 5, 6, 0, 3, 4, 3, 4, 7
	};
	float vertices[108];
	for (int i = 0; i < 36; i++){
		vertices[i * 3] = v[num[i] * 3];
		vertices[i * 3 + 1] = v[num[i] * 3 + 1];
		vertices[i * 3 + 2] = v[num[i] * 3 + 2];
	}
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);
	/////////////////////////////////////////////////////////////////////////////////////////////
	float t[6] = { pModel->maxx, pModel->minx,
				   pModel->maxy, pModel->miny,
				   pModel->maxz, pModel->minz };
	
	////////////////////////
	//   2_________3
	//   /        /|
	//	/        / |
	// /_6_____7/  | 1
	// |        |  /
	// |        | /
	// |________|/
	// 4        5
	////////////////////////
	for (int i = 0; i < 8; i++){
		vertex_ini[i].x = i & 1 ? t[0] : t[1];
		vertex_ini[i].y = i & 2 ? t[2] : t[3];
		vertex_ini[i].z = i & 4 ? t[4] : t[5];
		vertex[i] = vertex_ini[i];
	}

	//x,y,z
	surface[0] = glm::normalize(vertex[1] - vertex[0]);
	surface[1] = glm::normalize(vertex[2] - vertex[0]);
	surface[2] = glm::normalize(vertex[4] - vertex[0]);

	/*cout << name << endl;
	printf("x: %f %f %f\n", surface[0].x, surface[0].y, surface[0].z);
	printf("y: %f %f %f\n", surface[1].x, surface[1].y, surface[1].z);
	printf("z: %f %f %f\n", surface[2].x, surface[2].y, surface[2].z);*/

	//update
	if (state == Statical)
		return;
	else {
		obb_dirty = 1;
		obb_update();
	}
}

void Object::obb_update(){
	glm::mat4 temp = model_matrix;
	temp = glm::translate(temp, position);
	temp = glm::translate(temp, cool - glm::vec3(0));
	if (front.x >= 0)
		temp = glm::rotate(temp, acos(front.z), up);
	else
		temp = glm::rotate(temp, -acos(front.z), up);
	temp = glm::translate(temp, glm::vec3(0) - cool);

	for (int i = 0; i < 8; i++){
		glm::vec4 tt(vertex_ini[i].x, vertex_ini[i].y, vertex_ini[i].z, 1);
		tt = temp * tt;
		vertex[i] = glm::vec3(tt.x, tt.y, tt.z);
	}
	surface[0] = glm::normalize(vertex[1] - vertex[0]);
	surface[1] = glm::normalize(vertex[2] - vertex[0]);
	surface[2] = glm::normalize(vertex[4] - vertex[0]);

	//cout << name << endl;
	//printf("x: %f %f %f\n", surface[0].x, surface[0].y, surface[0].z);
	//printf("y: %f %f %f\n", surface[1].x, surface[1].y, surface[1].z);
	//printf("z: %f %f %f\n", surface[2].x, surface[2].y, surface[2].z);
	//printf("x: %f %f %f\n", vertex[0].x, vertex[0].y, vertex[0].z);
	//printf("y: %f %f %f\n", vertex[1].x, vertex[1].y, vertex[1].z);
	//printf("z: %f %f %f\n", vertex[2].x, vertex[2].y, vertex[2].z);
	//system("pause");
	//updated
	obb_dirty = 0;
}

////////////////////////////////////////////////////////////
//	DRAW OBB BOX FOR TEST
////////////////////////////////////////////////////////////
void Object::draw_test_obb(glm::mat4& model_matrix, MyShader& tShader, MyCamera& camera){
	tShader.use();
	setCamera(tShader, camera);
	tShader.setMat4("model", model_matrix);
	glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
	glDrawArrays(GL_LINES, 0, 36);
}

////////////////////////////////////////////////////////////
//	HERO the only object binds with a camera
////////////////////////////////////////////////////////////
Hero::Hero(string path) :Object(path, 0x6666){
	t = 0;
	firstMouse = true;

	camera.cameraSpeed = 0.1;
	camera.cameraFront = -front;
	camera.cameraUp = up;
	position = glm::vec3(0.0f, 0.0,5.0f);
	cool = glm::vec3((pModel->minx + pModel->maxx) / 2, (pModel->miny + pModel->maxy) / 2, (pModel->minz + pModel->maxz) / 2);
	cam_pos = glm::vec3(cool.x + 0.05, 1.0f, cool.z + 0.05);
	box_c = glm::vec3();
	//camera.cameraPos = position / scale + cam_pos;
	camera.cameraPos = position + cam_pos;
	heroSpeed = camera.cameraSpeed;
	test_mode = false;
	stand = true;
}

void Hero::Draw(MyShader& omyshader, MyShader& tShader, MyCamera& camera, MyModel* light){
	glm::mat4 temp = model_matrix;
	temp = glm::translate(temp, position);
	temp = glm::translate(temp, cool - glm::vec3(0));
	temp = glm::rotate(temp, -glm::radians(yaw + 90), up);
	temp = glm::rotate(temp, glm::radians(pitch),glm::vec3(1,0,0));
	temp = glm::translate(temp, glm::vec3(0) - cool);
	omyshader.use();
	setCamera(omyshader, camera);
	omyshader.setVec3("viewPos", camera.cameraPos);
	omyshader.setMat4("model", temp);
	omyshader.setFloat("material.shininess", 32.0f);
	omyshader.setFloat("num", light->meshes.size());
	light->SetLight(omyshader);
	pModel->Draw(omyshader);
	if (test_mode){
		draw_test_obb(temp, tShader, camera);
	}
}

void Hero::update_cam_pos(){
	//camera.cameraPos = position / scale + cam_pos;
	camera.cameraPos = position + cam_pos;
}

extern Object* only_used_for_test;
int test_cnt;
void Hero::move(GLFWwindow *window, double xpos, double ypos){
	//printf("%f %f %f\n",position.x, position.y, position.z);
	heroSpeed = 2 * deltaTime;
	if (xpos == 0 && ypos == 0){
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
			forward();
			obb_dirty = 1;
			//camera's coordinate is different from the objects'
			update_cam_pos();
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
			backward();
			obb_dirty = 1;
			//camera's coordinate is different from the objects'
			update_cam_pos();
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
			leftward();
			obb_dirty = 1;
			//camera's coordinate is different from the objects'
			update_cam_pos();
		}

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
			rightward();
			obb_dirty = 1;
			//camera's coordinate is different from the objects'
			update_cam_pos();
		}
	}
	else{
		this->turn(xpos, ypos);
		obb_dirty = 1;
	}
}

void Hero::forward(){
	position -= heroSpeed * front;// *scale;
}

void Hero::backward(){
	position += heroSpeed * front;// *scale;
}

void Hero::leftward(){
	position += heroSpeed * glm::normalize(glm::cross(front, up));// *scale;
}

void Hero::rightward(){
	position -= heroSpeed * glm::normalize(glm::cross(front, up));// *scale;
}


void Hero::turn(double xpos, double ypos){
	if (firstMouse){
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // 注意这里是相反的，因为y坐标是从底部往顶部依次增大的
	/*printf("xoff:%f yoff:%f\n", xoffset, yoffset);*/
	lastX = xpos;
	lastY = ypos;
	xoffset *= camera.MouseSensitivity;
	yoffset *= camera.MouseSensitivity;
	yaw += xoffset;
	pitch += yoffset;
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 f = glm::vec3(1.0f);
	f.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	f.y = sin(glm::radians(pitch));
	f.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	camera.cameraFront = glm::normalize(f);
	front = -glm::normalize(glm::vec3(camera.cameraFront.x, 0, camera.cameraFront.z));
}

////////////////////////////////////////////////////////////
//	ENEMY is ENEMY
////////////////////////////////////////////////////////////
Enemy::Enemy(MyModel* model, int id):Object(model, id, true){
	model_matrix = glm::mat4(1.0f);
	position.x = rand() % 6 - 2.2;
	position.z = rand() % 19 - 21;
	angle = rand() % 360;
	front = glm::vec3(cos((float)angle / 360 * 3.14), 0.0f, sin((float)angle / 360 * 3.14));
	cool = glm::vec3((pModel->minx + pModel->maxx) / 2, (pModel->miny + pModel->maxy) / 2, (pModel->minz + pModel->maxz) / 2);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	hp = 100;
	speed = 0.1;
	live = true;
	move = false;
	dead_counter = 0;
	state = Collider;
	obb_init();
}

Enemy::Enemy(const Enemy* f):Object(){
	pModel = new MyModel();
	pModel->minx = f->pModel->minx;
	pModel->maxx = f->pModel->maxx;
	pModel->miny = f->pModel->miny;
	pModel->maxy = f->pModel->maxy;
	pModel->minz = f->pModel->minz;
	pModel->maxz = f->pModel->maxz;
	front = f->front;
	position = f->position;
	up = f->up;
	angle = f->angle;
	live = f->live;
	model_matrix = f->model_matrix;
	cool = glm::vec3((pModel->minx + pModel->maxx) / 2, (pModel->miny + pModel->maxy) / 2, (pModel->minz + pModel->maxz) / 2);
	obb_init();
}

extern float gg;
extern int ttt;
void Enemy::Draw(MyShader& omyshader, MyShader& deathshader, MyShader& tShader, MyCamera& camera, MyModel* light){
	if (live){
		glm::mat4 temp = model_matrix;
		temp = glm::translate(temp, position);
		temp = glm::translate(temp, cool - glm::vec3(0));
		if (front.x >= 0)
			temp = glm::rotate(temp, acos(front.z), up);
		else
			temp = glm::rotate(temp, -acos(front.z), up);
		temp = glm::translate(temp, glm::vec3(0) - cool);
		omyshader.use();
		setCamera(omyshader, camera);
		omyshader.setVec3("viewPos", camera.cameraPos);
		omyshader.setMat4("model", temp);
		omyshader.setFloat("material.shininess", 32.0f);
		omyshader.setFloat("num", light->meshes.size());
		light->SetLight(omyshader);
		pModel->Draw(omyshader);
		if (test_mode){
			draw_test_obb(temp, tShader, camera);
		}
	}
	else{
		glm::mat4 temp = model_matrix;
		temp = glm::translate(temp, position);
		temp = glm::translate(temp, cool - glm::vec3(0));
		if (front.x >= 0)
			temp = glm::rotate(temp, acos(front.z), up);
		else
			temp = glm::rotate(temp, -acos(front.z), up);
		temp = glm::translate(temp, glm::vec3(0) - cool);
		deathshader.use();
		setCamera(deathshader, camera);
		deathshader.setVec3("viewPos", camera.cameraPos);
		deathshader.setMat4("model", temp);
		deathshader.setFloat("material.shininess", 32.0f);
		deathshader.setFloat("num", light->meshes.size());
		deathshader.setFloat("g", 12);
		deathshader.setFloat("p", 0.18f);
		deathshader.setFloat("deathcount", 0.015 * dead_counter);
		light->SetLight(deathshader);

		pModel->Draw(deathshader);

		
		light->SetLight(deathshader);
		pModel->Draw(deathshader);
	}
}

bool Enemy::hit(){
	hp -= 50;
	if (hp <= 0){
		live = false;
		dead_counter++;
	}
	return true;
}

bool Enemy::dead_animation(){
	if (dead_counter++ < 100){
		return true;
	}
	else{
		return false;
	}
}

////////////////////////////////////////////////////////////
//	set camera
////////////////////////////////////////////////////////////
void setCamera(MyShader& omyShader, MyCamera& camera){
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(camera.cameraPos, camera.cameraPos + camera.cameraFront, camera.cameraUp);
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
	//omyShader.setMat4("model", model);
	omyShader.setMat4("view", view);
	omyShader.setMat4("projection", projection);
}

////////////////////////////////////////////////////////////
//	OBB COLLISION DETECTION
////////////////////////////////////////////////////////////

bool detect_obb(Object* a, Object* b){
	if (a->obb_dirty)
		a->obb_update();
	if (b->obb_dirty)
		b->obb_update();
	float min1, max1, min2, max2;
	for (int i = 0; i < 3; i++){
		get_interval_obb(a, a->surface[i], min1, max1);
		get_interval_obb(b, a->surface[i], min2, max2);
		if (max1 < min2 || max2 < min1) return false;
	}
	for (int i = 0; i < 3; i++){
		get_interval_obb(a, b->surface[i], min1, max1);
		get_interval_obb(b, b->surface[i], min2, max2);
		if (max1 < min2 || max2 < min1) return false;
	}
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++){
			glm::vec3 axis;
			if (axis_equal_obb(a->surface[i], b->surface[j]))
				axis = glm::vec3(0.0f, 0.0f, 0.0f);
			else
				axis = glm::normalize(glm::cross(a->surface[i], b->surface[j]));
			get_interval_obb(a, axis, min1, max1);
			get_interval_obb(b, axis, min2, max2);
			if (max1 < min2 || max2 < min1) return false;
		}
	}
	return true;
}

void get_interval_obb(Object* a, glm::vec3 axis, float& min, float& max){
	float value;
	//分别投影八个点，取最大和最小值  
	min = max = glm::dot(axis, a->vertex[0]);
	for (int i = 1; i < 8; i++){
		value = glm::dot(axis, a->vertex[i]);
		if (value < min) min = value;
		if (value > max) max = value;
	}
}

bool axis_equal_obb(glm::vec3 a, glm::vec3 b){
	if (a.x == b.x)
	if (a.y == b.y)
	if (a.z == b.z)
		return true;
	return false;
}

glm::vec3 intersect_ray_obb(glm::vec3 ray_p, glm::vec3 ray_d, glm::vec3 plane_p, glm::vec3 plane_d){
	float t;
	t = (glm::dot(plane_d, plane_p) - glm::dot(ray_p, plane_d)) / glm::dot(plane_d, ray_d);
	return (ray_p + t * ray_d);
}

bool detect_ray_obj_obb(glm::vec3 ray_p, glm::vec3 ray_d, Object* object){
	if (object->obb_dirty)
		object->obb_update();
	glm::vec3 intersection[6];
	glm::vec3 nothing = glm::vec3(10086, 10086, 10086);
	int flag = 0;
	//line: p + d*t
	//if parralle
	//left & right
	if (glm::normalize(ray_d) == glm::normalize(object->surface[0])){
		flag = 1;
	}
	else{
		intersection[0] = intersect_ray_obb(ray_p, ray_d, object->vertex[1], object->surface[0]);
		intersection[1] = intersect_ray_obb(ray_p, ray_d, object->vertex[0], object->surface[0]);
	}
	//up & down
	if (glm::normalize(ray_d) == glm::normalize(object->surface[1])){
		flag = 2;
	}
	else{
		intersection[2] = intersect_ray_obb(ray_p, ray_d, object->vertex[2], object->surface[1]);
		intersection[3] = intersect_ray_obb(ray_p, ray_d, object->vertex[0], object->surface[1]);
	}
	//front & back
	if (glm::normalize(ray_d) == glm::normalize(object->surface[2])){
		flag = 3;
	}
	else{
		intersection[4] = intersect_ray_obb(ray_p, ray_d, object->vertex[0], object->surface[2]);
		intersection[5] = intersect_ray_obb(ray_p, ray_d, object->vertex[4], object->surface[2]);
	}
	switch (flag){
		case 0:
			if (interval_v3_v3(intersection[0], intersection[1], intersection[2], intersection[3])
				&& interval_v3_v3(intersection[0], intersection[1], intersection[4], intersection[5])
				&& interval_v3_v3(intersection[4], intersection[5], intersection[2], intersection[3])){
				return true;
			}
			break;
		case 1:
			if (interval_v3_v3(intersection[4], intersection[5], intersection[2], intersection[3])){
				return true;
			}
			break;
		case 2:
			if (interval_v3_v3(intersection[4], intersection[5], intersection[0], intersection[1])){
				return true;
			}
			break;
		case 3:
			if (interval_v3_v3(intersection[0], intersection[1], intersection[2], intersection[3])){
				return true;
			}
			break;
		default:
			printf("???\nfatal!\n");
			system("pause");
			exit(0);
	}
	return false;
}

bool interval_v3_v3(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d){
	if (!interval_f_f(a.x, b.x, c.x, d.x))
		return false;
	if (!interval_f_f(a.y, b.y, c.y, d.y))
		return false;
	if (!interval_f_f(a.z, b.z, c.z, d.z))
		return false;
	//all passed
	return true;
}

bool interval_f_f(float ta, float tb, float tc, float td){
	if (ta > tb)
		swap(ta, tb);
	if (tc > td)
		swap(tc, td);
	if (tb < tc)
		return false;
	if (td < ta)
		return false;
	return true;
}

float distance(glm::vec3 a, glm::vec3 b){
	return (((a.x - b.x) * (a.x - b.x)) + ((a.y - b.y) * (a.y - b.y)) + ((a.z - b.z) * (a.z - b.z)));
}

bool detect_ray_enemy_tri(glm::vec3 ray_p, glm::vec3 ray_d, Object* object, float* min){
	float t, u, v;
	float t_min = 0;
	int flag = 1;
	glm::mat4 temp = object->model_matrix;
	temp = glm::translate(temp, object->position);
	temp = glm::translate(temp, object->cool - glm::vec3(0));
	if (object->front.x >= 0)
		temp = glm::rotate(temp, acos(object->front.z), object->up);
	else
		temp = glm::rotate(temp, -acos(object->front.z), object->up);
	temp = glm::translate(temp, glm::vec3(0) - object->cool);
	for (int i = 0; i < object->pModel->meshes.size(); i++){
		int j = 0;
		while (j < object->pModel->meshes[i].indices.size()){
			glm::vec3 pos[3];
			for (int k = 0; k < 3; k++){
				glm::vec4 tt(
					object->pModel->meshes[i].vertices[object->pModel->meshes[i].indices[j + k]].Position.x,
					object->pModel->meshes[i].vertices[object->pModel->meshes[i].indices[j + k]].Position.y,
					object->pModel->meshes[i].vertices[object->pModel->meshes[i].indices[j + k]].Position.z,
					1);
				tt = temp * tt;
				pos[k] = glm::vec3(tt.x, tt.y, tt.z);
			}
			if (flag){
				if (intersect_ray_tri(ray_p, ray_d,
					pos[0],
					pos[1],
					pos[2],
					&t, &u, &v)){
					if (t >= 0){
						t_min = t;
						flag = 0;
					}
				}
			}
			else{
				if (intersect_ray_tri(ray_p, ray_d,
					pos[0],
					pos[1],
					pos[2],
					&t, &u, &v)){
					if (t >= 0){
						t_min = t;
					}
				}
			}
			j += 3;
		}
	}
	*min = t_min;
	if (flag)
		return false;
	else
		return true;
}

bool detect_ray_obj_tri(glm::vec3 ray_p, glm::vec3 ray_d, Object* object, float* min){
	float t, u, v;
	float t_min = 0;
	int flag = 1;
	for (int i = 0; i < object->pModel->meshes.size(); i++){
		int j = 0;
		while (j < object->pModel->meshes[i].indices.size()){
			if (flag){
				if (intersect_ray_tri(ray_p, ray_d,
					object->pModel->meshes[i].vertices[object->pModel->meshes[i].indices[j]].Position,
					object->pModel->meshes[i].vertices[object->pModel->meshes[i].indices[j + 1]].Position,
					object->pModel->meshes[i].vertices[object->pModel->meshes[i].indices[j + 2]].Position,
					&t, &u, &v)){
					if (t >= 0){
						t_min = t;
						flag = 0;
					}
				}
			}
			else{
				if (intersect_ray_tri(ray_p, ray_d,
					object->pModel->meshes[i].vertices[object->pModel->meshes[i].indices[j]].Position,
					object->pModel->meshes[i].vertices[object->pModel->meshes[i].indices[j + 1]].Position,
					object->pModel->meshes[i].vertices[object->pModel->meshes[i].indices[j + 2]].Position,
					&t, &u, &v)){
					if (t >= 0){
						t_min = t;
					}
				}
			}
			j += 3;
		}
	}
	*min = t_min;
	if (flag)
		return false;
	else
		return true;
}

// Determine whether a ray intersect with a triangle
// Parameters
// orig: origin of the ray
// dir: direction of the ray
// v0, v1, v2: vertices of triangle
// t(out): weight of the intersection for the ray
// u(out), v(out): barycentric coordinate of intersection
bool intersect_ray_tri(glm::vec3 orig, glm::vec3 dir, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2,	float* t, float* u, float* v){
	// E1
	glm::vec3 E1 = v1 - v0;

	// E2
	glm::vec3 E2 = v2 - v0;

	// P
	glm::vec3 P = glm::cross(dir, E2);
	// determinant
	float det = glm::dot(E1, P);

	// keep det > 0, modify T accordingly
	glm::vec3 T;
	if (det >0){
		T = orig - v0;
	}
	else{
		T = v0 - orig;
		det = -det;
	}

	// If determinant is near zero, ray lies in plane of triangle
	if (det < 0.0001f)
		return false;

	// Calculate u and make sure u <= 1
	*u = glm::dot(T, P);
	if (*u < 0.0f || *u > det)
		return false;

	// Q
	glm::vec3 Q = glm::cross(T, E1);

	// Calculate v and make sure u + v <= 1
	*v = glm::dot(dir, Q);
	if (*v < 0.0f || *u + *v > det)
		return false;

	// Calculate t, scale parameters, ray intersects triangle
	*t = glm::dot(E2, Q);

	float fInvDet = 1.0f / det;
	*t *= fInvDet;
	*u *= fInvDet;
	*v *= fInvDet;

	return true;
}