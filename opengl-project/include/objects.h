/*draw
		omyShader.use();
		setCamera(omyShader);
		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(0, 0, 0));
		model = glm::scale(model, glm::vec3(scalex, scaley, scalez));
		omyShader.setMat4("model", model);
		omyModel.Draw(omyShader);
		
		//glm::mat4 model = glm::mat4(1.0f);
		//model = glm::scale(glm::mat4(1.0f), glm::vec3(0.005f));
		//model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		*/

#ifndef OBJECTS_H
#define OBJECTS_H

#include "mymodel.h"
#include "myshader.h"
#include "mycamera.h"
#include "time.h"

enum{ Free, Collider, Statical };

extern float deltaTime;
extern int test_mode;

class Object{
public:
	int id;
	string name;
	MyModel* pModel;
	int state;
	bool test_mode;

	//model matrix
	glm::mat4 model_matrix;

	//model position
	glm::vec3 position;
	//model direction
	glm::vec3 front;
	glm::vec3 up;
	float lastX, lastY;
	float yaw, pitch;
	
	//obb detection
	glm::vec3 vertex[8];
	glm::vec3 vertex_ini[8];
	glm::vec3 surface[3];
	int obb_dirty;
	void obb_init();
	void obb_update();

	//obb draw test box
	unsigned int VAO, VBO;
	void draw_test_obb(glm::mat4& model_matrix, MyShader& tShader, MyCamera& camera);
	
	//normalized scale
	float scale;
	
	//default
	Object();
	//use the whole obj file to create a object
	Object(string path, int id);
	//use the MyModel* to create a object
	Object(MyModel* pModel, int id, bool test_m);
	~Object();


	//test
	float t = 0;
	glm::vec3 cool;

	virtual void Draw(MyShader& omyshader, MyShader& tShader, MyCamera& camera);
};

class Hero :public Object{
public:
	MyCamera camera;
	float heroSpeed;

	Hero(string path);

	glm::vec3 cam_pos;
	glm::vec3 box_c;
	void update_cam_pos();
	/*process keyboard and mouse input
	update model_matrix
	update camera*/
	void move(GLFWwindow *window, double xpos, double ypos);
	void forward();
	void backward();
	void leftward();
	void rightward();
	void turn(double xpos, double ypos);

	bool firstMouse = true;
	bool stand = true;
	void Draw(MyShader& omyshader, MyShader& tShader, MyCamera& camera, MyModel* light);

};

class Enemy :public Object{
public:
	bool live, move;
	float dead_counter;
	int hp;
	float speed;
	int angle;
	Enemy(MyModel* model, int id);
	Enemy(const Enemy* f);
	void Draw(MyShader& omyshader, MyShader& deathshader, MyShader& tShader, MyCamera& camera, MyModel* light);
	bool hit();
	bool dead_animation();

};

void setCamera(MyShader& omyShader, MyCamera& camera);

///////////
//obb & obb
///////////
bool detect_obb(Object* a, Object* b);
void get_interval_obb(Object* a, glm::vec3 axis, float& min, float& max);
bool axis_equal_obb(glm::vec3 a, glm::vec3 b);

///////////
//ray & obb
///////////
bool detect_ray_obj_obb(glm::vec3 ray_p, glm::vec3 ray_d, Object* object);
bool detect_ray_ene_obb(glm::vec3 ray_p, glm::vec3 ray_d, Object* object);
//ray & plane
//ray = ray_p + t * ray_d
//plane : plane_d * x = plane_d * plane_p0
glm::vec3 intersect_ray_obb(glm::vec3 ray_p, glm::vec3 ray_d, glm::vec3 plane_p, glm::vec3 plane_d);
//two line segments' interval
bool interval_v3_v3(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d);
bool interval_f_f(float ta, float tb, float tc, float td);
float distance(glm::vec3 a, glm::vec3 b);

/////////////////
//ray & triangles
/////////////////
bool detect_ray_enemy_tri(glm::vec3 ray_p, glm::vec3 ray_d, Object* object, float* min);
bool detect_ray_obj_tri(glm::vec3 ray_p, glm::vec3 ray_d, Object* object, float* min);
// Determine whether a ray intersect with a triangle
bool intersect_ray_tri(glm::vec3 orig, glm::vec3 dir, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, float* t, float* u, float* v);

#endif