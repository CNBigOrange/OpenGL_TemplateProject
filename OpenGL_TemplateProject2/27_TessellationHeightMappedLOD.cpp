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
namespace TessellationHeightMappedLOD {
	float toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }

#define numVAOs 1

	float cameraX, cameraY, cameraZ;
	float terLocX, terLocY, terLocZ;
	float lightLocX, lightLocY, lightLocZ;
	GLuint renderingProgram2;
	GLuint vao2[numVAOs];

	// variable allocation for display
	GLuint mvpLoc, mvLoc, projLoc, nLoc;
	GLuint globalAmbLoc, ambLoc, diffLoc, specLoc, posLoc, mambLoc, mdiffLoc, mspecLoc, mshiLoc;
	int width, height;
	float aspect;
	glm::mat4 pMat, vMat, mMat, mvMat, mvpMat,invTrMat;
	glm::vec3 currentLightPos;
	float lightPos[3];
	float lightMovement = 0.1f;
	double prevTime = 0.0;

	//white light
	float globalAmbient[4] = { 1.0f,1.0f,1.0f,1.0f };
	float lightAmbient[4] = { 0.4f, 0.4f, 0.4f, 1.0f };
	float lightDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float lightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	float* matAmb = silverAmbient();
	float* matDif = silverDiffuse();
	float* matSpe = silverSpecular();
	float matShi = silverShininess();

	GLuint MoonTexture,MoonHeightTex,MoonNormalTex;

	void window_size_callback(GLFWwindow* win, int newWidth, int newHeight) {
		aspect = (float)newWidth / (float)newHeight;
		glViewport(0, 0, newWidth, newHeight);
		pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
	}
}

using namespace TessellationHeightMappedLOD;

void installLightsHeightMappedLOD(glm::mat4 vMatrix) {
	glm::vec3 transformed = glm::vec3(vMatrix * glm::vec4(currentLightPos, 1.0));
	lightPos[0] = transformed.x;
	lightPos[1] = transformed.y;
	lightPos[2] = transformed.z;

	// get the locations of the light and material fields in the shader
	globalAmbLoc = glGetUniformLocation(renderingProgram2, "globalAmbient");
	ambLoc = glGetUniformLocation(renderingProgram2, "light.ambient");
	diffLoc = glGetUniformLocation(renderingProgram2, "light.diffuse");
	specLoc = glGetUniformLocation(renderingProgram2, "light.specular");
	posLoc = glGetUniformLocation(renderingProgram2, "light.position");
	mambLoc = glGetUniformLocation(renderingProgram2, "material.ambient");
	mdiffLoc = glGetUniformLocation(renderingProgram2, "material.diffuse");
	mspecLoc = glGetUniformLocation(renderingProgram2, "material.specular");
	mshiLoc = glGetUniformLocation(renderingProgram2, "material.shininess");

	//  set the uniform light and material values in the shader
	glProgramUniform4fv(renderingProgram2, globalAmbLoc, 1, globalAmbient);
	glProgramUniform4fv(renderingProgram2, ambLoc, 1, lightAmbient);
	glProgramUniform4fv(renderingProgram2, diffLoc, 1, lightDiffuse);
	glProgramUniform4fv(renderingProgram2, specLoc, 1, lightSpecular);
	glProgramUniform3fv(renderingProgram2, posLoc, 1, lightPos);
	glProgramUniform4fv(renderingProgram2, mambLoc, 1, matAmb);
	glProgramUniform4fv(renderingProgram2, mdiffLoc, 1, matDif);
	glProgramUniform4fv(renderingProgram2, mspecLoc, 1, matSpe);
	glProgramUniform1f(renderingProgram2, mshiLoc, matShi);
}

void init_TessellationHeightMappedLOD(GLFWwindow* window) {
	renderingProgram2 = createShaderProgram(TESSELLATION_HEIGHT_MAPPED_LOD, ENABLE_TESSELLATION);
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 10.0f;
	terLocX = -0.1f; terLocY = 0.0f; terLocZ = 0.0f;
	lightLocX = 0.0f; lightLocY = 0.01f; lightLocZ = 0.2f;

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	MoonTexture = loadTexture("texture/squareMoonMap.jpg");
	MoonHeightTex = loadTexture("texture/squareMoonBump.jpg");
	MoonNormalTex = loadTexture("texture/squareMoonNormal.jpg");
	glGenVertexArrays(numVAOs, vao2);
	glBindVertexArray(vao2[0]);
}

void display_TessellationHeightMappedLOD(GLFWwindow* window, double currentTime) {
	glClear(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(renderingProgram2);

	lightLocX += lightMovement * (currentTime - prevTime);
	if (lightLocX > 0.5) lightMovement = -0.1f;
	else if (lightLocX < -0.5) lightMovement = 0.1f;
	prevTime = currentTime;

	mvpLoc = glGetUniformLocation(renderingProgram2, "mvp_matrix");
	mvLoc = glGetUniformLocation(renderingProgram2, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram2, "proj_matrix");
	nLoc = glGetUniformLocation(renderingProgram2, "norm_matrix");

	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
	vMat = glm::rotate(vMat, float(20), glm::vec3(1.0, 0.0, 0.0));

	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(terLocX+sin(currentTime/200), terLocY, terLocZ));
	mMat = glm::rotate(mMat, float(30.0f) , glm::vec3(0.0f, 1.0f, 0.0f));
	mMat = glm::rotate(mMat, float(2.0f*currentTime), glm::vec3(0.0f, 1.0f, 0.0f));
	mMat = glm::scale(mMat, glm::vec3(1.0f, 1.0f, 1.0f));
	mvMat = vMat * mMat;
	mvpMat = pMat * vMat * mMat;
	invTrMat = glm::transpose(glm::inverse(mvMat));

	currentLightPos = glm::vec3(lightLocX, lightLocY, lightLocZ);
	installLightsHeightMappedLOD(vMat);

	glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvpMat));
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, MoonTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, MoonHeightTex);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, MoonNormalTex);

	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE); 需要关闭背面剔除，因为精度误差某些三角面显示的是背面，将不会被渲染

	glFrontFace(GL_CCW);
	//指定补丁顶点
	glPatchParameteri(GL_PATCH_VERTICES, 4);//四个顶点构建的补丁
	//glPolygonMode指定了如何格栅化图形
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64);//补丁顶点的总数量
}


int main27(void) {
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(1000, 900, "Chapter 7 - program 1", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	glfwSetWindowSizeCallback(window, window_size_callback);

	init_TessellationHeightMappedLOD(window);

	while (!glfwWindowShouldClose(window)) {
		display_TessellationHeightMappedLOD(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}