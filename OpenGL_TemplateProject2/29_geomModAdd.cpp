#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "ShapeClass/Sphere.h"
#include "Utils.h"
#include "CommonLibrary.h"
using namespace std;
namespace geomModAdd {
	float toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }

#define numVAOs 1
#define numVBOs 4

	float cameraX, cameraY, cameraZ;
	float torLocX, torLocY, torLocZ;
	GLuint renderingProgram2;
	GLuint vao2[numVAOs];
	GLuint vbo2[numVBOs];

	Sphere myTorus(48);
	int numTorusVertices = myTorus.getNumVertices();
	int numTorusIndices = myTorus.getNumIndices();

	glm::vec3 initialLightLoc = glm::vec3(5.0f, 2.0f, 2.0f);
	float amt = 0.0f;

	// variable allocation for display
	GLuint mvLoc, projLoc, nLoc, lLoc;
	GLuint globalAmbLoc, ambLoc, diffLoc, specLoc, posLoc, mambLoc, mdiffLoc, mspecLoc, mshiLoc;
	int width, height;
	float aspect;
	glm::mat4 pMat, vMat, mMat, mvMat, invTrMat, rMat;
	glm::vec3 currentLightPos, transformed;
	float lightPos[3];

	// white light
	float globalAmbient[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
	float lightAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float lightDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float lightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	// gold material
	float* matAmb = Utils::goldAmbient();
	float* matDif = Utils::goldDiffuse();
	float* matSpe = Utils::goldSpecular();
	float matShi = Utils::goldShininess();

	void window_size_callback(GLFWwindow* win, int newWidth, int newHeight) {
		aspect = (float)newWidth / (float)newHeight;
		glViewport(0, 0, newWidth, newHeight);
		pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
	}
}

using namespace geomModAdd;
void installLightsGeomModAdd(glm::mat4 vMatrix) {
	transformed = glm::vec3(vMatrix * glm::vec4(currentLightPos, 1.0));
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

void setupVertices_GeomModAdd(void) {
	std::vector<int> ind = myTorus.getIndices();
	std::vector<glm::vec3> vert = myTorus.getVertices();
	std::vector<glm::vec2> tex = myTorus.getTexCoords();
	std::vector<glm::vec3> norm = myTorus.getNormals();

	std::vector<float> pvalues;
	std::vector<float> tvalues;
	std::vector<float> nvalues;

	for (int i = 0; i < myTorus.getNumVertices(); i++) {
		pvalues.push_back(vert[i].x);
		pvalues.push_back(vert[i].y);
		pvalues.push_back(vert[i].z);
		tvalues.push_back(tex[i].s);
		tvalues.push_back(tex[i].t);
		nvalues.push_back(norm[i].x);
		nvalues.push_back(norm[i].y);
		nvalues.push_back(norm[i].z);
	}
	glGenVertexArrays(1, vao2);
	glBindVertexArray(vao2[0]);
	glGenBuffers(numVBOs, vbo2);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]);
	glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[1]);
	glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[2]);
	glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo2[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * 4, &ind[0], GL_STATIC_DRAW);
}

void init_lightGeomModAdd(GLFWwindow* window) {
	//renderingProgram2 = Utils::createShaderProgram("vertShaderGeomModInflate.glsl", "fragShaderGeomModInflate.glsl");
	renderingProgram2 = createShaderProgram(GEOM_MOD_INFLATE_ADD,ENABLE_GEOMETRY_ONLY);
	//renderingProgram2 = Utils::createShaderProgram("BlinnPhongShaders/vertShader.glsl", "BlinnPhongShaders/fragShader.glsl");
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 150.0f;
	torLocX = 0.0f; torLocY = 0.0f; torLocZ = 0.0f;

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	setupVertices_GeomModAdd();
}

void display_GeomModAdd(GLFWwindow* window, double currentTime) {
	glClear(GL_DEPTH_BUFFER_BIT);
	//glClearColor(1.0, 1.0, sin(currentTime), 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(renderingProgram2);

	mvLoc = glGetUniformLocation(renderingProgram2, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram2, "proj_matrix");
	nLoc = glGetUniformLocation(renderingProgram2, "norm_matrix");
	lLoc = glGetUniformLocation(renderingProgram2, "enableLighting");

	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX , -cameraY, -cameraZ));

	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(torLocX , torLocY , torLocZ ));
	mMat = glm::rotate(mMat, toRadians(35.0f * currentTime), glm::vec3(1.0f, 1.0f, 1.0f));

	currentLightPos = glm::vec3(initialLightLoc.x, initialLightLoc.y, initialLightLoc.z);
	amt = 25.0f;
	rMat = glm::rotate(glm::mat4(1.0f), toRadians(amt), glm::vec3(0.0f, 0.0f, 1.0f));
	currentLightPos = glm::vec3(rMat * glm::vec4(currentLightPos, 1.0f));

	installLightsGeomModAdd(vMat);

	mvMat = vMat * mMat;
	//用mv矩阵的逆转置矩阵来求正确的法向量
	invTrMat = glm::transpose(glm::inverse(mvMat));
	//invTrMat = mvMat;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[2]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glUniform1i(lLoc, 1);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo2[3]);
	glDrawElements(GL_TRIANGLES, numTorusIndices, GL_UNSIGNED_INT, 0);

	//glUniform1i(lLoc, 0);
	//glFrontFace(GL_CW);
	//glDrawElements(GL_TRIANGLES, numTorusIndices, GL_UNSIGNED_INT, 0);
}

int main29(void) {
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(800, 800, "Chapter 28 - program 1", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	glfwSetWindowSizeCallback(window, window_size_callback);

	init_lightGeomModAdd(window);

	while (!glfwWindowShouldClose(window)) {
		display_GeomModAdd(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}