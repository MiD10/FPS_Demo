#include "myinclude.h"
#include "myshader.h"
#include "mycamera.h"
#include "mymesh.h"
#include "mymodel.h"
#include "objects.h"
#include "scene.h"

using namespace std;

//test
int test_mode;

//time
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

//a better way?
Scene* controller;

//define a universal camera
MyCamera camera;

//camera selection
int cam_sel = 1;
int test_rotate = 0;
int cnt = 0;
int c_flag = 0;
int c_flagg = 0;
float gg = 0;
int ttt = 0;
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window){
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS){
		if (c_flag == 0) { c_flag = 1; c_flagg = 1; cam_sel ^= 1; }
		/*if (test_rotate == 0) { test_rotate = 1; }*/
		cout << "press_c: ";
	}
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE){
		if (c_flag) { if (c_flagg){ c_flag = 0; cam_sel ^= 1; } }
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !shadowsKeyPressed)
	{
		shadows = !shadows;
		shadowsKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
	{
		shadowsKeyPressed = false;
	}

	switch (cam_sel){
	case 0:
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
				camera.KeyBoardMovement(FORWARD);
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
				camera.KeyBoardMovement(BACKWARD);
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
				camera.KeyBoardMovement(LEFTWARD);
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
				camera.KeyBoardMovement(RIGHTWARD);
		}
		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS){
			printf("x=%f,y=%f,z=%f\n", camera.cameraPos.x, camera.cameraPos.y, camera.cameraPos.z);
		}
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
			gg -= 0.01;
			printf("gg=%f\n", gg);
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
			gg += 0.01;
			printf("gg=%f\n", gg);
		}

		break;
	case 1:
		controller->hero_move(window,0,0);
		break;
	default:
		break;
	}
	
}

//process mouse button
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS) switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		controller->hero_shoot();
		printf("Mosue left button clicked!\n");
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		printf("Mosue middle button clicked!\n");
		ttt += 1;
		ttt %= 2;
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		printf("Mosue right button clicked!\n");
		test_mode++;
		test_mode %= 3;
		switch (test_mode){
		case 0:
			printf("draw all objects!\n"); break;
		case 1:
			printf("draw only the shot one!\n"); break;
		case 2:
			printf("draw only the collided one!\n"); break;
		default:
			printf("wrong test mode!\n");
			system("pause");
			exit(0);
		}
		break;
	default:
		return;
	}
	return;
}

//process for mouse movement
void mouse_callback(GLFWwindow* window, double xpos, double ypos){
	switch (cam_sel){
	case 0:
		camera.MouseMovement(xpos, ypos);
		break;
	case 1:
		controller->hero_move(window, xpos, ypos);
		break;
	default:
		break;
	}
	
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

//skybox
unsigned int loadCubemap(unsigned int* skyboxVAO, unsigned int* skyboxVBO)
{
	//这里采用程序建模的box 手工建模的导入 会有接缝
	float skyboxVertices[] = {
		// positions          
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f
	};

	glGenVertexArrays(1, skyboxVAO);
	glGenBuffers(1, skyboxVBO);
	glBindVertexArray(*skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, *skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	std::vector<std::string> faces
	{
		"skybox/mp_drakeq/right.tga",
		"skybox/mp_drakeq/left.tga",
		"skybox/mp_drakeq/up.tga",
		"skybox/mp_drakeq/down.tga",
		"skybox/mp_drakeq/back.tga",
		"skybox/mp_drakeq/front.tga",
	};
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}
void drawCubemap(unsigned int skyboxVAO, MyShader skyboxShader, unsigned int cubemapTexture){
	glDepthFunc(GL_LEQUAL);
	skyboxShader.use();
	setCamera(skyboxShader, camera);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(120.0f));
	skyboxShader.setMat4("model", model);

	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

//crosshair
void loadCrosshair(unsigned int* crossVAO, unsigned int* crossVBO){
	float vertices[18] = {
		-0.1, 0, 0,
		0.1, 0, 0,
		0, -0.1, 0,
		0, 0.1, 0
	};
	glGenVertexArrays(1, crossVAO);
	glGenBuffers(1, crossVBO);
	glBindVertexArray(*crossVAO);
	glBindBuffer(GL_ARRAY_BUFFER, *crossVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
void drawCrosshair(unsigned int crossVAO, MyShader tShader, Hero* hero){
	tShader.use();
	tShader.setMat4("mdoel", glm::mat4(1.0f));
	tShader.setMat4("view", glm::mat4(1.0f));
	tShader.setMat4("projection", glm::mat4(1.0f));
	glBindVertexArray(crossVAO);
	glDrawArrays(GL_LINES, 0, 4);
	glBindVertexArray(0);
}

int main(void){
	// glfw: initialize and configure
	// ------------------------------
	cout << "INITIALIZING..." << endl;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_SAMPLES, 8);
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	cout << "INITIALIZATION FINISHED" << endl;

	//ENABLE DEPTH
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_MULTISAMPLE);

	//skybox
	unsigned int skyboxVAO, skyboxVBO;
	unsigned int cubemapTexture = loadCubemap(&skyboxVAO,&skyboxVBO);
	MyShader skyboxShader("Shader/skybox.vs.txt", "Shader/skybox.fs.txt");

	//crosshair
	unsigned int crossVAO, crossVBO;
	loadCrosshair(&crossVAO, &crossVBO);

	//shader preparation
	MyShader omyShader("Shader\\VertexShader.gl", "Shader\\FragmentShader.gl");
	MyShader tShader("Shader\\TVS.gl", "Shader\\TFS.gl");
	MyShader light_Shader("Shader\\light.vs.txt", "Shader\\light.fs.txt");
	MyShader deathShader("Shader\\death_vs.gl", "Shader\\death_fs.gl");
	MyShader simpleDepthShader("Shader\\shadow_mapping_depth.vs.txt", "Shader\\shadow_mapping_depth.fs.txt", "Shader\\shadow_mapping_depth.gs.txt");
	//Create Scene
	Scene main_scene;
	controller = &main_scene;

	////Create Object
	main_scene.add_hero("Model/solider76/solider76.obj");
	main_scene.add_map("Model/scene2/scene2.obj",true);
	main_scene.add_light("Model/light/light.obj");
	main_scene.init_enemy_model("Model/snowball/snowball.obj");

	//skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	MyModel object("Model/scene/scene.obj");
	MyModel light("Model/light/light.obj");
	float far_plane = 200.0f;
	float near_plane = 1.0f;
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (int i = 0; i < light.meshes.size(); i++)
	{
		unsigned int &temp = light.meshes[i].depthCubeMap;
		glGenTextures(1, &temp);
		glBindTexture(GL_TEXTURE_CUBE_MAP, temp);
		for (int j = 0; j < 6; j++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, temp, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glm::mat4 shadowProj;
		std::vector<glm::mat4> shadowTransforms;

		float aspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;
		shadowProj = glm::perspective(glm::radians(90.0f), aspect, near_plane, far_plane);

		glm::vec3 lightPos = light.meshes[i].vertices[0].Position;
		shadowTransforms.push_back(shadowProj *	glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj *	glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj *	glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
		shadowTransforms.push_back(shadowProj *	glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
		shadowTransforms.push_back(shadowProj *	glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj *	glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		simpleDepthShader.use();
		for (int i = 0; i < 6; i++)
		{
			string num = std::to_string(i);
			string name = "shadowMatrices[" + num + "]";
			simpleDepthShader.setMat4(name.c_str(), shadowTransforms[i]);
		}
		simpleDepthShader.setFloat("far_plane", far_plane);
		simpleDepthShader.setVec3("lightPos", lightPos);
		glm::mat4 model = glm::mat4(1.0f);
		simpleDepthShader.setMat4("model", model);
		//render scene
		object.Draw(simpleDepthShader);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	srand((unsigned)time(NULL));

	float t = 0;
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		////test
		////only_used_in_mouse_callback->move(window, t+=1000, 0);
		if (main_scene.enemys.size() < 5){
			main_scene.add_enemy();
		}
		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		//draw
		if (cam_sel){
			main_scene.hero->Draw(omyShader, tShader, main_scene.hero->camera, main_scene.lights[0]);
			main_scene.Draw(omyShader, deathShader, tShader, light_Shader, main_scene.hero->camera);
			light_Shader.use();
			glm::mat4 projection = glm::perspective(45.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
			setCamera(light_Shader, main_scene.hero->camera);
			light_Shader.setMat4("model", glm::mat4(1.0f));
			light.Draw(light_Shader);


			omyShader.use();
			setCamera(omyShader, main_scene.hero->camera);
			omyShader.setInt("shadows", shadows);
			omyShader.setMat4("model", glm::mat4(1.0f));
			omyShader.setVec3("viewPos", main_scene.hero->camera.cameraPos);
			//omyShader.setFloat("material.shininess", 32.0f);
			omyShader.setFloat("num", light.meshes.size());
			omyShader.setFloat("far_plane", far_plane);
			light.SetLight(omyShader);
			for (int i = 0; i < light.meshes.size(); i++)
			{
				glActiveTexture(GL_TEXTURE4 + i);
				glBindTexture(GL_TEXTURE_CUBE_MAP, light.meshes[i].depthCubeMap);
			}
			object.Draw(omyShader);
		}
		else{
			main_scene.hero->Draw(omyShader, tShader, camera, main_scene.lights[0]);
			main_scene.Draw(omyShader, deathShader, tShader, light_Shader, camera);
			light_Shader.use();
			glm::mat4 projection = glm::perspective(45.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
			setCamera(light_Shader, camera);
			light_Shader.setMat4("model", glm::mat4(1.0f));
			light.Draw(light_Shader);


			omyShader.use();
			setCamera(omyShader, camera);
			omyShader.setInt("shadows", shadows);
			omyShader.setMat4("model", glm::mat4(1.0f));
			omyShader.setVec3("viewPos", camera.cameraPos);
			//omyShader.setFloat("material.shininess", 32.0f);
			omyShader.setFloat("num", light.meshes.size());
			omyShader.setFloat("far_plane", far_plane);
			light.SetLight(omyShader);
			for (int i = 0; i < light.meshes.size(); i++)
			{
				glActiveTexture(GL_TEXTURE4 + i);
				glBindTexture(GL_TEXTURE_CUBE_MAP, light.meshes[i].depthCubeMap);
			}
			object.Draw(omyShader);
		}
		

		
		drawCubemap(skyboxVAO, skyboxShader, cubemapTexture);
		drawCrosshair(crossVAO, tShader, main_scene.hero);
		main_scene.enemy_update();
		main_scene.hero_update();
		

		//glDepthFunc(GL_LESS);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//system("pause");
	return 0;
}