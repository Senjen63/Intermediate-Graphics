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
glm::vec3 lightColor = glm::vec3(1.0f);
glm::vec3 lightPosition = glm::vec3(0.0f, 3.0f, 0.0f);

bool wireFrame = false;
bool flip = true;

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
	glm::vec3 color = glm::vec3(1, 1, 1);
	float ambientK = 0.25f, diffuseK = 0.5f, specularK = 0.5f; //(0 - 1)
	float shininess = 64.0f;
};

struct TransitionModifier
{
	float startSpeed = 2.7;
	float speed = 1.0;
	float radius = 0.0;
	float blur = 200.0;
	glm::vec3 color = glm::vec3(1.0, 0.0, 0.0);
	float interval = 3.0; //seconds
	float resolution = 0.1;
	float reflection = 0.4;
	float perspective = 0.2;
	float depth = 3.0;
};

struct TransitionStyle
{
	bool isBurn = false;
	bool isLooney = false;
	bool isMelt = false;
	bool isNoise = false;
	bool isSwap = true;
	bool is2DBlock = false;
};

Material material;
DirectionalLight directionalLight;
PointLights pointLights;
SpotLight spotLight;
TransitionModifier transitionM;
TransitionStyle transitionS;

GLuint createTexture(const char* filePath)
{
	GLuint texture = 0;
	int width = 0;
	int height = 0;
	int numComponents = 3;

	stbi_set_flip_vertically_on_load(true);

	unsigned char* textureData = stbi_load(filePath, &width, &height, &numComponents, 0);

	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);

	switch(numComponents)
	{
	case 1:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R, width, height, 0, GL_R, GL_UNSIGNED_BYTE, textureData);
		break;
	case 2:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, width, height, 0, GL_RG, GL_UNSIGNED_BYTE, textureData);
		break;
	case 3:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
		break;
	case 4:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
		break;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glGenerateMipmap(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	/*glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture3);*/



	return texture;
}

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

	Shader TransitionBurnShader("shaders/Transitioning.vert", "TransitionStyle/TransitionBurn.frag");
	Shader TransitionLooneyShader("shaders/Transitioning.vert", "TransitionStyle/TransitionLooney.frag");
	Shader TransitionScreenMeltShader("shaders/Transitioning.vert", "TransitionStyle/TransitionScreenMelt.frag");
	Shader TransitionNoiseShader("shaders/Transitioning.vert", "TransitionStyle/TransitionNoise.frag");
	Shader TransitionSwapShader("shaders/Transitioning.vert", "TransitionStyle/TransitionSwap.frag");
	Shader Transition2DBlockDissolveShader("shaders/Transitioning.vert", "TransitionStyle/Transition2DBlockDissolve.frag");

	//Used to draw light sphere
	Shader unlitShader("shaders/defaultLit.vert", "shaders/unlit.frag");

	const char* woodFloorFile = "Texture/WoodFloor051_1K_Color.png";
	const char* bricksFile = "Texture/Bricks075A_1K_Color.png";
	const char* noise = "Texture/1k_Dissolve_Noise_Texture.png";
	const char* smoke = "Texture/Smoke.png";
	const char* grass = "Texture/Grass001_1K_Color.png";

	GLuint texture = createTexture(woodFloorFile);
	GLuint texture2 = createTexture(noise);
	GLuint texture3 = createTexture(smoke);
	GLuint texture4 = createTexture(bricksFile);
	GLuint texture5 = createTexture(grass);


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
	

	ew::MeshData quadMeshData;
	ew::createQuad(2, 2, quadMeshData);
	ew::Mesh quadMesh(&quadMeshData);

	//Enable back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

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

	/************************************************************************************/
	unsigned int frameBufferObject;
	unsigned int colorBuffer;

	glGenFramebuffers(1, &frameBufferObject);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObject);

	glGenTextures(1, &colorBuffer);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

	unsigned int renderBufferObject;
	glGenRenderbuffers(1, &renderBufferObject);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBufferObject);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, SCREEN_WIDTH, SCREEN_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBufferObject);

	GLenum frameBufferObjectStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (frameBufferObjectStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Frame buffer is not Complete");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	/***************************************************************************************/

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

		/*****************************************************/
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObject);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		/*****************************************************/


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, texture2);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, texture3);

		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, texture4);

		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, texture5);		

		//Draw
		litShader.use();
		litShader.setMat4("_Projection", camera.getProjectionMatrix());
		litShader.setMat4("_View", camera.getViewMatrix());
		litShader.setInt("_WoodFloor", 0);
		litShader.setInt("_Brick", 1);

		litShader.setVec3("_PointLights.position", pointLights.position);
		litShader.setFloat("_PointLights.intensity", pointLights.intensity);
		litShader.setVec3("_PointLights.color", pointLights.color);
		litShader.setFloat("_PointLights.linearAttenuation", pointLights.linearAttenuation);
		litShader.setFloat("_PointLights.quadractic", pointLights.quadractic);

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
		unlitShader.setMat4("_Model", lightTransform.getModelMatrix());
		unlitShader.setVec3("_Color", lightColor);
		sphereMesh.draw();

		lightColor = pointLights.color;
		lightTransform.position = pointLights.position;

		/*******************************************************/
		//Clearing Buffers
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, colorBuffer);
		/*******************************************************/

		
		/***********************************************/
		if (transitionS.is2DBlock && !transitionS.isSwap && !transitionS.isNoise && 
			!transitionS.isBurn && !transitionS.isMelt && !transitionS.isLooney)
		{
			Transition2DBlockDissolveShader.use();

			time = time * transitionM.speed;


			Transition2DBlockDissolveShader.setFloat("_Time", time);
			Transition2DBlockDissolveShader.setFloat("_Resolution", transitionM.resolution);

			quadMesh.draw();
		}

		else if (transitionS.isSwap && !transitionS.isNoise && !transitionS.isBurn && 
				!transitionS.isMelt && !transitionS.isLooney && !transitionS.is2DBlock)
		{
			TransitionSwapShader.use();

			time = time * transitionM.speed;

			TransitionSwapShader.setFloat("_Time", time);
			TransitionSwapShader.setInt("_Texture", 6);
			TransitionSwapShader.setInt("_Texture2", 0);
			TransitionSwapShader.setFloat("_Reflection", transitionM.reflection);
			TransitionSwapShader.setFloat("_Perspective", transitionM.perspective);
			TransitionSwapShader.setFloat("_Depth", transitionM.depth);

			quadMesh.draw();
		}

		else if (transitionS.isNoise && !transitionS.isBurn && !transitionS.isMelt && 
				!transitionS.isLooney && !transitionS.isSwap && !transitionS.is2DBlock)
		{
			TransitionNoiseShader.use();
			TransitionNoiseShader.setInt("_Texture", 2);
			TransitionNoiseShader.setInt("_Blank", 9);
			TransitionNoiseShader.setInt("_Noise", 3);

			TransitionNoiseShader.setFloat("_Time", time);
			TransitionNoiseShader.setFloat("_Speed", transitionM.speed);
			TransitionNoiseShader.setVec3("_Color", transitionM.color);

			quadMesh.draw();
		}

		else if (transitionS.isMelt && !transitionS.isBurn && !transitionS.isNoise && 
				!transitionS.isLooney && !transitionS.isSwap && !transitionS.is2DBlock)
		{
			TransitionScreenMeltShader.use();
			TransitionScreenMeltShader.setInt("_Texture", 2);
			TransitionScreenMeltShader.setInt("_Texture2", 6);

			TransitionScreenMeltShader.setFloat("_Time", time);
			TransitionScreenMeltShader.setFloat("_Speed", transitionM.speed);
			TransitionScreenMeltShader.setFloat("_StartSpeed", transitionM.startSpeed);
			TransitionScreenMeltShader.setFloat("_Restart", transitionM.interval);

			quadMesh.draw();
		}

		else if (transitionS.isLooney && !transitionS.isBurn && !transitionS.isMelt && 
				!transitionS.isNoise && !transitionS.isSwap && !transitionS.is2DBlock)
		{
			TransitionLooneyShader.use();
			TransitionLooneyShader.setInt("_Texture", 2);

			TransitionLooneyShader.setFloat("_Time", time);
			TransitionLooneyShader.setFloat("_Speed", transitionM.speed);
			TransitionLooneyShader.setFloat("_Blur", transitionM.blur);
			TransitionLooneyShader.setFloat("_Radius", transitionM.radius);

			quadMesh.draw();
		}

		else if (transitionS.isBurn && !transitionS.isLooney && !transitionS.isNoise && 
				!transitionS.isMelt && !transitionS.isSwap && !transitionS.is2DBlock)
		{
			TransitionBurnShader.use();
			TransitionBurnShader.setInt("_Texture", 2);

			time = time * transitionM.speed;

			TransitionBurnShader.setFloat("_Time", time);

			quadMesh.draw();
		}
		/*************************************************/

		//Draw UI
		ImGui::Begin("Transitioning style");

		ImGui::Checkbox("Burning", &transitionS.isBurn);
		ImGui::Checkbox("Circle Reveal", &transitionS.isLooney);
		ImGui::Checkbox("Screen Melt", &transitionS.isMelt);
		ImGui::Checkbox("Noise", &transitionS.isNoise);
		ImGui::Checkbox("Swap", &transitionS.isSwap);
		ImGui::Checkbox("2D Block Dissolve", &transitionS.is2DBlock);

		ImGui::End();

		if (transitionS.is2DBlock && !transitionS.isSwap && !transitionS.isNoise && 
			!transitionS.isMelt && !transitionS.isBurn && !transitionS.isLooney)
		{
			ImGui::Begin("2D Block Modifier");

			ImGui::SliderFloat("Speed", &transitionM.speed, 0.0, 10.0);
			ImGui::SliderFloat("Resolution", &transitionM.resolution, 0.0, 1.0);

			ImGui::End();
		}

		else if (transitionS.isSwap && !transitionS.isNoise && !transitionS.isMelt && 
				!transitionS.isBurn && !transitionS.isLooney && !transitionS.is2DBlock)
		{
			ImGui::Begin("Swap Modifier");

			ImGui::SliderFloat("Speed", &transitionM.speed, 0.0, 10.0);
			ImGui::SliderFloat("Reflection", &transitionM.reflection, 0.0, 10.0);
			ImGui::SliderFloat("Perspective", &transitionM.perspective, 0.0, 10.0);
			ImGui::SliderFloat("Depth", &transitionM.depth, 0.0, 10.0);

			ImGui::End();
		}

		else if (transitionS.isNoise && !transitionS.isMelt && !transitionS.isBurn && 
				!transitionS.isLooney && !transitionS.isSwap && !transitionS.is2DBlock)
		{
			ImGui::Begin("Noise Modifier");

			ImGui::SliderFloat("Speed", &transitionM.speed, 0.0, 10.0);
			ImGui::ColorEdit3("Color", &transitionM.color.r);

			ImGui::End();
		}

		else if (transitionS.isMelt && !transitionS.isBurn && !transitionS.isLooney && 
				!transitionS.isNoise && !transitionS.isSwap && !transitionS.is2DBlock)
		{
			ImGui::Begin("Screen Melt Modifier");

			ImGui::SliderFloat("Speed", &transitionM.speed, 0.0, 10.0);
			ImGui::SliderFloat("Start Speed", &transitionM.startSpeed, 0.0, 10.0);
			ImGui::SliderFloat("Restart Interval", &transitionM.interval, 0.0, 9.0);

			ImGui::End();
		}

		else if (transitionS.isLooney && !transitionS.isBurn && !transitionS.isMelt && 
				!transitionS.isNoise && !transitionS.isSwap && !transitionS.is2DBlock)
		{
			ImGui::Begin("Circle Reveal Modifier");

			ImGui::SliderFloat("Speed", &transitionM.speed, 0.0, 10.0);
			ImGui::SliderFloat("Blur", &transitionM.blur, 0.0, 1000.0);
			ImGui::SliderFloat("Radius", &transitionM.radius, -1.0, 1.0);

			ImGui::End();
		}

		else if (transitionS.isBurn && !transitionS.isLooney && !transitionS.isMelt && 
				!transitionS.isNoise && !transitionS.isSwap && !transitionS.is2DBlock)
		{
			ImGui::Begin("Burning Modifier");

			ImGui::SliderFloat("Speed", &transitionM.speed, 0.0, 10.0);

			ImGui::End();
		}		

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
