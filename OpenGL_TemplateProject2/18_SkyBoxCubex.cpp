#include <string>
#include <iostream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "ModelLoader.h"
#include "CommonLibrary.h"
using namespace std;

namespace skyCube {
	float toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }

#define numVAOs 1
#define numVBOs 5

	float cameraX, cameraY, cameraZ;
	float torLocX, torLocY, torLocZ;
	GLuint renderingProgram2;
	GLuint vao2[numVAOs];
	GLuint vbo2[numVBOs];
	GLuint brickTexture, skyboxTexture;
	float rotAmt = 0.0f;

	// variable allocation for display
	GLuint mvLoc, projLoc;
	int width, height;
	float aspect;
	glm::mat4 pMat, vMat, mMat, mvMat;

	ImportedModel myModel("models/shuttle.obj");

	void window_size_callback2(GLFWwindow* win, int newWidth, int newHeight) {
		aspect = (float)newWidth / (float)newHeight;
		glViewport(0, 0, newWidth, newHeight);
		pMat = glm::perspective(1.0472f, aspect, 0.1f, 2000.0f);
	}
}
using namespace skyCube;

void setupVerticesSkyCube(void) {
	float cubeVertexPositions[108] =
	{	-1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
		1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
		1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f
	};
	float cubeTextureCoord[72] =
	{	1.00f, 0.66f, 1.00f, 0.33f, 0.75f, 0.33f,	// back face lower right
		0.75f, 0.33f, 0.75f, 0.66f, 1.00f, 0.66f,	// back face upper left
		0.75f, 0.33f, 0.50f, 0.33f, 0.75f, 0.66f,	// right face lower right
		0.50f, 0.33f, 0.50f, 0.66f, 0.75f, 0.66f,	// right face upper left
		0.50f, 0.33f, 0.25f, 0.33f, 0.50f, 0.66f,	// front face lower right
		0.25f, 0.33f, 0.25f, 0.66f, 0.50f, 0.66f,	// front face upper left
		0.00f, 0.33f, 0.00f, 0.66f, 0.25f, 0.66f,	// left face upper left
		0.25f, 0.33f, 0.50f, 0.33f, 0.50f, 0.00f,	// bottom face upper right
		0.50f, 0.00f, 0.25f, 0.00f, 0.25f, 0.33f,	// bottom face lower left
		0.25f, 1.00f, 0.50f, 1.00f, 0.50f, 0.66f,	// top face upper right
		0.50f, 0.66f, 0.25f, 0.66f, 0.25f, 1.00f		// top face lower left
	};

	//float cubeVertexPositions[18] =
	//{ -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
	//	1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
	//};
	//float cubeTextureCoord[12] =
	//{ 1.00f, 0.0f, 1.00f, 1.0f, 0.0f, 0.0f,	// back face lower right
	//	0.0f, 1.0f, 1.0f, 1.0f, 0.00f, 0.0f,	// back face upper left
	//};

	std::vector<glm::vec3> vert = myModel.getVertices();
	std::vector<glm::vec2> tex = myModel.getTextureCoords();
	std::vector<glm::vec3> norm = myModel.getNormals();
	int numObjVertices = myModel.getNumVertices();

	std::vector<float> pvalues;//顶点位置
	std::vector<float> tvalues;//纹理坐标
	std::vector<float> nvalues;//法向量

	//由索引去找对应顶点坐标、纹理坐标、法向量
	for (int i = 0; i < numObjVertices; i++) {
		pvalues.push_back((vert[i]).x);
		pvalues.push_back((vert[i]).y);
		pvalues.push_back((vert[i]).z);

		tvalues.push_back((tex[i]).s);
		tvalues.push_back((tex[i]).t);

		nvalues.push_back((norm[i]).x);
		nvalues.push_back((norm[i]).y);
		nvalues.push_back((norm[i]).z);
	}
	glGenVertexArrays(1, vao2);
	glBindVertexArray(vao2[0]);
	glGenBuffers(numVBOs, vbo2);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertexPositions) * 4 , cubeVertexPositions, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTextureCoord) * 4 , cubeTextureCoord, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[2]);
	glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[3]);
	glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);

}

void initSkyCube(GLFWwindow* window) {
	renderingProgram2 = createShaderProgram(SKY_CUBE);

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	setupVerticesSkyCube();

	brickTexture = loadTexture("texture/spstob_1.jpg");
	skyboxTexture = loadTexture("texture/3a24142dce7b271799b6501fabc4ee19_r.jpg");

	torLocX = 0.0f; torLocY = 0.0f; torLocZ = 0.0f;
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 100.0f;

	glBindTexture(GL_TEXTURE_2D, brickTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
}

void displaySkyCube(GLFWwindow* window, double currentTime) {
	glClear(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);

	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));

	// draw cube map

	glUseProgram(renderingProgram2);

	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(cameraX, cameraY, -500.0 + cameraZ));
	mMat = glm::scale(mMat, glm::vec3(5.0, 5.0, 5.0));
	mMat = glm::rotate(mMat, toRadians(150), glm::vec3(0.0, 1.0, 0.0));
	mvMat = vMat * mMat;

	mvLoc = glGetUniformLocation(renderingProgram2, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram2, "proj_matrix");

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, skyboxTexture);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);	// cube is CW, but we are viewing the inside
	glDisable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glEnable(GL_DEPTH_TEST);

	// draw scene (in this case it is just a model

	glUseProgram(renderingProgram2);

	mvLoc = glGetUniformLocation(renderingProgram2, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram2, "proj_matrix");

	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(torLocX, torLocY, torLocZ));
	mMat = glm::rotate(mMat, toRadians(450.0f * currentTime), glm::vec3(0.0f, 1.0f, 0.0f));
	mMat = glm::scale(mMat, glm::vec3(1.0, 1.0, 1.0));
	mvMat = vMat * mMat;

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[2]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[3]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, brickTexture);

	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, myModel.getNumVertices());
}

int main18(void) {
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(800, 800, "Chapter 9 - program 1", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	glfwSetWindowSizeCallback(window, window_size_callback2);

	initSkyCube(window);

	while (!glfwWindowShouldClose(window)) {
		displaySkyCube(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}