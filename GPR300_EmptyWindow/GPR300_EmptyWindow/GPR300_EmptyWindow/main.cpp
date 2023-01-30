#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <stdio.h>

//void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void resizeFrameBufferCallback(GLFWwindow* window, int width, int height);

//TODO: Vertex shader source code
const char* vertexShaderSource =
"#version 450                                       \n"
"layout(location = 0) in vec3 vPos;                 \n"
"layout(location = 1) in vec4 vColor;               \n"
"out vec4 Color;                                    \n"
"void main() {                                      \n"
"  Color = vColor;                                  \n"
"  gl_Position = vec4(vPos, 1.0);                   \n"
"}                                                  \0";



//TODO: Fragment shader source code
const char* fragmentShaderSource = 
"#version 450                                       \n"
"out vec4 FragColor;                                \n"
"in vec4 Color;                                     \n"
"uniform float _Time;                               \n"
"void main() {                                      \n"
"  float t = abs(sin(_Time));                       \n"
"  FragColor = Color * t;                           \n"
"}                                                  \0";

//TODO: Vertex data array
const float vertexData[] = 
{
	//x    y     z      color(rgba)
	//Triangle 1
	-0.5, -0.25,  0.0,  1.0, 1.0, 0.0, 1.0,//Bottom Left
	 0.0, -0.25,  0.0,  1.0, 1.0, 0.0, 1.0, //Bottom Right
	-0.25, 0.25,  0.0,  1.0, 1.0, 0.0, 1.0, //Top Center

	////Triangle 2
	 0.0, -0.25,  0.0,  1.0, 1.0, 0.0, 1.0,
	 0.5, -0.25,  0.0,  1.0, 1.0, 0.0, 1.0,
	 0.25, 0.25,  0.0,  1.0, 1.0, 0.0, 1.0,

	//Triangle 3
	 -0.25, 0.25,  0.0,  1.0, 1.0, 0.0, 1.0,
	  0.25, 0.25,  0.0,  1.0, 1.0, 0.0, 1.0,
	  0.0,  0.75,  0.0,  1.0, 1.0, 0.0, 1.0,

	  //Triangle 4
	 -0.25, 0.25,  0.0,  1.0, 3.0, 0.0, 1.0,
	  0.25, 0.25,  0.0,  0.0, 1.0, 2.0, 1.0,
	  0.0, -0.25,  0.0,  3.0, 0.0, 1.0, 1.0,

	  //The Tri-Force (From Legend of Zelda) with a blinding middle
};

int main() {
	if (!glfwInit()) {
		printf("glfw failed to init");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(640, 480, "OpenGLExample", 0, 0);
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		printf("glew failed to init");
		return 1;
	}

	glfwSetFramebufferSizeCallback(window, resizeFrameBufferCallback);

	

	//TODO: Create and compile vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	//TODO: Get vertex shader compilation status and output info log
	GLint success;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLchar infoLog[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		printf("Failed to compile. Vertex Shader Error: %s", infoLog);
	}
	
	
	//TODO: Create and compile fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	//TODO: Get fragment shader compilation status and output info log
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar infoLog[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		printf("Failed to compile. Fragment Shader Error: %s", infoLog);
	}

	//TODO: Create shader program
	GLuint shaderProgram = glCreateProgram();

	//TODO: Attach vertex and fragment shaders to shader program
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	//TODO: Link shader program
	glLinkProgram(shaderProgram);

	//TODO: Check for link status and output errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		GLchar infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		printf("Failed to compile. Shader Program Link Error: %s", infoLog);
	}

	//TODO: Delete vertex + fragment shader objects
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//TODO: Create and bind Vertex Array Object (VAO)
	GLuint vertexArrayObject;
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	//TODO: Create and bind Vertex Buffer Object (VBO), fill with vertexData
	GLuint vertexBufferObject;
	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	//TODO: Define vertex attribute layout
	//Position (3 floats, xyz)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (const void*)0);
	glEnableVertexAttribArray(0);

	//Color (4 floats, rgba)
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (const void*)(sizeof(float)*3));
	glEnableVertexAttribArray(1);

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.2f, 0.3f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		

		//TODO:Use shader program
		glUseProgram(shaderProgram);

		//GLint timeLocation = glGetUniformLocation(shaderProgram, "_Time");
		float time = (float)glfwGetTime();
		glUniform1f(glGetUniformLocation(shaderProgram, "_Time"), time);
		
		//TODO: Draw triangle (3 indices!)
		glDrawArrays(GL_TRIANGLES, 0, 12);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void resizeFrameBufferCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
