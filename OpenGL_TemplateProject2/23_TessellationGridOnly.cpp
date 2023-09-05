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
namespace TessellationGridOnly {
	float toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }

#define numVAOs 1

	//-------------------------------------------Utils util = Utils();
	float cameraX, cameraY, cameraZ;
	float terLocX, terLocY, terLocZ;
	GLuint renderingProgram2;
	GLuint vao2[numVAOs];

	// variable allocation for display
	GLuint mvpLoc;
	int width, height;
	float aspect;
	glm::mat4 pMat, vMat, mMat, mvpMat;


	void window_size_callback(GLFWwindow* win, int newWidth, int newHeight) {
		aspect = (float)newWidth / (float)newHeight;
		glViewport(0, 0, newWidth, newHeight);
		pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
	}
}

using namespace TessellationGridOnly;

void init_TessellationGridOnly(GLFWwindow* window) {
	renderingProgram2 = createShaderProgram(TESSELLATION_GRID_ONLY, ENABLE);
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 120.0f;
	terLocX = 0.0f; terLocY = 0.0f; terLocZ = 0.0f;

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	glGenVertexArrays(numVAOs, vao2);
	glBindVertexArray(vao2[0]);
}

void display_TessellationGridOnly(GLFWwindow* window, double currentTime) {
	glClear(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(renderingProgram2);

	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));

	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(terLocX, terLocY, terLocZ));
	mMat = glm::rotate(mMat, float(60.0f * currentTime) , glm::vec3(1.0f, 1.0f, 1.0f));
	mMat = glm::scale(mMat, glm::vec3(1.0f*sin(currentTime), 1.0f*cos(currentTime), 1.0f * sin(currentTime)));
	mvpMat = pMat * vMat * mMat;

	mvpLoc = glGetUniformLocation(renderingProgram2, "mvp_matrix");

	glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvpMat));

	glFrontFace(GL_CCW);
	//指定补丁顶点
	glPatchParameteri(GL_PATCH_VERTICES, 1);
	//glPolygonMode指定了如何格栅化图形
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // FILL or LINE
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_PATCHES, 0, 16);//16为顶点数
}


int main23(void) {
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(1000, 900, "Chapter 7 - program 1", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	glfwSetWindowSizeCallback(window, window_size_callback);

	init_TessellationGridOnly(window);

	while (!glfwWindowShouldClose(window)) {
		display_TessellationGridOnly(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}