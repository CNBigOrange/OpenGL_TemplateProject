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
namespace Dissolve {
	float toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }

#define numVAOs 1
#define numVBOs 3

	float cameraX, cameraY, cameraZ;
	float torLocX, torLocY, torLocZ;
	GLuint renderingProgram2;
	GLuint vao2[numVAOs];
	GLuint vbo2[numVBOs];

	Sphere mySphere(48);
	int numSphereVertices;

	GLuint noiseTexture, earthTexture;
	const int noiseHeight = 256;
	const int noiseWidth = 256;
	const int noiseDepth = 256;
	double noise[noiseHeight][noiseWidth][noiseDepth];

	// variable allocation for display
	GLuint mvLoc, projLoc, tLoc;
	int width, height;
	float aspect;
	glm::mat4 pMat, vMat, mMat, mvMat;

	float threshold = -0.2f;

	void window_size_callback(GLFWwindow* win, int newWidth, int newHeight) {
		aspect = (float)newWidth / (float)newHeight;
		glViewport(0, 0, newWidth, newHeight);
		pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
	}
}

using namespace Dissolve;
//ƽ������
double smoothNoiseDissolve(double zoom, double x1, double y1, double z1) {
	//get fractional part of x, y, and z
	double fractX = x1 - (int)x1;
	double fractY = y1 - (int)y1;
	double fractZ = z1 - (int)z1;

	//neighbor values
	double x2 = x1 - 1; if (x2 < 0) x2 = round(noiseWidth / zoom) - 1.0;
	double y2 = y1 - 1; if (y2 < 0) y2 = round(noiseHeight / zoom) - 1.0;
	double z2 = z1 - 1; if (z2 < 0) z2 = round(noiseDepth / zoom) - 1.0;

	//smooth the noise by interpolating
	double value = 0.0;
	value += fractX * fractY * fractZ * noise[(int)x1][(int)y1][(int)z1];
	value += (1.0 - fractX) * fractY * fractZ * noise[(int)x2][(int)y1][(int)z1];
	value += fractX * (1.0 - fractY) * fractZ * noise[(int)x1][(int)y2][(int)z1];
	value += (1.0 - fractX) * (1.0 - fractY) * fractZ * noise[(int)x2][(int)y2][(int)z1];

	value += fractX * fractY * (1.0 - fractZ) * noise[(int)x1][(int)y1][(int)z2];
	value += (1.0 - fractX) * fractY * (1.0 - fractZ) * noise[(int)x2][(int)y1][(int)z2];
	value += fractX * (1.0 - fractY) * (1.0 - fractZ) * noise[(int)x1][(int)y2][(int)z2];
	value += (1.0 - fractX) * (1.0 - fractY) * (1.0 - fractZ) * noise[(int)x2][(int)y2][(int)z2];

	return value;
}

//
double turbulenceDissolve(double x, double y, double z, double maxZoom) {
	double sum = 0.0, zoom = maxZoom, cloudQuant;
	while (zoom >= 0.9) {
		sum = sum + smoothNoiseDissolve(zoom, x / zoom, y / zoom, z / zoom) * zoom;
		zoom = zoom / 2.0;
	}
	sum = 128.0 * sum / maxZoom;
	cloudQuant = 130.0;  // tunable quantity of clouds
	sum = 256.0 * logistic(sum - cloudQuant);
	return sum;
}

void fillDataArrayDissolve(GLubyte data[]) {
	for (int i = 0; i < noiseHeight; i++) {
		for (int j = 0; j < noiseWidth; j++) {
			for (int k = 0; k < noiseDepth; k++) {
				data[i * (noiseWidth * noiseHeight * 4) + j * (noiseHeight * 4) + k * 4 + 0] = (GLubyte)(noise[i][j][k] * 255);
				data[i * (noiseWidth * noiseHeight * 4) + j * (noiseHeight * 4) + k * 4 + 1] = (GLubyte)(noise[i][j][k] * 255);
				data[i * (noiseWidth * noiseHeight * 4) + j * (noiseHeight * 4) + k * 4 + 2] = (GLubyte)(noise[i][j][k] * 255);
				data[i * (noiseWidth * noiseHeight * 4) + j * (noiseHeight * 4) + k * 4 + 3] = (GLubyte)255;
			}
		}
	}
}

int build3DTextureDissolve() {
	GLuint textureID;
	GLubyte* data = new GLubyte[noiseWidth * noiseHeight * noiseDepth * 4];

	fillDataArrayDissolve(data);

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_3D, textureID);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexStorage3D(GL_TEXTURE_3D, 1, GL_RGBA8, noiseWidth, noiseHeight, noiseDepth);
	glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, noiseWidth, noiseHeight, noiseDepth, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, data);

	delete[] data;
	return textureID;
}

void generate3DpatternDissolve() {
	for (int x = 0; x < noiseWidth; x++) {
		for (int y = 0; y < noiseHeight; y++) {
			for (int z = 0; z < noiseDepth; z++) {
				noise[x][y][z] = (double)rand() / (RAND_MAX + 1.0);
			}
		}
	}
}

void setupVertices_Dissolve(void) {
	std::vector<int> ind = mySphere.getIndices();
	std::vector<glm::vec3> vert = mySphere.getVertices();
	std::vector<glm::vec2> tex = mySphere.getTexCoords();
	std::vector<glm::vec3> norm = mySphere.getNormals();
	numSphereVertices = mySphere.getNumIndices();

	std::vector<float> pvalues;
	std::vector<float> tvalues;
	std::vector<float> nvalues;

	for (int i = 0; i < mySphere.getNumIndices(); i++) {
		pvalues.push_back((vert[ind[i]]).x);
		pvalues.push_back((vert[ind[i]]).y);
		pvalues.push_back((vert[ind[i]]).z);
		tvalues.push_back((tex[ind[i]]).x);
		tvalues.push_back((tex[ind[i]]).y);
		nvalues.push_back((norm[ind[i]]).x);
		nvalues.push_back((norm[ind[i]]).y);
		nvalues.push_back((norm[ind[i]]).z);
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
}

void init_lightDissolve(GLFWwindow* window) {
	//renderingProgram2 = Utils::createShaderProgram("vertShaderGeomModInflate.glsl", "fragShaderGeomModInflate.glsl");
	renderingProgram2 = createShaderProgram(DISSOLVE);
	//renderingProgram2 = Utils::createShaderProgram("BlinnPhongShaders/vertShader.glsl", "BlinnPhongShaders/fragShader.glsl");
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 150.0f;
	torLocX = 0.0f; torLocY = 0.0f; torLocZ = 0.0f;

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	setupVertices_Dissolve();

	generate3DpatternDissolve();
	noiseTexture = build3DTextureDissolve();
	earthTexture = loadTexture("texture/moon.jpg");
}

void display_Dissolve(GLFWwindow* window, double currentTime) {
	glClear(GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(renderingProgram2);

	mvLoc = glGetUniformLocation(renderingProgram2, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram2, "proj_matrix");
	tLoc = glGetUniformLocation(renderingProgram2, "t");

	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX , -cameraY, -cameraZ));
	vMat = glm::rotate(vMat, toRadians(300 ), glm::vec3(0.0, 1.0, 0.0));

	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(torLocX , torLocY , torLocZ ));
	mMat = glm::rotate(mMat, toRadians(350 * currentTime), glm::vec3(0.0f, 1.0f, 0.0f));
	mMat = glm::scale(mMat, glm::vec3(1. , 1. , 1.));

	mvMat = vMat * mMat;

	threshold = (float)currentTime * 0.2f;

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniform1f(tLoc, threshold);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, noiseTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, earthTexture);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glFrontFace(GL_CCW);
	glDrawArrays(GL_TRIANGLES, 0, numSphereVertices);

}

int main38(void) {
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(800, 800, "Chapter 37", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	glfwSetWindowSizeCallback(window, window_size_callback);

	init_lightDissolve(window);

	while (!glfwWindowShouldClose(window)) {
		display_Dissolve(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}