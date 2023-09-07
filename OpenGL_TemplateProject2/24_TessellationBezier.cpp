#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "ShapeClass/Sphere.h"
#include "CommonLibrary.h"
using namespace std;
namespace TessellationBezier {
	float toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }

#define numVAOs 1

	//-------------------------------------------Utils util = Utils();
	float cameraX, cameraY, cameraZ;
	float terLocX, terLocY, terLocZ;
	GLuint renderingProgram2;
	GLuint vao2[numVAOs];

	// variable allocation for display
	GLuint mvpLoc, myTime;
	int width, height;
	float aspect;
	glm::mat4 pMat, vMat, mMat, mvpMat;
	GLuint floorTexture;

	void window_size_callback(GLFWwindow* win, int newWidth, int newHeight) {
		aspect = (float)newWidth / (float)newHeight;
		glViewport(0, 0, newWidth, newHeight);
		pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
	}
}

using namespace TessellationBezier;

void init_TessellationBezier(GLFWwindow* window) {
	renderingProgram2 = createShaderProgram(TESSELLATION_BEZIER, ENABLE_TESSELLATION);
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 150.0f;
	terLocX = 0.0f; terLocY = 0.0f; terLocZ = 0.0f;

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	floorTexture = loadTexture("texture/floor_color.jpg");

	glGenVertexArrays(numVAOs, vao2);
	glBindVertexArray(vao2[0]);
}

void display_TessellationBezier(GLFWwindow* window, double currentTime) {
	glClear(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(renderingProgram2);

	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));

	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(terLocX, terLocY, terLocZ));
	mMat = glm::rotate(mMat, float(10.0f) , glm::vec3(1.0f, 1.0f, 0.0f));
	mMat = glm::scale(mMat, glm::vec3(1.0f, 1.0f, 1.0f));
	mvpMat = pMat * vMat * mMat;

	mvpLoc = glGetUniformLocation(renderingProgram2, "mvp_matrix");
	myTime = glGetUniformLocation(renderingProgram2, "currentTime");
	float CcurrentTime = (float)currentTime;
	glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvpMat));
	
	glProgramUniform1f(renderingProgram2, myTime, CcurrentTime);
	//纹理坐标在顶点着色器中计算并传给曲面细分控制着色器
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, floorTexture);

	glFrontFace(GL_CCW);
	//指定补丁顶点
	glPatchParameteri(GL_PATCH_VERTICES, 16);//每个补丁的顶点数量为16
	//glPolygonMode指定了如何格栅化图形
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // FILL or LINE
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//补丁顶点的总数量：16*1=16
	glDrawArrays(GL_PATCHES, 0, 16);
}


int main24(void) {
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(1000, 900, "Chapter 7 - program 1", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	glfwSetWindowSizeCallback(window, window_size_callback);

	init_TessellationBezier(window);

	while (!glfwWindowShouldClose(window)) {
		display_TessellationBezier(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}