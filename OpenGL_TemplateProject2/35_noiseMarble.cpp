#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "ShapeClass/Sphere.h"
#include "CommonLibrary.h"
#include "ModelLoader.h"

using namespace std;
namespace Marble {
	float toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }

#define numVAOs 1
#define numVBOs 5

	float cameraX, cameraY, cameraZ;
	float torLocX, torLocY, torLocZ;
	GLuint renderingProgram2;
	GLuint vao2[numVAOs];
	GLuint vbo2[numVBOs];

	glm::vec3 lightLoc = glm::vec3(0.0f, 0.0f, 10.6f);

	ImportedModel dolphinObj("models/dolphinHighPoly.obj");
	int numDolphinVertices;

	GLuint noiseTexture;
	const int noisePrecision = 256;  // set depending on GPU memory
	const int noiseWidth = noisePrecision;
	const int noiseHeight = noisePrecision;
	const int noiseDepth = noisePrecision;
	double noise[noiseWidth][noiseHeight][noiseDepth];

	// variable allocation for display
	GLuint mvLoc, projLoc, nLoc;
	GLuint globalAmbLoc, ambLoc, diffLoc, specLoc, posLoc, mambLoc, mdiffLoc, mspecLoc, mshiLoc;
	int width, height;
	float aspect;
	glm::mat4 pMat, vMat, mMat, mvMat, invTrMat;
	glm::vec3 currentLightPos;
	float lightPos[3];

	// white light
	float globalAmbient[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	float lightAmbient[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float lightDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float lightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	float thisAmb[4], thisDif[4], thisSpe[4], matAmb[4], matDif[4], matSpe[4];
	float thisShi;

	// white material
	float matShi = 75.0f;

	void window_size_callback(GLFWwindow* win, int newWidth, int newHeight) {
		aspect = (float)newWidth / (float)newHeight;
		glViewport(0, 0, newWidth, newHeight);
		pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
	}
}

using namespace Marble;
//平滑噪声
double smoothNoise(double zoom, double x1, double y1, double z1) {
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
double turbulence(double x, double y, double z, double maxZoom) {
	double sum = 0.0, zoom = maxZoom;
	while (zoom >= 0.9) {
		sum = sum + smoothNoise(zoom, x / zoom, y / zoom, z / zoom) * zoom;
		zoom = zoom / 2.0;//对每个2的幂缩放因子
	}
	sum = 128.0 * sum / maxZoom;
	return sum;
}

void fillDataArrayMarble(GLubyte data[]) {
	double veinFrequency = 2.0;//用于调整条纹数量
	double turbPower = 2.0;//扰动量
	double maxZoom = 32.0;//湍流的缩放系数
	for (int i = 0; i < noiseWidth; i++) {
		for (int j = 0; j < noiseHeight; j++) {
			for (int k = 0; k < noiseDepth; k++) {
				double xyzValue = (float)i / noiseWidth + (float)j / noiseHeight + (float)k / noiseDepth
					+ turbPower * turbulence(i, j, k, maxZoom) / 256.0;

				double sineValue = logistic(abs(sin(xyzValue * 3.14159 * veinFrequency)));//logistic()函数使噪声图中的值更倾向于靠近0.0或255.0
				sineValue = std::max(-1.0, std::min(sineValue * 1.25 - 0.20, 1.0));

				float redPortion = 255.0f * (float)std::min(sineValue - 0.05, 1.0);
				float greenPortion = 255.0f * (float)std::min(sineValue + 0.15, 1.0);
				//float greenPortion = 255.0f * (float)sineValue;
				float bluePortion = 255.0f * (float)sineValue;

				data[i * (noiseWidth * noiseHeight * 4) + j * (noiseHeight * 4) + k * 4 + 0] = (GLubyte)redPortion;
				data[i * (noiseWidth * noiseHeight * 4) + j * (noiseHeight * 4) + k * 4 + 1] = (GLubyte)greenPortion;
				data[i * (noiseWidth * noiseHeight * 4) + j * (noiseHeight * 4) + k * 4 + 2] = (GLubyte)bluePortion;
				data[i * (noiseWidth * noiseHeight * 4) + j * (noiseHeight * 4) + k * 4 + 3] = (GLubyte)255;
			}
		}
	}
}

int build3DTextureMarble() {
	GLuint textureID;
	GLubyte* data = new GLubyte[noiseWidth * noiseHeight * noiseDepth * 4];

	fillDataArrayMarble(data);

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_3D, textureID);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexStorage3D(GL_TEXTURE_3D, 1, GL_RGBA8, noiseWidth, noiseHeight, noiseDepth);
	glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, noiseWidth, noiseHeight, noiseDepth, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, data);

	delete[] data;
	return textureID;
}

//void generate3Dpattern() {
//	for (int x = 0; x < texHeight; x++) {
//		for (int y = 0; y < texWidth; y++) {
//			for (int z = 0; z < texDepth; z++) {
//				if ((y / 10) % 2 == 0)
//					tex3Dpattern[x][y][z] = 0.0;
//				else
//					tex3Dpattern[x][y][z] = 1.0;
//			}
//		}
//	}
//}


//  replace above function with the one below
//	to change the stripes to a checkerboard.

void generate3DpatternMarble() {
	for (int x = 0; x < noiseWidth; x++) {
		for (int y = 0; y < noiseHeight; y++) {
			for (int z = 0; z < noiseDepth; z++) {
				noise[x][y][z] = (double)rand() / (RAND_MAX + 1.0);
			}
		}
	}
}

void installLightsMarble(glm::mat4 vMatrix) {
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
	mshiLoc = glGetUniformLocation(renderingProgram2, "material.shininess");

	//  set the uniform light and material values in the shader
	glProgramUniform4fv(renderingProgram2, globalAmbLoc, 1, globalAmbient);
	glProgramUniform4fv(renderingProgram2, ambLoc, 1, lightAmbient);
	glProgramUniform4fv(renderingProgram2, diffLoc, 1, lightDiffuse);
	glProgramUniform4fv(renderingProgram2, specLoc, 1, lightSpecular);
	glProgramUniform3fv(renderingProgram2, posLoc, 1, lightPos);
	glProgramUniform1f(renderingProgram2, mshiLoc, matShi);
}

void setupVertices_Marble(void) {

	numDolphinVertices = dolphinObj.getNumVertices();
	std::vector<glm::vec3> vert = dolphinObj.getVertices();
	std::vector<glm::vec2> tex = dolphinObj.getTextureCoords();
	std::vector<glm::vec3> norm = dolphinObj.getNormals();

	std::vector<float> pvalues;
	std::vector<float> tvalues;
	std::vector<float> nvalues;

	for (int i = 0; i < numDolphinVertices; i++) {
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
	glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[1]);
	glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[2]);
	glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);

}

void init_lightMarble(GLFWwindow* window) {
	//renderingProgram2 = Utils::createShaderProgram("vertShaderGeomModInflate.glsl", "fragShaderGeomModInflate.glsl");
	renderingProgram2 = createShaderProgram(MARBLE);
	//renderingProgram2 = Utils::createShaderProgram("BlinnPhongShaders/vertShader.glsl", "BlinnPhongShaders/fragShader.glsl");
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 100.0f;
	torLocX = 0.0f; torLocY = 0.0f; torLocZ = 0.0f;

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	setupVertices_Marble();

	generate3DpatternMarble();
	noiseTexture = build3DTextureMarble();
}

void display_Marble(GLFWwindow* window, double currentTime) {
	glClear(GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(renderingProgram2);

	mvLoc = glGetUniformLocation(renderingProgram2, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram2, "proj_matrix");
	nLoc = glGetUniformLocation(renderingProgram2, "norm_matrix");

	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX , -cameraY, -cameraZ));
	vMat = glm::rotate(vMat, toRadians(300 ), glm::vec3(0.0, 1.0, 0.0));

	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(torLocX , torLocY , torLocZ ));
	mMat = glm::rotate(mMat, toRadians(350.0f * -currentTime), glm::vec3(0.0f, 1.0f, 0.0f));
	mMat = glm::scale(mMat, glm::vec3(1. , 1. , 1.));

	mvMat = vMat * mMat;
	invTrMat = glm::transpose(glm::inverse(mvMat));

	currentLightPos = glm::vec3(lightLoc.x, lightLoc.y, lightLoc.z);
	installLightsMarble(vMat);

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[2]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, noiseTexture);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glFrontFace(GL_CCW);
	glDrawArrays(GL_TRIANGLES, 0, numDolphinVertices);

}

int main35(void) {
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(800, 800, "Chapter 35", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	glfwSetWindowSizeCallback(window, window_size_callback);

	init_lightMarble(window);

	while (!glfwWindowShouldClose(window)) {
		display_Marble(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}