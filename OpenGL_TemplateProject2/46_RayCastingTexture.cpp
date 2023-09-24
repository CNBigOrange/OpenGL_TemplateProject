#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "CommonLibrary.h"
#include "Utils.h"

using namespace std;
namespace Texture {
#define RAYTRACE_RENDER_WIDTH	800
#define RAYTRACE_RENDER_HEIGHT	800

	int windowWidth = 800;   // or set to RAYTRACE_RENDER_WIDTH
	int windowHeight = 800;  // or set to RAYTRACE_RENDER_HEIGHT

	int workGroupsX = RAYTRACE_RENDER_WIDTH;
	int workGroupsY = RAYTRACE_RENDER_HEIGHT;
	int workGroupsZ = 1;

	GLuint screenTextureID;     // The texture ID of the fullscreen texture 全屏纹理的纹理ID
	unsigned char* screenTexture;   // The screen texture RGBA8888 color data 屏幕纹理的RGBA格式颜色数据
	GLuint offSet;

#define numVAOs 1
#define numVBOs 2
	GLuint vao2[numVAOs];
	GLuint vbo2[numVBOs];

	GLuint screenQuadShader, raytraceComputeShader;
	GLuint brickTexture, moonTexture;

	void setWindowSizeCallback(GLFWwindow* win, int newWidth, int newHeight) {
		glViewport(0, 0, newWidth, newHeight);
	}
}

using namespace Texture;

void init_Texture(GLFWwindow* window) {
	Utils::displayComputeShaderLimits();

	// Allocate the memory for the screen texture, and wipe its contacts
	screenTexture = (unsigned char*)malloc(sizeof(unsigned char) * 4 * RAYTRACE_RENDER_WIDTH * RAYTRACE_RENDER_HEIGHT);
	memset(screenTexture, 0, sizeof(char) * 4 * RAYTRACE_RENDER_WIDTH * RAYTRACE_RENDER_HEIGHT); //将0复制给screenTexture前参数3个字符

	// Setting the initial texture colors to pink to reveal if error 将初始纹理像素颜色设置为粉红色——如果出现粉红色。则该像素可能存在错误
	for (int i = 0; i < RAYTRACE_RENDER_HEIGHT; i++) {
		for (int j = 0; j < RAYTRACE_RENDER_WIDTH; j++) {
			screenTexture[i * RAYTRACE_RENDER_WIDTH * 4 + j * 4 + 0] = 250;
			screenTexture[i * RAYTRACE_RENDER_WIDTH * 4 + j * 4 + 1] = 128;
			screenTexture[i * RAYTRACE_RENDER_WIDTH * 4 + j * 4 + 2] = 255;
			screenTexture[i * RAYTRACE_RENDER_WIDTH * 4 + j * 4 + 3] = 255;
		}
	}

	// Create the OpenGL Texture 创建OpenGL纹理，在该纹理上上对场景进行光线投射
	glGenTextures(1, &screenTextureID);
	glBindTexture(GL_TEXTURE_2D, screenTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, RAYTRACE_RENDER_WIDTH, RAYTRACE_RENDER_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, (const void*)screenTexture);

	// create quad Vertices and texture coordinates for rendering the finished texture to the window
	const float windowQuadVerts[] =
	{ -1.0f, 1.0f, 0.3f,  -1.0f,-1.0f, 0.3f,  1.0f, -1.0f, 0.3f,
		1.0f, -1.0f, 0.3f,  1.0f,  1.0f, 0.3f,  -1.0f,  1.0f, 0.3f
	};
	const float windowQuadUVs[] =
	{ 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
	};

	glGenVertexArrays(1, vao2);
	glBindVertexArray(vao2[0]);
	glGenBuffers(numVBOs, vbo2);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]);  // vertext positions
	glBufferData(GL_ARRAY_BUFFER, sizeof(windowQuadVerts), windowQuadVerts, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[1]);  // texture coordinates
	glBufferData(GL_ARRAY_BUFFER, sizeof(windowQuadUVs), windowQuadUVs, GL_STATIC_DRAW);

	raytraceComputeShader = Utils::createShaderProgram("RayCastingTextureComputeShader.glsl");
	screenQuadShader = Utils::createShaderProgram("RayCastingVertShader.glsl", "RayCastingFragShader.glsl");

	brickTexture = Utils::loadTexture("texture/brick1.jpg");
	moonTexture = Utils::loadTexture("texture/moon.jpg");
}

void displayTexture(GLFWwindow* window, double currentTime) {
	//=======================================================
	// Call the Raytrace compute shader
	//=======================================================
	glUseProgram(raytraceComputeShader);

	offSet = glGetUniformLocation(raytraceComputeShader, "time");
	glUniform1f(offSet, currentTime);

	// Bind the screen_texture_id texture to an image unit as the compute shader's output 将screenTextureID纹理绑定到OpenGL图像单元作为计算着色器的输出
	glBindImageTexture(0, screenTextureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, moonTexture);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, brickTexture);//当同时使用图像存储和纹理时，需要重置激活的纹理

	glActiveTexture(GL_TEXTURE0);

	//启动计算着色器并指定工作组数量
	glDispatchCompute(workGroupsX, workGroupsY, workGroupsZ);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	//=======================================================
	// Call the shader program that draws the resulting texture to the screen
	//=======================================================
	glUseProgram(screenQuadShader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, screenTextureID);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(1);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

int main46(void) {
	int wait;
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Program 44 - simple ray casting", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	glfwSetWindowSizeCallback(window, setWindowSizeCallback);

	init_Texture(window);

	while (!glfwWindowShouldClose(window)) {
		displayTexture(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	std::cin >> wait;
	exit(EXIT_SUCCESS);
}