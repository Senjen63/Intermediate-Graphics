#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "EW/Shader.h"
#include "EW/ShapeGen.h"
#include <random>
#include <time.h>

using namespace std;

void resizeFrameBufferCallback(GLFWwindow* window, int width, int height);
void keyboardCallback(GLFWwindow* window, int keycode, int scancode, int action, int mods);

float lastFrameTime;
float deltaTime;

int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;

double prevMouseX;
double prevMouseY;
bool firstMouseInput = false;

/* Button to lock / unlock mouse
* 1 = right, 2 = middle
* Mouse will start locked. Unlock it to use UI
* */
const int MOUSE_TOGGLE_BUTTON = 1;
const float MOUSE_SENSITIVITY = 0.1f;

glm::vec3 bgColor = glm::vec3(0);


namespace Function
{
	
	glm::mat4 scale(glm::vec3 scaling)
	{
		glm::mat4 s;

		s = glm::mat4(1);

		s[0][0] = scaling.x;
		s[1][1] = scaling.y;
		s[2][2] = scaling.z;
		s[3][3] = 1;
		

		return s;
	}

	glm::mat4 rotateX(float x)
	{
		glm::mat4 pitch = glm::mat4(1);
		
		pitch[0][0] = 1;
		pitch[1][1] = cos(x);
		pitch[1][2] = sin(x);
		pitch[2][1] = -sin(x);
		pitch[2][2] = cos(x);
		pitch[3][3] = 1;

		return pitch;
	}

	glm::mat4 rotateY(float y)
	{
		glm::mat4 yaw = glm::mat4(1);

		yaw[0][0] = cos(y);
		yaw[0][2] = -sin(y);
		yaw[1][1] = 1;
		yaw[2][0] = sin(y);
		yaw[2][2] = cos(y);
		yaw[3][3] = 1;

		return yaw;
	}

	glm::mat4 rotateZ(float z)
	{
		glm::mat4 roll = glm::mat4(1);

		roll[0][0] = cos(z);
		roll[0][1] = sin(z);
		roll[1][0] = -sin(z);
		roll[1][1] = cos(z);
		roll[2][2] = 1;
		roll[3][3] = 1;

		return roll;
	}

	glm::mat4 rotateXYZ(float x, float y, float z)
	{
		return rotateX(x) * rotateY(y) * rotateZ(z);
	}

	glm::mat4 translate(glm::vec3 position)
	{
		glm::mat4 m = glm::mat4(1);
		m[0][0] = 1;
		m[1][1] = 1;
		m[2][2] = 1;
		m[3][0] = position.x;
		m[3][1] = position.y;
		m[3][2] = position.z;
		m[3][3] = 1;


		return m;
	}
}

struct Transform
{
	glm::vec3 position = glm::vec3(0);
	glm::vec3 rotation = glm::vec3(0);
	glm::vec3 scale = glm::vec3(1);

	

	glm::mat4 getModelMatrix()
	{
		return Function::translate(position) * Function::rotateXYZ(rotation.x, rotation.y, rotation.z) * Function::scale(scale);
	}
};

struct Camera
{
	float deltaTime = glfwGetTime();
	float speed = 0.5f;
	float radius = 5.0f;
	glm::vec3 position = glm::vec3(1);
	
	glm::vec3 target = glm::vec3(0, 0, 0); //world position to look at
	float fov = 50.0f; //vertical field of view
	float orthographicSize = 30.0f; //height of frustum in view space
	bool orthographic;
	

	glm::mat4 getViewMatrix()
	{
		deltaTime = glfwGetTime();
		glm::mat4 view = glm::mat4(1);
		glm::vec3 direction = glm::vec3(0.0f, 1.0f, 0.0f);
		position = glm::vec3(cos(deltaTime * speed), 0, sin(deltaTime * speed)) * radius;

		view = lookAt(target, position, direction);



		return view;
	}

	

	glm::mat4 getProjectionMatrix()
	{
		if (orthographic)
		{
			return ortho(orthographicSize, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 1000.0f);
			
		}

		else
		{
			return perspective(fov, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 1000.0f);
		}
	}

	glm::mat4 ortho(float height, float aspectRatio, float nearPlane, float farPlane)
	{
		glm::mat4 o = glm::mat4(1);

		float r = height / 2 * aspectRatio;
		float t = height / 2;
		float l = -r;
		float b = -t;

		o[0][0] = 2 / (r - l);
		o[1][1] = 2 / (t - b);
		o[2][2] = -2 / (farPlane - nearPlane);
		o[3][0] = -(r + l) / (r - l);
		o[3][1] = -(t + b) / (t - b);
		o[3][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
		o[3][3] = 1;

		return o;
	}

	glm::mat4 perspective(float fov, float aspectRatio, float nearPlane, float farPlane)
	{
		fov = glm::radians(fov);
		glm::mat4 p = glm::mat4(1);
		float c = tan(fov / 2);

		p[0][0] = 1 / (aspectRatio * c);
		p[1][1] = 1 / c;
		p[2][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
		p[2][3] = -1;
		p[3][2] = -2 * (farPlane * nearPlane) / (farPlane - nearPlane);
		p[3][3] = 1;

		return p;
	}

	glm::mat4 lookAt(glm::vec3 targetPos, glm::vec3 camPos, glm::vec3 up)
	{
		glm::mat4 look = glm::mat4(1);

		glm::vec3 forward = glm::normalize(targetPos - camPos);
		glm::vec3 right = glm::normalize(glm::cross(forward, up));
		up = glm::cross(right, forward);

		forward = -forward;

		

		look[0][0] = right.x;
		look[1][0] = right.y;
		look[2][0] = right.z;
		look[0][1] = up.x;
		look[1][1] = up.y;
		look[2][1] = up.z;
		look[0][2] = forward.x;
		look[1][2] = forward.y;
		look[2][2] = forward.z;

		glm::mat4 translate = Function::translate(-camPos);

		return look * translate;
	}
};

const int NUM_CUBE = 5;
Transform transforms[5];
Camera camera;
float sliderFloat = 0.0f;
float OrbitRadius = 1.0f;
float OrbitSpeed = 5.0f;


int main() {
	srand(time(NULL));
	if (!glfwInit()) {
		printf("glfw failed to init");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Transformations", 0, 0);
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		printf("glew failed to init");
		return 1;
	}

	glfwSetFramebufferSizeCallback(window, resizeFrameBufferCallback);
	glfwSetKeyCallback(window, keyboardCallback);

	// Setup UI Platform/Renderer backends
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	//Dark UI theme.
	ImGui::StyleColorsDark();

	Shader shader("shaders/vertexShader.vert", "shaders/fragmentShader.frag");

	MeshData cubeMeshData;
	createCube(1.0f, 1.0f, 1.0f, cubeMeshData);

	Mesh cubeMesh(&cubeMeshData);

	//Enable back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	for (int j = 0; j < NUM_CUBE; j++)
	{
		float pX = rand() % 20 - 10;
		float pY = rand() % 20 - 10;
		float pZ = rand() % 30 - 10;

		float rX = rand() % 360 + 0;
		float rY = rand() % 360 + 0;
		float rZ = rand() % 360 + 0;

		transforms[j].position = glm::vec3(pX, pY, pZ);
		transforms[j].rotation = glm::vec3(rX, rY, rZ);
	}

	while (!glfwWindowShouldClose(window)) {
		glClearColor(bgColor.r,bgColor.g,bgColor.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		float time = (float)glfwGetTime();
		deltaTime = time - lastFrameTime;
		lastFrameTime = time;

		//Draw
		shader.use();
		shader.setMat4("_View", camera.getViewMatrix());
		shader.setMat4("_Projection", camera.getProjectionMatrix());

		for (size_t i = 0; i < NUM_CUBE; i++)
		{
			shader.setMat4("_Model", transforms[i].getModelMatrix());
			//shader.setMat4("_Model", glm::mat4(1));
			cubeMesh.draw();
		}

		
		
		//Draw UI
		ImGui::Begin("Settings");
		ImGui::SliderFloat("Orbit radius", &camera.radius, 0.0f, 100.0f);
		ImGui::SliderFloat("Orbit speed", &camera.speed, 0.0f, 10.0f);
		ImGui::SliderFloat("Field of View", &camera.fov, 0.0f, 100.0f);
		ImGui::SliderFloat("Orthographic height", &camera.orthographicSize, 0.0f, 100.0f);
		ImGui::Checkbox("Orthographic toggle", &camera.orthographic);
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwPollEvents();

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

void resizeFrameBufferCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
}

void keyboardCallback(GLFWwindow* window, int keycode, int scancode, int action, int mods)
{
	if (keycode == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}