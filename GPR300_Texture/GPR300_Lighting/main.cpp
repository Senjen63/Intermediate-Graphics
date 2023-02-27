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
#include "EW/EwMath.h"
#include "EW/Camera.h"
#include "EW/Mesh.h"
#include "EW/Transform.h"
#include "EW/ShapeGen.h"

void processInput(GLFWwindow* window);
void resizeFrameBufferCallback(GLFWwindow* window, int width, int height);
void keyboardCallback(GLFWwindow* window, int keycode, int scancode, int action, int mods);
void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void mousePosCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

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
const float CAMERA_MOVE_SPEED = 5.0f;
const float CAMERA_ZOOM_SPEED = 3.0f;

Camera camera((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);

glm::vec3 bgColor = glm::vec3(0);
//glm::vec3 lightColor = glm::vec3(1.0f);
//glm::vec3 lightColor2 = glm::vec3(1.0f);

glm::vec3 lightColor;


bool wireFrame = false;
const int MAX_LIGHTS = 3;

struct DirectionalLight
{
	glm::vec3 color = glm::vec3(1, 0, 0);
	glm::vec3 direction = glm::vec3(0, 1, 0);
	float intensity = 1.0f;
	//linear and spotlight
};

struct PointLights
{
	glm::vec3 color = glm::vec3(1, 0, 0);
	glm::vec3 position = glm::vec3(1, 1, 0);
	float intensity = 1.0f;
	float linearAttenuation = 0.22f;
	float quadractic = 0.2f;
	//linear
};

struct SpotLight
{
	glm::vec3 color = glm::vec3(1, 0, 0);
	glm::vec3 position = glm::vec3(0, 1, 0);
	glm::vec3 direction = glm::vec3(0, 1, 0);
	float intensity = 1.0f;
	float linearAttenuation = 1.0f;
	float quadractic = 1.0f;
	float minAngle = 1.0f;
	float maxAngle = 1.0f;
	float angleFallOff = 2.0f;
	//linear and spotlight
};

struct Material
{
	glm::vec3 color = glm::vec3(1, 0, 0);
	float ambientK = 0.25f, diffuseK = 0.5f, specularK = 0.5f; //(0 - 1)
	float shininess = 64.0f;
};



Material material;
DirectionalLight directionalLight;
//PointLights pointLights[MAX_LIGHTS];
PointLights pointLights;
SpotLight spotLight;



int main() {
	if (!glfwInit()) {
		printf("glfw failed to init");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Lighting", 0, 0);
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		printf("glew failed to init");
		return 1;
	}

	glfwSetFramebufferSizeCallback(window, resizeFrameBufferCallback);
	glfwSetKeyCallback(window, keyboardCallback);
	glfwSetScrollCallback(window, mouseScrollCallback);
	glfwSetCursorPosCallback(window, mousePosCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);

	//Hide cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Setup UI Platform/Renderer backends
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	//Dark UI theme.
	ImGui::StyleColorsDark();

	//Used to draw shapes. This is the shader you will be completing.
	Shader litShader("shaders/defaultLit.vert", "shaders/defaultLit.frag");

	//Used to draw light sphere
	Shader unlitShader("shaders/defaultLit.vert", "shaders/unlit.frag");

	ew::MeshData cubeMeshData;
	ew::createCube(1.0f, 1.0f, 1.0f, cubeMeshData);
	ew::MeshData sphereMeshData;
	ew::createSphere(0.5f, 64, sphereMeshData);
	ew::MeshData cylinderMeshData;
	ew::createCylinder(1.0f, 0.5f, 64, cylinderMeshData);
	ew::MeshData planeMeshData;
	ew::createPlane(1.0f, 1.0f, planeMeshData);

	ew::Mesh cubeMesh(&cubeMeshData);
	ew::Mesh sphereMesh(&sphereMeshData);
	ew::Mesh planeMesh(&planeMeshData);
	ew::Mesh cylinderMesh(&cylinderMeshData);

	//Enable back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	int numLight = 1;

	//Initialize shape transforms
	ew::Transform cubeTransform;
	ew::Transform sphereTransform;
	ew::Transform planeTransform;
	ew::Transform cylinderTransform;
	ew::Transform lightTransform;

	cubeTransform.position = glm::vec3(-2.0f, 0.0f, 0.0f);
	sphereTransform.position = glm::vec3(0.0f, 0.0f, 0.0f);

	planeTransform.position = glm::vec3(0.0f, -1.0f, 0.0f);
	planeTransform.scale = glm::vec3(10.0f);

	cylinderTransform.position = glm::vec3(2.0f, 0.0f, 0.0f);

	lightTransform.scale = glm::vec3(0.5f);
	lightTransform.position = glm::vec3(0.0f, 5.0f, 0.0f);
	
	

	

	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		glClearColor(bgColor.r,bgColor.g,bgColor.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		float time = (float)glfwGetTime();
		deltaTime = time - lastFrameTime;
		lastFrameTime = time;

		//UPDATE
		cubeTransform.rotation.x += deltaTime;
		
		//Draw
		litShader.use();
		litShader.setMat4("_Projection", camera.getProjectionMatrix());
		litShader.setMat4("_View", camera.getViewMatrix());
		litShader.setInt("_NumberOfLight", numLight);
		
		//Point Light Uniforms
		litShader.setVec3("_PointLights.position", pointLights.position);
		litShader.setFloat("_PointLights.intensity", pointLights.intensity);
		litShader.setVec3("_PointLights.color", pointLights.color);
		litShader.setFloat("_PointLights.linearAttenuation", pointLights.linearAttenuation);
		litShader.setFloat("_PointLights.quadractic", pointLights.quadractic);

		//Couldn't figure out array problem
		/*for (size_t i = 0; i < numLight; i++)
		{
			litShader.setVec3("_PointLights[" + std::to_string(i) + "].position", pointLights[i].position);
			litShader.setFloat("_PointLights[" + std::to_string(i) + "].intensity", pointLights[i].intensity);
			litShader.setVec3("_PointLights[" + std::to_string(i) + "].color", pointLights[i].color);
			litShader.setFloat("_PointLights[" + std::to_string(i) + "].linearAttenuation", pointLights[i].linearAttenuation);
			litShader.setFloat("_PointLights[" + std::to_string(i) + "].quadractic", pointLights[i].quadractic);
		}*/

		//Directional Light Uniforms
		litShader.setVec3("_DirectionalLight.direction", directionalLight.direction);
		litShader.setFloat("_DirectionalLight.intensity", directionalLight.intensity);
		litShader.setVec3("_DirectionalLight.color", directionalLight.color);
		
		//Spot Light Uniforms
		litShader.setVec3("_SpotLight.position", spotLight.position);
		litShader.setFloat("_SpotLight.intensity", spotLight.intensity);
		litShader.setVec3("_SpotLight.color", spotLight.color);
		litShader.setVec3("_SpotLight.direction", spotLight.direction);
		litShader.setFloat("_SpotLight.linearAttenuation", spotLight.linearAttenuation);
		litShader.setFloat("_SpotLight.quadractic", spotLight.quadractic);
		litShader.setFloat("_SpotLight.minAngle", spotLight.minAngle);
		litShader.setFloat("_SpotLight.maxAngle", spotLight.maxAngle);
		litShader.setFloat("_SpotLight.angleFallOff", spotLight.angleFallOff);

		//Material Uniforms
		litShader.setVec3("_Material.color", material.color);
		litShader.setFloat("_Material.ambientK", material.ambientK);
		litShader.setFloat("_Material.diffuseK", material.diffuseK);
		litShader.setFloat("_Material.specularK", material.specularK);
		litShader.setFloat("_Material.shininess", material.shininess);
		

		//conversion
		//for angle to cosine of angle for Spot Light
		//cosines = radians
		//convert degrees to radians to the cosine to them
		//dot product = cosine
	
		
		//Draw cube
		litShader.setMat4("_Model", cubeTransform.getModelMatrix());
		cubeMesh.draw();

		//Draw sphere
		litShader.setMat4("_Model", sphereTransform.getModelMatrix());
		sphereMesh.draw();

		//Draw cylinder
		litShader.setMat4("_Model", cylinderTransform.getModelMatrix());
		cylinderMesh.draw();

		//Draw plane
		litShader.setMat4("_Model", planeTransform.getModelMatrix());
		planeMesh.draw();

		//Draw light as a small sphere using unlit shader, ironically.
		unlitShader.use();
		unlitShader.setMat4("_Projection", camera.getProjectionMatrix());
		unlitShader.setMat4("_View", camera.getViewMatrix());

		//Couldn't figure out array problem
		/*for (int l = 0; l < numLight; l++)
		{
			unlitShader.setMat4("_Model", lightTransform[l].getModelMatrix());
		}*/

		unlitShader.setMat4("_Model", lightTransform.getModelMatrix());
		
		//Couldn't figure out array problem
		/*for (int i = 0; i < numLight; i++)
		{
			unlitShader.setVec3("_Color", lightColor[i]);
		}*/

		unlitShader.setVec3("_Color", lightColor);
		
		

		sphereMesh.draw();

		//Couldn't figure out array problem
		/*lightColor = pointLights[0].color;
		lightColor2 = pointLights[1].color;*/

		/*for (int i = 0; i < numLight; i++)
		{
			lightColor[i] = pointLights[i].color;
		}

		for (int i = 0; i < numLight; i++)
		{
			lightTransform.position = pointLights[i].position;
		}*/

		lightColor = pointLights.color;
		

		lightTransform.position = pointLights.position;
		
		
		
		
		
		//Draw UI
		ImGui::Begin("Material");
		ImGui::ColorEdit3("Material Color", &material.color.r);
		ImGui::SliderFloat("Material Ambient K", &material.ambientK, 0.0f, 1.0f);
		ImGui::SliderFloat("Material Diffuse K", &material.diffuseK, 0.0f, 1.0f);
		ImGui::SliderFloat("Material Specular K", &material.specularK, 0.0f, 1.0f);
		ImGui::SliderFloat("Material Shininess", &material.shininess, 2.0f, 512.0f);
		ImGui::End();

		ImGui::Begin("Directional Light");
		ImGui::DragFloat3("Directional Light Direction", &directionalLight.direction.x);
		ImGui::SliderFloat("Directional Light Intensity", &directionalLight.intensity, 0.0f, 1.0f);
		ImGui::ColorEdit3("Directional Light Color", &directionalLight.color.r);
		ImGui::End();

		ImGui::Begin("Point Lights");
		//ImGui::SliderInt("Number of Point Lights", &numLight, 0.0f, 2.0f);

		ImGui::SliderFloat("Point Light Intensity", &pointLights.intensity, 0.0f, 1.0f);
		ImGui::SliderFloat("Point Light Linear", &pointLights.linearAttenuation, 0.0f, 1.0f);
		ImGui::SliderFloat("Point Light Quadratic", &pointLights.quadractic, 0.0f, 1.0f);
		ImGui::DragFloat3("Point Light position", &pointLights.position.x);
		ImGui::ColorEdit3("Point Light Color", &pointLights.color.r);
		
		//Couldn't figure out array problem
		/*for (int p = 0; p < numLight; p++)
		{
			ImGui::SliderFloat("Point Light Intensity", &pointLights[p].intensity, 0.0f, 1.0f);
			ImGui::SliderFloat("Point Light Linear", &pointLights[p].linearAttenuation, 0.0f, 1.0f);
			ImGui::SliderFloat("Point Light Quadratic", &pointLights[p].quadractic, 0.0f, 1.0f);
			ImGui::DragFloat3("Point Light position", &pointLights[p].position.x);
			ImGui::ColorEdit3("Point Light Color", &pointLights[p].color.r);
		}*/

		ImGui::End();

		ImGui::Begin("Spot Light");
		ImGui::DragFloat3("Spot Light Position", &spotLight.position.x);
		ImGui::DragFloat3("Spot Light Direction", &spotLight.direction.x);
		ImGui::SliderFloat("Spot Light Intensity", &spotLight.intensity, 0.0f, 1.0f);
		ImGui::ColorEdit3("Spot Light Color", &spotLight.color.r);
		ImGui::SliderFloat("Spot Light Inner Angle", &spotLight.minAngle, 0.0f, 100.0f);
		ImGui::SliderFloat("Spot Light Outer Angle", &spotLight.maxAngle, 0.0f, 100.0f);
		ImGui::SliderFloat("Spot Light Angle Falloff", &spotLight.angleFallOff, 0.0f, 100.0f);
		ImGui::SliderFloat("Spot Light Linear", &spotLight.linearAttenuation, 0.0f, 100.0f);
		ImGui::SliderFloat("Spot Light Quadractic", &spotLight.quadractic, 0.0f, 100.0f);
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwPollEvents();

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}
//Author: Eric Winebrenner
void resizeFrameBufferCallback(GLFWwindow* window, int width, int height)
{
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
	camera.setAspectRatio((float)SCREEN_WIDTH / SCREEN_HEIGHT);
	glViewport(0, 0, width, height);
}
//Author: Eric Winebrenner
void keyboardCallback(GLFWwindow* window, int keycode, int scancode, int action, int mods)
{
	if (keycode == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	//Reset camera
	if (keycode == GLFW_KEY_R && action == GLFW_PRESS) {
		camera.setPosition(glm::vec3(0, 0, 5));
		camera.setYaw(-90.0f);
		camera.setPitch(0.0f);
		firstMouseInput = false;
	}
	if (keycode == GLFW_KEY_1 && action == GLFW_PRESS) {
		wireFrame = !wireFrame;
		glPolygonMode(GL_FRONT_AND_BACK, wireFrame ? GL_LINE : GL_FILL);
	}
}
//Author: Eric Winebrenner
void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (abs(yoffset) > 0) {
		float fov = camera.getFov() - (float)yoffset * CAMERA_ZOOM_SPEED;
		camera.setFov(fov);
	}
}
//Author: Eric Winebrenner
void mousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED) {
		return;
	}
	if (!firstMouseInput) {
		prevMouseX = xpos;
		prevMouseY = ypos;
		firstMouseInput = true;
	}
	float yaw = camera.getYaw() + (float)(xpos - prevMouseX) * MOUSE_SENSITIVITY;
	camera.setYaw(yaw);
	float pitch = camera.getPitch() - (float)(ypos - prevMouseY) * MOUSE_SENSITIVITY;
	pitch = glm::clamp(pitch, -89.9f, 89.9f);
	camera.setPitch(pitch);
	prevMouseX = xpos;
	prevMouseY = ypos;
}
//Author: Eric Winebrenner
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	//Toggle cursor lock
	if (button == MOUSE_TOGGLE_BUTTON && action == GLFW_PRESS) {
		int inputMode = glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED;
		glfwSetInputMode(window, GLFW_CURSOR, inputMode);
		glfwGetCursorPos(window, &prevMouseX, &prevMouseY);
	}
}

//Author: Eric Winebrenner
//Returns -1, 0, or 1 depending on keys held
float getAxis(GLFWwindow* window, int positiveKey, int negativeKey) {
	float axis = 0.0f;
	if (glfwGetKey(window, positiveKey)) {
		axis++;
	}
	if (glfwGetKey(window, negativeKey)) {
		axis--;
	}
	return axis;
}

//Author: Eric Winebrenner
//Get input every frame
void processInput(GLFWwindow* window) {

	float moveAmnt = CAMERA_MOVE_SPEED * deltaTime;

	//Get camera vectors
	glm::vec3 forward = camera.getForward();
	glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0,1,0)));
	glm::vec3 up = glm::normalize(glm::cross(forward, right));

	glm::vec3 position = camera.getPosition();
	position += forward * getAxis(window, GLFW_KEY_W, GLFW_KEY_S) * moveAmnt;
	position += right * getAxis(window, GLFW_KEY_D, GLFW_KEY_A) * moveAmnt;
	position += up * getAxis(window, GLFW_KEY_Q, GLFW_KEY_E) * moveAmnt;
	camera.setPosition(position);
}
