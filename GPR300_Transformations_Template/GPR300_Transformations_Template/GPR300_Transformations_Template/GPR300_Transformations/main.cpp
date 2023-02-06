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
float sliderFloat = 0.0f;
float OrbitRadius = 1.0f;
float OrbitSpeed = 2.0f;
float FieldOfView = 5.0f;
float OrthographicHeight = 10.0f;

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

	glm::mat4 roate(glm::vec3 rotation)
	{
		glm::mat4 R = glm::mat4(1);
		glm::mat4 r = glm::mat4(1);
		glm::mat4 u = glm::mat4(1);
		glm::mat4 f = glm::mat4(1);

		r[0][0] = rotation.x;
		R[0][0] = r[0][0];

		u[1][0] = rotation.x;
		R[1][0] = u[1][0];

		f[2][0] = rotation.x;
		R[2][0] = f[2][0];

		r[0][1] = rotation.y;
		R[0][1] = r[0][1];

		u[1][1] = rotation.y;
		R[1][1] = r[1][1];

		f[2][1] = rotation.y;
		R[2][1] = r[2][1];

		r[0][2] = rotation.z;
		R[0][2] = r[0][2];

		u[1][2] = rotation.z;
		R[1][2] = r[1][2];

		f[2][2] = rotation.z;
		R[2][2] = r[2][2];

		R[3][3] = 1;




		return R;
	}

	glm::mat4 rotateX(float x)
	{
		return glm::mat4(1);
	}

	glm::mat4 rotateY(float y)
	{
		return glm::mat4(1);
	}

	glm::mat4 rotateZ(float z)
	{
		return glm::mat4(1);
	}

	glm::mat4 rotateXYZ(float x, float y, float z)
	{
		return rotateX(x) * rotateY(y) * rotateZ(z);
	}

	glm::mat4 translate(glm::vec3 position)
	{
		glm::mat4 m = glm::mat4(1);
		m[3][0] = position.x;
		m[3][1] = position.y;
		m[3][2] = position.z;

		return m;
	}
}

struct Transform
{
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	glm::mat4 getModelMatrix()
	{
		return Function::translate(position) * Function::roate(rotation) * Function::scale(scale);
	}
};

struct Camera
{
	glm::vec3 position;
	glm::vec3 target; //world position to look at
	float fov; //vertical field of view
	float orthographicSize; //height of frustum in view space
	bool orthographic;
	glm::mat4 getViewMatrix()
	{
		return glm::mat4(1);
	}

	glm::mat4 getProjectionMatrix()
	{
		return glm::mat4(1);
	}

	glm::mat4 ortho(float height, float aspectRatio, float nearPlane, float farPlane)
	{
		return glm::mat4(1);
	}

	glm::mat4 perspective(float fov, float aspectRatio, float nearPlane, float farPlane)
	{
		return glm::mat4(1);
	}

	glm::mat4 lookAt(glm::vec3 targetPos, glm::vec3 camPos, glm::vec3 up)
	{
		return glm::mat4(1);
	}
};

const int NUM_CUBE = 5;
Transform transforms[5];
Camera camera;


int main() {
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

		shader.setMat4("_Projection", camera.getProjectionMatrix());

		for (size_t i = 0; i < NUM_CUBE; i++)
		{
			shader.setMat4("_Model", transforms[i].getModelMatrix());
			cubeMesh.draw();
		}

		shader.setMat4("_View", camera.getViewMatrix());
		shader.setMat4("_Projection", camera.getProjectionMatrix());
		
		//Draw UI
		ImGui::Begin("Settings");
		ImGui::SliderFloat("Orbit radius", &OrbitRadius, 0.0f, 100.0f);
		ImGui::SliderFloat("Orbit speed", &OrbitSpeed, 0.0f, 100.0f);
		ImGui::SliderFloat("Field of View", &FieldOfView, 0.0f, 100.0f);
		ImGui::SliderFloat("Orthographic height", &OrthographicHeight, 0.0f, 100.0f);
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