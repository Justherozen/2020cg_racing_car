#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <glad/glad.h>
#include <gl/glut.h>
#include <gl/GL.h>
#include <GLFW/glfw3.h>
#include <ctime>
#include <irrKlang/irrKlang.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/model.h>
#include <learnopengl/shader_m.h>

#include <camera.h>
#include <car.h>
#include <iostream>
#include <gl/glut.h>
#include "mainmenu.h"
#include "carselect.h"
#include<string>
#include"loadobj.h"
using namespace std;
#pragma comment(lib, "irrKlang.lib")
#pragma comment(lib, "glut32.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "assimp.lib")
#define BITMAP_ID 0x4D42
byte* image;          //����ͼ������
FILE* fp;            //�ļ�ָ��
BITMAPFILEHEADER FileHeader;    //����λͼ�ļ�ͷ
BITMAPINFOHEADER InfoHeader;    //����λͼ��Ϣͷ
// ------------------------------------------
// ��������
// ------------------------------------------
int screennum = 1;
irrklang::ISoundEngine* SoundEngine_menu = irrklang::createIrrKlangDevice();
irrklang::ISoundEngine* SoundEngine_coin = irrklang::createIrrKlangDevice();
bool SnapScreen(int width, int height, const char* file);
void collisionDetect(int totalnum,glm::vec3* coinpos);
GLFWwindow* windowInit();
bool init();
void depthMapFBOInit();
void skyboxInit();

void setDeltaTime();
void changeLightPosAsTime();
void changeLightColorAsTime();

// ʹ�á�&���������ܸ���
void renderLight(Shader& shader);
void renderCarAndCamera(Model& carModel, Model& cameraModel, Shader& shader);
void renderCar(Model& model, glm::mat4 modelMatrix, Shader& shader);
void renderCamera(Model& model, glm::mat4 modelMatrix, Shader& shader);
void renderStopSign(Model& model, Shader& shader);
void renderRaceTrack(Model& model, Shader& shader);
void renderTree(Model& model, Shader& shader);
void renderSkyBox(Shader& shader);
void renderCoin(Model& model, Shader& shader, glm::vec3 a);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void handleKeyInput(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void key_callback_win(GLFWwindow* window, int key, int scancode, int action, int mods);
void key_callback_help(GLFWwindow* window, int key, int scancode, int action, int mods);
unsigned int loadCubemap(vector<std::string> faces);

// ------------------------------------------
// ȫ�ֱ���
// ------------------------------------------

// ���ڳߴ�
const unsigned int SCR_WIDTH = 1500;
const unsigned int SCR_HEIGHT = 800;

// ��Ⱦ��Ӱʱ�Ĵ��ڷֱ��ʣ���Ӱ����Ӱ�ľ�ݱ������
const unsigned int SHADOW_WIDTH = 1024 * 10;
const unsigned int SHADOW_HEIGHT = 1024 * 10;

// �Ƿ�Ϊ�߿�ͼģʽ
bool isPolygonMode = false;

// ��������ϵY�ᵥλ����
glm::vec3 WORLD_UP(0.0f, 1.0f, 0.0f);

// ����
Car car(glm::vec3(-5.0f, 0.0f, 18.0f));

// ���
glm::vec3 cameraPos(0.0f, 2.0f, 5.0f);
Camera ourCamera(cameraPos);

// �����������
glm::vec3 lightPos(-1.0f, 1.0f, -1.0f);
glm::vec3 lightCol(1.0f, 1.0f, 1.0f);
glm::vec3 lightDirection = glm::normalize(lightPos);
glm::mat4 lightSpaceMatrix;


glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
};
//glm::vec3 pointLightColors[] = {
//			glm::vec3(0.2f, 0.2f, 0.6f),
//			glm::vec3(0.3f, 0.3f, 0.7f),
//			glm::vec3(0.0f, 0.0f, 0.3f),
//			glm::vec3(0.4f, 0.4f, 0.4f)
//};
// ���Map��ID
unsigned int depthMap;
unsigned int depthMapFBO;

// �������������Ļ����
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing ����ƽ�ⲻͬ������Ⱦˮƽ���������ٶȱ仯
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//����ͼ
int pos = 0;
clock_t curTime = 0;
clock_t lastTime = 0;

// ��պ�
unsigned int cubemapTexture1;
unsigned int cubemapTexture2;
unsigned int cubemapTexture3;
unsigned int cubemapTexture4;
unsigned int skyboxVAO, skyboxVBO;

// ��պж�������
const float skyboxVertices[] = {
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

// ��պе�������
int facesindex = 0;
const vector<std::string> faces_1{
    FileSystem::getPath("asset/textures/skybox2/right.jpg"),
    FileSystem::getPath("asset/textures/skybox2/left.jpg"),
    FileSystem::getPath("asset/textures/skybox2/top.jpg"),
    FileSystem::getPath("asset/textures/skybox2/bottom.jpg"),
    FileSystem::getPath("asset/textures/skybox2/front.jpg"),
    FileSystem::getPath("asset/textures/skybox2/back.jpg")
};
const vector<std::string> faces_2{
	FileSystem::getPath("asset/textures/skybox3/right.jpg"),
	FileSystem::getPath("asset/textures/skybox3/left.jpg"),
	FileSystem::getPath("asset/textures/skybox3/top.jpg"),
	FileSystem::getPath("asset/textures/skybox3/bottom.jpg"),
	FileSystem::getPath("asset/textures/skybox3/front.jpg"),
	FileSystem::getPath("asset/textures/skybox3/back.jpg")
};
const vector<std::string> faces_3{
	FileSystem::getPath("asset/textures/skybox4/right.jpg"),
	FileSystem::getPath("asset/textures/skybox4/left.jpg"),
	FileSystem::getPath("asset/textures/skybox4/top.jpg"),
	FileSystem::getPath("asset/textures/skybox4/bottom.jpg"),
	FileSystem::getPath("asset/textures/skybox4/front.jpg"),
	FileSystem::getPath("asset/textures/skybox4/back.jpg")
};
const vector<std::string> faces_4{
	FileSystem::getPath("asset/textures/skybox5/right.jpg"),
	FileSystem::getPath("asset/textures/skybox5/left.jpg"),
	FileSystem::getPath("asset/textures/skybox5/top.jpg"),
	FileSystem::getPath("asset/textures/skybox5/bottom.jpg"),
	FileSystem::getPath("asset/textures/skybox5/front.jpg"),
	FileSystem::getPath("asset/textures/skybox5/back.jpg")
};

int carindex = 0;
int gamestatus = 0;
int oldgamestatus = -1;
void key_callback_carselect(GLFWwindow* window, int key, int scancode, int action, int mods);
void key_callback_menu(GLFWwindow* window, int key, int scancode, int action, int mods);

//�����ε�������������
float bgvertices[] = {
	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
	1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
};
//�����ε�������������
float carvertices[] = {
	-0.1f, -0.1f, -0.1f,
	0.1f, -0.1f, -0.1f,
	0.1f, 0.1f, -0.1f,
	0.1f, 0.1f, -0.1f,
	-0.1f, 0.1f, -0.1f,
	-0.1f, -0.1f, -0.1f,

	-0.1f, -0.1f, 0.1f,
	0.1f, -0.1f, 0.1f,
	0.1f, 0.1f, 0.1f,
	0.1f, 0.1f, 0.1f,
	-0.1f, 0.1f, 0.1f,
	-0.1f, -0.1f, 0.1f,

	-0.1f, 0.1f, 0.1f,
	-0.1f, 0.1f, -0.1f,
	-0.1f, -0.1f, -0.1f,
	-0.1f, -0.1f, -0.1f,
	-0.1f, -0.1f, 0.1f,
	-0.1f, 0.1f, 0.1f,

	0.1f, 0.1f, 0.1f,
	0.1f, 0.1f, -0.1f,
	0.1f, -0.1f, -0.1f,
	0.1f, -0.1f, -0.1f,
	0.1f, -0.1f, 0.1f,
	0.1f, 0.1f, 0.1f,

	-0.1f, -0.1f, -0.1f,
	0.1f, -0.1f, -0.1f,
	0.1f, -0.1f, 0.1f,
	0.1f, -0.1f, 0.1f,
	-0.1f, -0.1f, 0.1f,
	-0.1f, -0.1f, -0.1f,

	-0.1f, 0.1f, -0.1f,
	0.1f, 0.1f, -0.1f,
	0.1f, 0.1f, 0.1f,
	0.1f, 0.1f, 0.1f,
	-0.1f, 0.1f, 0.1f,
	-0.1f, 0.1f, -0.1f,
};

unsigned int bgindices[] = {
	0,1,2,
	0,2,3,
};

int coinexit[15] = { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };
int coinindex = 0;
int numPointLight = 4;
int main()
{
    // ------------------------------
    // ��ʼ��
    // ------------------------------

    // ���ڳ�ʼ��

	GLFWwindow* window = windowInit();
	// OpenGL��ʼ��
	bool isInit = init();
	if (window == NULL || !isInit) {
		return -1;
	}

		// ���Map��FBO����
		depthMapFBOInit();
		// ��պе�����
		skyboxInit();

		// ------------------------------
		// �����ͱ�����ɫ��
		// ------------------------------

		// Ϊ����������ӹ��պ���Ӱ��shader
		Shader shader("shader/light_and_shadow.vs", "shader/light_and_shadow.fs");
		Shader coinShader("shader/coin.vs", "shader/coin.fs");
		// ��̫��ƽ�й�Ƕ����������Ϣ��shader
		Shader depthShader("shader/shadow_mapping_depth.vs", "shader/shadow_mapping_depth.fs");
		// ��պ�shader
		Shader skyboxShader("shader/skybox.vs", "shader/skybox.fs");

		// ------------------------------
		// ģ�ͼ���
		// ------------------------------

		// ����ģ��
		//Model carModel(FileSystem::getPath("asset/models/obj/Lamborghini/Lamborghini.obj"));
		// ���ģ��
		Model cameraModel(FileSystem::getPath("asset/models/obj/camera-cube/camera-cube.obj"));
		// ����ģ��
		Model raceTrackModel(FileSystem::getPath("asset/models/obj/testrace/marmo_12.obj"));
		// STOP��ģ��
		Model stopSignModel(FileSystem::getPath("asset/models/obj/StopSign/stopsign.obj"));
		// �ϰ�ģ��
		//Model barrierModel(FileSystem::getPath("asset/models/obj/barrier/barrier.obj"));
		// ---------------------------------
		// shader ��������
		// ---------------------------------
		// ��ģ��
		Model treeModels[27] = {
			Model(FileSystem::getPath("asset/models/obj/tree/0.obj")),
			Model(FileSystem::getPath("asset/models/obj/tree/1.obj")),
			Model(FileSystem::getPath("asset/models/obj/tree/2.obj")),
			Model(FileSystem::getPath("asset/models/obj/tree/3.obj")),
			Model(FileSystem::getPath("asset/models/obj/tree/4.obj")),
			Model(FileSystem::getPath("asset/models/obj/tree/5.obj")),
			Model(FileSystem::getPath("asset/models/obj/tree/6.obj")),
			Model(FileSystem::getPath("asset/models/obj/tree/7.obj")),
			Model(FileSystem::getPath("asset/models/obj/tree/8.obj")),
			Model(FileSystem::getPath("asset/models/obj/tree/9.obj")),
			Model(FileSystem::getPath("asset/models/obj/tree/10.obj")),
			Model(FileSystem::getPath("asset/models/obj/tree/11.obj")),
			Model(FileSystem::getPath("asset/models/obj/tree/12.obj")),
			Model(FileSystem::getPath("asset/models/obj/tree/13.obj")),
			Model(FileSystem::getPath("asset/models/obj/tree/14.obj")),
			Model(FileSystem::getPath("asset/models/obj/tree/15.obj")),
			Model(FileSystem::getPath("asset/models/obj/tree/16.obj")),
			Model(FileSystem::getPath("asset/models/obj/tree/17.obj")),
			Model(FileSystem::getPath("asset/models/obj/tree/18.obj")),
			Model(FileSystem::getPath("asset/models/obj/tree/19.obj")),
			Model(FileSystem::getPath("asset/models/obj/tree/20.obj")),
			Model(FileSystem::getPath("asset/models/obj/tree/21.obj")),
			Model(FileSystem::getPath("asset/models/obj/tree/22.obj")),
			Model(FileSystem::getPath("asset/models/obj/tree/23.obj")),
			Model(FileSystem::getPath("asset/models/obj/tree/24.obj")),
			Model(FileSystem::getPath("asset/models/obj/tree/25.obj")),
			Model(FileSystem::getPath("asset/models/obj/tree/26.obj")),
		};

		Shader carShader_1("shader/carselect.vs", "shader/carselect.fs");

		Model carModel_1(FileSystem::getPath("asset/models/obj/Lamborghini/Lamborghini.obj"));
		Model carModel_2(FileSystem::getPath("asset/models/obj/redcar/redcar.obj"));
		//��Ҫʹ����һ������ʹ�����ģ��
		//Model carModel_2(FileSystem::getPath("asset/models/obj/newcar1/a.obj"));
		Model coinModel(FileSystem::getPath("asset/models/obj/dollar_coin/coin.obj"));
		shader.use();
		shader.setInt("diffuseTexture", 0);
		shader.setInt("shadowMap", 15); // �����15��ָ"GL_TEXTURE15"����Ҫ�����Ķ�Ӧ

		skyboxShader.use();
		skyboxShader.setInt("skybox", 0);

		// ---------------------------------
		// ѭ����Ⱦ
		// ---------------------------------
		unsigned int bgVBO, bgVAO, bgEBO;
		glGenVertexArrays(1, &bgVAO);
		glGenBuffers(1, &bgVBO);
		glGenBuffers(1, &bgEBO);

		glBindVertexArray(bgVAO);

		glBindBuffer(GL_ARRAY_BUFFER, bgVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(bgvertices), bgvertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bgEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bgindices), bgindices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);


		int bgshaderProgram = glCreateProgram();
		unsigned int bgtexture;
		bgtexture = BgInit_1(bgshaderProgram);



		unsigned int bgVBO2, bgVAO2, bgEBO2;
		glGenVertexArrays(1, &bgVAO2);
		glGenBuffers(1, &bgVBO2);
		glGenBuffers(1, &bgEBO2);

		glBindVertexArray(bgVAO2);

		glBindBuffer(GL_ARRAY_BUFFER, bgVBO2);
		glBufferData(GL_ARRAY_BUFFER, sizeof(bgvertices), bgvertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bgEBO2);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bgindices), bgindices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		int bgshaderProgram2 = glCreateProgram();
		unsigned int bgtexture2;
		bgtexture2 = BgInit_2(bgshaderProgram2);
	


		////--------------------------------------------------------------
		unsigned int menuVBO, menuVAO, menuEBO;
		glGenVertexArrays(1, &menuVAO);
		glGenBuffers(1, &menuVBO);
		glGenBuffers(1, &menuEBO);

		glBindVertexArray(menuVAO);

		glBindBuffer(GL_ARRAY_BUFFER, menuVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(bgvertices), bgvertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, menuEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bgindices), bgindices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		int menushaderProgram = glCreateProgram();
		unsigned int menutexture;
		menutexture=MenuInit(menushaderProgram);

		unsigned int winVBO, winVAO, winEBO;
		glGenVertexArrays(1, &winVAO);
		glGenBuffers(1, &winVBO);
		glGenBuffers(1, &winEBO);

		glBindVertexArray(winVAO);

		glBindBuffer(GL_ARRAY_BUFFER, winVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(bgvertices), bgvertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, winEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bgindices), bgindices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		int winshaderProgram = glCreateProgram();
		unsigned int wintexture;
		wintexture = WinInit(winshaderProgram);




		unsigned int helpVBO, helpVAO, helpEBO;
		glGenVertexArrays(1, &helpVAO);
		glGenBuffers(1, &helpVBO);
		glGenBuffers(1, &helpEBO);

		glBindVertexArray(helpVAO);

		glBindBuffer(GL_ARRAY_BUFFER, helpVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(bgvertices), bgvertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, helpEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bgindices), bgindices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		int helpshaderProgram = glCreateProgram();
		unsigned int helptexture;
		helptexture = HelpInit(helpshaderProgram);
		/////----------------------------------------------------------------------



		unsigned int carVBO, carVAO;
		glGenVertexArrays(1, &carVAO);
		glGenBuffers(1, &carVBO);
		glBindVertexArray(carVAO);

		glBindBuffer(GL_ARRAY_BUFFER, carVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(carvertices), carvertices, GL_STATIC_DRAW);


		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);


		int carshaderProgram = glCreateProgram();
		CarSelectInit(carshaderProgram);


		



		glEnable(GL_DEPTH_TEST);//�Ƿ���Ȳ���
		SoundEngine_menu->play2D("asset/music/mainmenu.wav", GL_TRUE);
		while (!glfwWindowShouldClose(window)) {
			if (gamestatus == 0) {
				coinindex = 0;
				if (oldgamestatus == -1 || oldgamestatus == 4 || oldgamestatus == 5) {
					SoundEngine_coin->stopAllSounds();
					SoundEngine_menu->stopAllSounds();
					SoundEngine_menu->play2D("asset/music/mainmenu.wav", GL_TRUE);
				}
				oldgamestatus = gamestatus;
				car.CarInit(glm::vec3(-5.0f, 0.0f, 18.0f));
				if(!ourCamera.isFixed())
					ourCamera.setFixed(car.Front);
				glfwPollEvents();
				glfwSetKeyCallback(window, key_callback_menu);
				Drawmainmenu(menutexture,menushaderProgram, menuVAO,window);
				glfwSwapBuffers(window);

			}
			else if (gamestatus == 1) {
				oldgamestatus = gamestatus;
				glfwSetKeyCallback(window, key_callback_carselect);

				glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
				glViewport(0, 0, 1500, 800);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


				switch (carindex)
				{
				case 0: {
					Drawcarselect1(bgtexture, bgshaderProgram, bgVAO, carShader_1, carModel_1, window);

					break; }
				case 1: {
					Drawcarselect2(bgtexture2, bgshaderProgram2, bgVAO2, carShader_1, carModel_2, window);
					
					break;
					}	
				}
				glfwSwapBuffers(window);
				glfwPollEvents();
			}
			else if (gamestatus == 3) {
				oldgamestatus = gamestatus;
				glfwPollEvents();
				glfwSetKeyCallback(window, key_callback_help);
				Drawhelp(helptexture, helpshaderProgram, helpVAO, window);
				glfwSwapBuffers(window);
				
			}
			else if (gamestatus == 4)
			{
				if (oldgamestatus != 4) {
					SoundEngine_menu->stopAllSounds();
					SoundEngine_menu->play2D("asset/music/ingame.wav", GL_TRUE);
				}
				// ����һ֡��ʱ�䳤���Ա���ʹ֡�����ٶȾ���
				oldgamestatus = gamestatus;
				setDeltaTime();
				ourCamera.setDeltaTime();

				// ����ʱ��ı��Դλ��
				if (!ourCamera.isFixed())
				{
					changeLightPosAsTime();
					//changeLightColorAsTime();
				}
					

				// ��������
				handleKeyInput(window);
				ourCamera.CameraMove(window);
				ourCamera.FixView(car.getMidValPosition(), car.Front);

				// ��Ⱦ����
				glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				ourCamera.shakewindow();

				// ---------------------------------
				// ��Ⱦ��ó����������Ϣ
				// ---------------------------------

				// �����Դ�Ӽ��壬����Ӱ���ɷ�Χ������ͶӰ����
				glm::mat4 lightProjection = glm::ortho(
					-200.0f, 200.0f,
					-200.0f, 200.0f,
					-200.0f, 200.0f);
				//lightPos�������λ�ý����ƶ���ʹ�����Χ�ĵط��ܻ�����Ӱ��
				glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), WORLD_UP);
				lightSpaceMatrix = lightProjection * lightView;

				// �ӹ�Դ�Ƕ���Ⱦ��������
				depthShader.use();
				depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

				// �ı��ӿڴ�С�Ա��ڽ�����ȵ���Ⱦ
				glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

				glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
				// ʹ�����shader��Ⱦ���ɳ���
				glClear(GL_DEPTH_BUFFER_BIT);
				
				switch (carindex)
				{
				case 0: {
					renderCarAndCamera(carModel_1, cameraModel, depthShader);
					break;
				}
				case 1: {
					renderCarAndCamera(carModel_2, cameraModel, depthShader);
					break;
				}
				}
				//renderCarAndCamera(carModel_1, cameraModel, depthShader);
			renderRaceTrack(raceTrackModel, depthShader);
			renderStopSign(stopSignModel, depthShader);

			//render the tree
			for (int i = 0; i < 27; i++) {
				renderTree(treeModels[i], depthShader);
			}

			glm::vec3 coinpos[15] = {
				glm::vec3(22.0f, -1.2f, -7.3f),
				glm::vec3(-23.0f, -1.2f, 124.0f),
				glm::vec3(83.0f, -1.2f, 83.0f),
				glm::vec3(96.0f, -1.2f, -105.0f),
				glm::vec3(-8.0f, -1.2f, -151.0f),
				glm::vec3(-89.0f, -1.2f, -115.0f),
				glm::vec3(-92.0f, -1.2f, 156.0f),
				glm::vec3(44.0f, -1.2f, 210.0f),
				glm::vec3(100.0f, -1.2f, 170.0f),
				glm::vec3(-12.0f, -1.2f, 174.0f),
				glm::vec3(-60.0f, -1.2f, 100.0f),
				glm::vec3(-60.0f, -1.2f, -103.0f),
				glm::vec3(44.0f, -1.2f, -119.0f),
				glm::vec3(-25.0f, -1.2f, -27.0f),
				glm::vec3(7.0f, -1.2f, 10.0f)
			};
			int coinnum = 15;	
			renderCoin(coinModel, depthShader, coinpos[coinindex]);
			
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// ��ԭ�ӿ�
			glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// ---------------------------------
			// ģ����Ⱦ
			// ---------------------------------

			shader.use();

			// ���ù����������
			renderLight(shader);

			car.UpdateDelayYaw();
			car.UpdateDelayPosition();

			// ʹ��shader��Ⱦcar��Camera���㼶ģ�ͣ�
			switch (carindex)
			{
			case 0: {
				renderCarAndCamera(carModel_1, cameraModel, shader);
				break;
			}
			case 1: {
				renderCarAndCamera(carModel_2, cameraModel, shader);
				break;
			}
			}
	
			// ��ȾStop��
			renderStopSign(stopSignModel, shader);
			
			renderCoin(coinModel, coinShader, coinpos[coinindex]);
				
			
			collisionDetect(coinnum, coinpos);
			if (coinindex == 15)gamestatus = 5;
			// ��Ⱦ����
			renderRaceTrack(raceTrackModel, shader);
			//renderRaceTrack(barrierModel, shader);

			// ��������Ҷ����
			curTime = clock();
			if (curTime - lastTime >= 90) {
				pos = (pos + 1) % 26;
				lastTime = curTime;
			}
			renderTree(treeModels[pos], shader);

			// --------------
			// �������Ⱦ��պ�

			// �ı���Ȳ��ԣ�ʹ��ȵ���1.0ʱΪ����Զ
			glDepthFunc(GL_LEQUAL);
			skyboxShader.use();
			renderSkyBox(skyboxShader);
			// ��ԭ��Ȳ���
			glDepthFunc(GL_LESS);

			// �����������͵���IO�¼������µİ���,����ƶ��ȣ�
			glfwSwapBuffers(window);

			// ��ѯ�¼�
			glfwPollEvents();
		}
		else if (gamestatus == 5) {
				if (oldgamestatus != 5) {
					SoundEngine_menu->stopAllSounds();
					SoundEngine_menu->play2D("asset/music/win.wav", GL_FALSE);
				}
		oldgamestatus = gamestatus;
		glfwPollEvents();
		glfwSetKeyCallback(window, key_callback_win);
		Drawwin(wintexture, winshaderProgram, winVAO, window);
		glfwSwapBuffers(window);
		
		}
		}
	
    // �ر�glfw
    glfwTerminate();
    return 0;
}

// ------------------------------------------
// ��������
// ------------------------------------------


// ---------------------------------
// ��ʼ��
// ---------------------------------

void collisionDetect(int totalnum,glm::vec3* coinpos){
	if ((coinpos[coinindex].x - car.getMidValPosition().x) * (coinpos[coinindex].x - car.getMidValPosition().x) + (coinpos[coinindex].z - car.getMidValPosition().z) * (coinpos[coinindex].z - car.getMidValPosition().z) < 80.0f) {
		coinindex++;
		
		SoundEngine_coin->play2D("asset/music/yesclick.wav", GL_FALSE);
		
	}
}

GLFWwindow* windowInit()
{
    // ��ʼ������
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // ��������
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, u8"RacingGame��ZJUCGgroup", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        system("pause");
        return NULL;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // ��GLFW��׽�û������
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return window;
}


bool init()
{
    // ��������OpenGL����ָ��
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        system("pause");
        return false;
    }

    // ����ȫ��openGL״̬
    glEnable(GL_DEPTH_TEST);

    return true;
}

// ���ͼ����
void depthMapFBOInit()
{
    glGenFramebuffers(1, &depthMapFBO);
    // �����������
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // �����ɵ����������Ϊ֡�������Ȼ���
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//obj����
void renderTree(Model& model, Shader& shader)
{
	// ��ͼת��
	glm::mat4 viewMatrix = ourCamera.CameraView();
	shader.setMat4("view", viewMatrix);
	// ģ��ת��
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(5.0f, 5.0f, 5.0f));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(4.0f, -0.1f, -4.5f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(-120.0f), WORLD_UP);
	shader.setMat4("model", modelMatrix);
	// ͶӰת��
	glm::mat4 projMatrix = ourCamera.getProjection((float)SCR_WIDTH, (float)SCR_HEIGHT);
	shader.setMat4("projection", projMatrix);

	model.Draw(shader);
}

// ��պ�����
void skyboxInit()
{
    // skybox VAO
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // �������

	cubemapTexture1 = loadCubemap(faces_1);
	
	cubemapTexture2 = loadCubemap(faces_2);

	cubemapTexture3 = loadCubemap(faces_3);

	cubemapTexture4 = loadCubemap(faces_4);

  
}

// ---------------------------------
// ʱ����غ���
// ---------------------------------

// ����һ֡��ʱ�䳤��
void setDeltaTime()
{
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

void changeLightPosAsTime()
{
    float freq = 0.1;
    lightPos.x = 1.0 + cos(glfwGetTime() * freq) * -0.5f;
    lightPos.z = -1.0 + sin(glfwGetTime() * freq) * 0.5f;
    lightPos.y = 1.0 + cos(glfwGetTime() * freq) * 0.5f;
    lightDirection = glm::normalize(lightPos);
	//cout <<"pos:"<< lightDirection[0]<<","<< lightDirection[1] << "," << lightDirection[2] << endl;
}
void changeLightColorAsTime() {
	float freq = 0.1;
	lightCol[0] = lightDirection[0];
	lightCol[1] = lightDirection[1];
	lightCol[2] = -lightDirection[2];
	cout <<"color:"<< lightCol[0]<<","<< lightCol[1] << "," << lightCol[2] << endl;
}

// ---------------------------------
// ��Ⱦ����
// ---------------------------------

// ���ù����������
glm::vec3 pointLightColors[] = {
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f)
};
void renderLight(Shader& shader)
{
    shader.setVec3("viewPos", ourCamera.cameraPos);
    shader.setVec3("lightDirection", lightDirection);
    shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
	
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	
	glm::vec3 pointLightPositions[] = {
	glm::vec3(-18.0f,  3.0f,  18.0f),
	glm::vec3(18.0f,  3.0f,  18.0f),
	glm::vec3(18.0f,  3.0f,  -18.0f),
	glm::vec3(-18.0f,  3.0f,  -18.0f)
	};
	
	shader.setInt("numPointLight", ourCamera.isFixed() ? 0 : numPointLight);
	for (int i = 0; i < 4; i++) {
		std::ostringstream buffer;
		buffer << "pointLights[" << i << "]";
		string str = buffer.str();
		shader.setVec3(str + ".position", pointLightPositions[i]);
		shader.setVec3(str + ".ambient", pointLightColors[i].x * 0.1, pointLightColors[i].y * 0.1, pointLightColors[i].z * 0.1);
		shader.setVec3(str + ".diffuse", pointLightPositions[i]);
		shader.setVec3(str + ".specular", pointLightPositions[i]);
		shader.setFloat(str + ".constant", 1.0f);
		shader.setFloat(str + ".linear", 0.09);
		shader.setFloat(str + ".quadratic", 0.032);
	}
    glActiveTexture(GL_TEXTURE15);
    glBindTexture(GL_TEXTURE_2D, depthMap);
}

void renderCarAndCamera(Model& carModel, Model& cameraModel, Shader& shader)
{
    // ��ͼת��
    glm::mat4 viewMatrix = ourCamera.CameraView();
    shader.setMat4("view", viewMatrix);
    // ͶӰת��
    glm::mat4 projMatrix = ourCamera.getProjection((float)SCR_WIDTH, (float)SCR_HEIGHT);
    shader.setMat4("projection", projMatrix);

    // -------
    // �㼶��ģ

    // ģ��ת��
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, car.getMidValPosition());
    modelMatrix = glm::rotate(modelMatrix, glm::radians(car.getDelayYaw() / 2), WORLD_UP);

    // ��Ⱦ����
    renderCar(carModel, modelMatrix, shader);

    // ����mat4����������ʱΪֵ���ݣ��ʲ���Ҫ����modelMatrix

    // ��Ⱦ���
    //renderCamera(cameraModel, modelMatrix, shader);
}

// ��Ⱦ����
void renderCar(Model& model, glm::mat4 modelMatrix, Shader& shader)
{
    modelMatrix = glm::rotate(modelMatrix, glm::radians(car.getYaw() - car.getDelayYaw() / 2), WORLD_UP);
    // ����ģ��ԭ���Դ�����ת
    modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), WORLD_UP);
    // ����ģ�ʹ�С
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.004f, 0.004f, 0.004f));

    // Ӧ�ñ任����
    shader.setMat4("model", modelMatrix);

    model.Draw(shader);
}

void renderCamera(Model& model, glm::mat4 modelMatrix, Shader& shader)
{
	modelMatrix = glm::rotate(modelMatrix, glm::radians((ourCamera.getYaw() + car.getYaw()) / 2), WORLD_UP);
    modelMatrix = glm::translate(modelMatrix, cameraPos);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.01f, 0.01f, 0.01f));

    // Ӧ�ñ任����
    shader.setMat4("model", modelMatrix);

    model.Draw(shader);
}

void renderStopSign(Model& model, Shader& shader)
{
    // ��ͼת��
    glm::mat4 viewMatrix = ourCamera.CameraView();
    shader.setMat4("view", viewMatrix);
    // ģ��ת��
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(3.0f, 1.5f, -4.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(-120.0f), WORLD_UP);
    shader.setMat4("model", modelMatrix);
    // ͶӰת��
    glm::mat4 projMatrix = ourCamera.getProjection((float)SCR_WIDTH, (float)SCR_HEIGHT);
    shader.setMat4("projection", projMatrix);

    model.Draw(shader);
}
void renderCoin(Model& model, Shader& shader, glm::vec3 a)
{

		glm::mat4 viewMatrix = ourCamera.CameraView();
		shader.setMat4("view", viewMatrix);
		// ģ��ת��
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, a);
		modelMatrix = glm::rotate(modelMatrix, 1 * (float)glfwGetTime(), WORLD_UP);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3,0.3,0.3));
		shader.setMat4("model", modelMatrix);
		// ͶӰת��
		glm::mat4 projMatrix = ourCamera.getProjection((float)SCR_WIDTH, (float)SCR_HEIGHT);
		shader.setMat4("projection", projMatrix);
		
		model.Draw(shader);
	
}


void renderRaceTrack(Model& model, Shader& shader)
{
    // ��ͼת��
    glm::mat4 viewMatrix = ourCamera.CameraView();
    shader.setMat4("view", viewMatrix);
    // ģ��ת��
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    shader.setMat4("model", modelMatrix);
    // ͶӰת��
    glm::mat4 projMatrix = ourCamera.getProjection((float)SCR_WIDTH, (float)SCR_HEIGHT);
    shader.setMat4("projection", projMatrix);

    model.Draw(shader);
}

void renderSkyBox(Shader& shader)
{
    // viewMatrix ͨ�����죬�Ƴ�������ƶ�
    glm::mat4 viewMatrix = glm::mat4(glm::mat3(ourCamera.CameraView()));
    // ͶӰ
    glm::mat4 projMatrix = ourCamera.getProjection((float)SCR_WIDTH, (float)SCR_HEIGHT);

    shader.setMat4("view", viewMatrix);
    shader.setMat4("projection", projMatrix);

    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
	switch (facesindex) {
	case 0: {glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture1);
		break;
	}
	case 1: {glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture2);
		break;
	}
	case 2: {glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture3);
		break;
	}
	case 3: {glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture4);
		break;
	}
	}

    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

	void handleKeyInput(GLFWwindow * window)
	{
		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
			string tmpstr = ("screenshot/screenshot" + to_string(screennum) + ".bmp");
			SnapScreen(1500, 800,tmpstr.c_str());
			screennum ++;
		}

		// �����ƶ�
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			car.ProcessKeyboard(CAR_FORWARD, deltaTime);

			// ֻ�г�����������ʱ��ſ���������ת
			if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
				car.ProcessKeyboard(CAR_LEFT, deltaTime);
			if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
				car.ProcessKeyboard(CAR_RIGHT, deltaTime);

			if (ourCamera.isFixed()) {
				if (glm::abs(glm::dot(glm::normalize(ourCamera.cameraFront), glm::normalize(car.Front)) > 0.5))
					ourCamera.BackEffect();
				else
					ourCamera.ForwardEffect();
			}
		}
		else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			car.ProcessKeyboard(CAR_BACKWARD, deltaTime);

			// ͬ��
			if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
				car.ProcessKeyboard(CAR_LEFT, deltaTime);
			if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
				car.ProcessKeyboard(CAR_RIGHT, deltaTime);

			if (ourCamera.isFixed()) {
				if (glm::abs(glm::dot(glm::normalize(ourCamera.cameraFront), glm::normalize(car.Front)) > 0.5))
					ourCamera.ForwardEffect();
				else
					ourCamera.BackEffect();
			}
		}
		else {
			car.ProcessKeyboard_static(deltaTime);
			if (car.Speed != 0) {
				if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
					car.ProcessKeyboard(CAR_LEFT, deltaTime);
				if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
					car.ProcessKeyboard(CAR_RIGHT, deltaTime);
			}
			//if (car.MovementSpeed!=0&&isCameraFixed)
				//camera.ZoomIn();

		}
		// �ص�����������һ������ֻ�ᴥ��һ���¼���
		glfwSetKeyCallback(window, key_callback);
	}
	
// �����ص�������ʹ��һ�ΰ���ֻ����һ���¼�
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
		ourCamera.setFixed(car.Front);
        string info = ourCamera.isFixed() ? "�л�Ϊ�̶��ӽ�" : "�л�Ϊ�����ӽ�";
        std::cout << "[CAMERA]" << info << std::endl;
    }
    if (key == GLFW_KEY_X && action == GLFW_PRESS) {
        isPolygonMode = !isPolygonMode;
        if (isPolygonMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        string info = isPolygonMode ? "�л�Ϊ�߿�ͼ��Ⱦģʽ" : "�л�Ϊ������Ⱦģʽ";
        std::cout << "[POLYGON_MODE]" << info << std::endl;
    }
	if (key == GLFW_KEY_M && action == GLFW_PRESS) {
		facesindex = (facesindex + 1);
		facesindex = facesindex % 4;
	}
	if (key == GLFW_KEY_N && action == GLFW_PRESS) {
		if (facesindex == 0)facesindex = 4;
		facesindex = (facesindex - 1) % 4;
	}
	if (key == GLFW_KEY_B && action == GLFW_PRESS) {
		gamestatus = 0;
	}
	if (key == GLFW_KEY_Z && action == GLFW_PRESS && (!ourCamera.isFixed())) {
		numPointLight++;
		numPointLight = numPointLight % 5;
	}
	if (key == GLFW_KEY_V && action == GLFW_PRESS ) {	
		const char *fp = "dollar_coin1/coin.obj"; 
		myModel test = loadObj(fp); 
		test.Save("saveobj/coinsave.obj"); 
	}
}

// ����ƶ�
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (!ourCamera.isFixed()) {
        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // ���귭ת�Զ�Ӧ����ϵ

        lastX = xpos;
        lastY = ypos;

		ourCamera.MovewithMouse(xoffset, yoffset);
    }
}

// ������
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	ourCamera.Zoom(yoffset);
}

// ---------------------------------
// ������غ���
// ---------------------------------

// �ı䴰�ڴ�С�Ļص�����
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // ȷ������ƥ����´��ڳߴ�
    glViewport(0, 0, width, height);
}

// ---------------------------------
// ������غ���
// ---------------------------------

// �������������Ϊһ��cubemap����
unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
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


void key_callback_carselect(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action != GLFW_PRESS)
		return;
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, GL_TRUE);
		break;
	case GLFW_KEY_LEFT:
		carindex = (carindex + 1) % 2;
		SoundEngine_coin->play2D("asset/music/menuselect.wav", GL_FALSE);
		break;
	case GLFW_KEY_RIGHT:
		if (carindex == 0)carindex = 2;
		SoundEngine_coin->play2D("asset/music/menuselect.wav", GL_FALSE);
		carindex = (carindex - 1) % 2;
		break;
	case GLFW_KEY_ENTER:
		SoundEngine_coin->play2D("asset/music/yesclick.wav", GL_FALSE);
		gamestatus = 4;
		break;
	default:
		break;
	}
}

void key_callback_menu(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action != GLFW_PRESS)
		return;
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, GL_TRUE);
		break;
	case GLFW_KEY_Q:
		glfwSetWindowShouldClose(window, GL_TRUE);
		break;
	case GLFW_KEY_Y:
		SoundEngine_coin->play2D("asset/music/yesclick.wav", GL_FALSE);
		gamestatus = 1;
		break;
	case GLFW_KEY_H:
		gamestatus = 3;
		break;
	default:
		break;
	}
}

void key_callback_win(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action != GLFW_PRESS)
		return;
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, GL_TRUE);
		break;
	case GLFW_KEY_Q:
		glfwSetWindowShouldClose(window, GL_TRUE);
		break;
	case GLFW_KEY_B:
		gamestatus = 0;
		break;
	default:
		break;
	}
}

void key_callback_help(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action != GLFW_PRESS)
		return;
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, GL_TRUE);
		break;
	case GLFW_KEY_B:
		gamestatus = 0;
		break;
	default:
		break;
	}
}


bool SnapScreen(int width, int height, const char* file)
{
	byte* image;          //����ͼ������
	FILE* fp;            //�ļ�ָ��
	BITMAPFILEHEADER FileHeader;    //����λͼ�ļ�ͷ
	BITMAPINFOHEADER InfoHeader;    //����λͼ��Ϣͷ

	FileHeader.bfType = BITMAP_ID;                                                  //ID����Ϊλͼ��id��
	FileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);      //ʵ��ͼ�����ݵ�λ�����ļ�ͷ����Ϣͷ֮��
	FileHeader.bfReserved1 = 0;                                                    //��������Ϊ0
	FileHeader.bfReserved2 = 0;                                                    //��������Ϊ0
	FileHeader.bfSize = height * width * 24 + FileHeader.bfOffBits;                      //BMPͼ���ļ���С

	InfoHeader.biXPelsPerMeter = 0;                                              //ˮƽ�ֱ��ʣ�������ʱ��Ϊ0����
	InfoHeader.biYPelsPerMeter = 0;                                              //��ֱ�ֱ��ʣ�������ʱ��Ϊ0����
	InfoHeader.biClrUsed = 0;                                                    //ͼ��ʹ�õ���ɫ��������ʱ��Ϊ0����
	InfoHeader.biClrImportant = 0;                                                //��Ҫ����ɫ����������ʱ��Ϊ0����                        //��ֱ�ֱ��ʣ�������ʱ��Ϊ0����
	InfoHeader.biPlanes = 1;                //��������Ϊ1
	InfoHeader.biCompression = BI_RGB;                                              //����ΪBI_RGBʱ,��ʾͼ��û�в�ɫ��
	InfoHeader.biBitCount = 24;                                                    //ͼ���λ��
	InfoHeader.biSize = sizeof(BITMAPINFOHEADER);                                  //�ṹ��Ĵ�С
	InfoHeader.biHeight = height;
	InfoHeader.biWidth = width;
	InfoHeader.biSizeImage = height * width * 4;

	image = (byte*)malloc(sizeof(byte) * InfoHeader.biSizeImage);
	if (image == NULL)
	{
		free(image);
		printf("Exception: No enough space!\n");
		return false;
	}
	//���ظ�ʽ����4�ֽڶ���
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	//���ճ����ص�����
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image);
	for (int i = 0; (i + 2) < InfoHeader.biSizeImage; i++) {
		if (i % 3 == 0) {
			byte tmp = image[i];
			image[i] = image[i + 2];
			image[i + 2] = tmp;
		}
	}
	fp = fopen(file, "wb");
	if (fp == NULL)
	{
		printf("Exception: Fail to open file!\n");
		return false;
	}
	fwrite(&FileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fwrite(&InfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);
	fwrite(image, InfoHeader.biSizeImage, 1, fp);
	free(image);
	fclose(fp);
	return true;
}