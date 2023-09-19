#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "CommonLibrary.h"
#include <algorithm>
#include <array>
#include <random>

using namespace std;
namespace Water {
	float toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }

#define numVAOs 1
#define numVBOs 5

	float cameraHeight = 2.0f, cameraPitch = 35.0f, cameraZ = -700;
	float surfacePlaneHeight = 1.0f;
	float floorPlaneHeight = -0.0f;
	GLuint renderingProgramSURFACE, renderingProgramFLOOR, renderingProgramCubeMap;
	GLuint vao2[numVAOs];
	GLuint vbo2[numVBOs];

	// variable allocation for display
	GLuint vLoc, mvLoc, projLoc, nLoc;
	int width, height;
	float aspect;
	glm::mat4 pMat, vMat, mMat, mvMat, invTrMat;
	GLuint skyboxTexture, noiseTexture;

	glm::vec3 lightLoc = glm::vec3(0.0f, 4.0f, -20.0f);
	GLuint globalAmbLoc, ambLoc, diffLoc, specLoc, posLoc, mambLoc, mdiffLoc, mspecLoc, mshiLoc, aboveLoc;
	glm::vec3 currentLightPos, transformed;
	float lightPos[3];

	// white light
	float globalAmbient[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
	float lightAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float lightDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float lightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	// water material
	float matAmb[4] = { 0.5f, 0.6f, 0.8f, 1.0f };
	float matDif[4] = { 0.8f, 0.9f, 1.0f, 1.0f };
	float matSpe[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
	float matShi = 100.0f;

	GLuint refractTextureId;
	GLuint reflectTextureId;
	GLuint refractFrameBuffer;
	GLuint reflectFrameBuffer;

	GLuint skyTexture;
	const int noiseHeight = 256;
	const int noiseWidth = 256;
	const int noiseDepth = 256;
	double noise[noiseHeight][noiseWidth][noiseDepth];

	float depthLookup = 0.0f;
	GLuint dOffsetLoc;
	double prevTime;

	void window_size_callback(GLFWwindow* win, int newWidth, int newHeight) {
		aspect = (float)newWidth / (float)newHeight;
		glViewport(0, 0, newWidth, newHeight);
		pMat = glm::perspective(1.0472f, aspect, 0.1f, 1500.0f);
	}
}

using namespace Water;

// 3D Noise Texture section

double smoothNoiseWater(double zoom, double x1, double y1, double z1) {
	//get fractional part of x, y, and z
	double fractX = x1 - (int)x1;
	double fractY = y1 - (int)y1;
	double fractZ = z1 - (int)z1;

	//neighbor values that wrap
	double x2 = x1 - 1; if (x2 < 0) x2 = (round(noiseHeight / zoom)) - 1;
	double y2 = y1 - 1; if (y2 < 0) y2 = (round(noiseWidth / zoom)) - 1;
	double z2 = z1 - 1; if (z2 < 0) z2 = (round(noiseDepth / zoom)) - 1;

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

double turbulenceWater(double x, double y, double z, double maxZoom) {
	double sum = 0.0, zoom = maxZoom;
	//对角线正弦波
	sum = (sin((1.0 / 512.0) * (8 * PI) * (x + z - 4 * y)) + 1) * 8.0;

	while (zoom >= 0.9) {
		sum = sum + smoothNoiseWater(zoom, x / zoom, y / zoom, z / zoom) * zoom;
		zoom = zoom / 2.0;
	}

	sum = 128.0 * sum / maxZoom;
	return sum;
}

void fillDataArrayWater(GLubyte data[]) {
	double maxZoom = 32.0;
	for (int i = 0; i < noiseHeight; i++) {
		for (int j = 0; j < noiseWidth; j++) {
			for (int k = 0; k < noiseDepth; k++) {
				data[i * (noiseWidth * noiseHeight * 4) + j * (noiseHeight * 4) + k * 4 + 0] = (GLubyte)turbulenceWater(i, j, k, maxZoom);
				data[i * (noiseWidth * noiseHeight * 4) + j * (noiseHeight * 4) + k * 4 + 1] = (GLubyte)turbulenceWater(i, j, k, maxZoom);
				data[i * (noiseWidth * noiseHeight * 4) + j * (noiseHeight * 4) + k * 4 + 2] = (GLubyte)turbulenceWater(i, j, k, maxZoom);
				data[i * (noiseWidth * noiseHeight * 4) + j * (noiseHeight * 4) + k * 4 + 3] = (GLubyte)255;
			}
		}
	}
}

GLuint buildNoiseTextureWater() {
	GLuint textureID;
	GLubyte* data = new GLubyte[noiseHeight * noiseWidth * noiseDepth * 4];

	fillDataArrayWater(data);

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_3D, textureID);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexStorage3D(GL_TEXTURE_3D, 1, GL_RGBA8, noiseWidth, noiseHeight, noiseDepth);
	glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, noiseWidth, noiseHeight, noiseDepth, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, data);

	delete[] data;
	return textureID;
}

void generateNoiseWater() {
	for (int x = 0; x < noiseHeight; x++) {
		for (int y = 0; y < noiseWidth; y++) {
			for (int z = 0; z < noiseDepth; z++) {
				noise[x][y][z] = (double)rand() / (RAND_MAX + 1.0);
			}
		}
	}
}

// ========================================== END NOISE SECTION ==============================

void installLights_Water(glm::mat4 vMatrix, GLuint renderingProgram) {
	transformed = glm::vec3(vMatrix * glm::vec4(currentLightPos, 1.0));
	lightPos[0] = transformed.x;
	lightPos[1] = transformed.y;
	lightPos[2] = transformed.z;

	// get the locations of the light and material fields in the shader
	globalAmbLoc = glGetUniformLocation(renderingProgram, "globalAmbient");
	ambLoc = glGetUniformLocation(renderingProgram, "light.ambient");
	diffLoc = glGetUniformLocation(renderingProgram, "light.diffuse");
	specLoc = glGetUniformLocation(renderingProgram, "light.specular");
	posLoc = glGetUniformLocation(renderingProgram, "light.position");
	mambLoc = glGetUniformLocation(renderingProgram, "material.ambient");
	mdiffLoc = glGetUniformLocation(renderingProgram, "material.diffuse");
	mspecLoc = glGetUniformLocation(renderingProgram, "material.specular");
	mshiLoc = glGetUniformLocation(renderingProgram, "material.shininess");

	//  set the uniform light and material values in the shader
	glProgramUniform4fv(renderingProgram, globalAmbLoc, 1, globalAmbient);
	glProgramUniform4fv(renderingProgram, ambLoc, 1, lightAmbient);
	glProgramUniform4fv(renderingProgram, diffLoc, 1, lightDiffuse);
	glProgramUniform4fv(renderingProgram, specLoc, 1, lightSpecular);
	glProgramUniform3fv(renderingProgram, posLoc, 1, lightPos);
	glProgramUniform4fv(renderingProgram, mambLoc, 1, matAmb);
	glProgramUniform4fv(renderingProgram, mdiffLoc, 1, matDif);
	glProgramUniform4fv(renderingProgram, mspecLoc, 1, matSpe);
	glProgramUniform1f(renderingProgram, mshiLoc, matShi);
}

void setupVertices_Water(void) {
	float cubeVertexPositions[108] =
	{ -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
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

	//float cubeVertexPositions[108] =
	//{ -100.0f,  100.0f, -100.0f, -100.0f, -100.0f, -100.0f, 100.0f, -100.0f, -100.0f,
	//	100.0f, -100.0f, -100.0f, 100.0f,  100.0f, -100.0f, -100.0f,  100.0f, -100.0f,
	//	100.0f, -100.0f, -100.0f, 100.0f, -100.0f,  100.0f, 100.0f,  100.0f, -100.0f,
	//	100.0f, -100.0f,  100.0f, 100.0f,  100.0f,  100.0f, 100.0f,  100.0f, -100.0f,
	//	100.0f, -100.0f,  100.0f, -100.0f, -100.0f,  100.0f, 100.0f,  100.0f,  100.0f,
	//	-100.0f, -100.0f,  100.0f, -100.0f,  100.0f,  100.0f, 100.0f,  100.0f,  100.0f,
	//	-100.0f, -100.0f,  100.0f, -100.0f, -100.0f, -100.0f, -100.0f,  100.0f,  100.0f,
	//	-100.0f, -100.0f, -100.0f, -100.0f,  100.0f, -100.0f, -100.0f,  100.0f,  100.0f,
	//	-100.0f, -100.0f,  100.0f,  100.0f, -100.0f,  100.0f,  100.0f, -100.0f, -100.0f,
	//	100.0f, -100.0f, -100.0f, -100.0f, -100.0f, -100.0f, -100.0f, -100.0f,  100.0f,
	//	-100.0f,  100.0f, -100.0f, 100.0f,  100.0f, -100.0f, 100.0f,  100.0f,  100.0f,
	//	100.0f,  100.0f,  100.0f, -100.0f,  100.0f,  100.0f, -100.0f,  100.0f, -100.0f
	//};

	float cubeTextureCoord[72] =
	{ 1.00f, 0.66f, 1.00f, 0.33f, 0.75f, 0.33f,	// back face lower right
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

	float PLANE_POSITIONS[18] = {
		-120.0f, 0.0f, -120.0f,  -120.0f, 0.0f, 120.0f,  120.0f, 0.0f, -120.0f,
		120.0f, 0.0f, -120.0f,  -120.0f, 0.0f, 120.0f,  120.0f, 0.0f, 120.0f
	};
	float PLANE_TEXCOORDS[12] = {
		0.0f, 0.0f,  0.0f, 1.0f,  1.0f, 0.0f,
		1.0f, 0.0f,  0.0f, 1.0f,  1.0f, 1.0f
	};
	float PLANE_NORMALS[18] = {
		0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, vao2);
	glBindVertexArray(vao2[0]);
	glGenBuffers(numVBOs, vbo2);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertexPositions), cubeVertexPositions, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(PLANE_POSITIONS), PLANE_POSITIONS, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(PLANE_TEXCOORDS), PLANE_TEXCOORDS, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(PLANE_NORMALS), PLANE_NORMALS, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[4]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTextureCoord) * 4, cubeTextureCoord, GL_STATIC_DRAW);
}

void createReflectRefractBuffersWater(GLFWwindow* window) {
	GLuint bufferId[1];
	glGenBuffers(1, bufferId);
	glfwGetFramebufferSize(window, &width, &height);

	// Initialize Reflect Framebuffer初始化反射帧缓冲区
	glGenFramebuffers(1, bufferId);
	reflectFrameBuffer = bufferId[0];
	glBindFramebuffer(GL_FRAMEBUFFER, reflectFrameBuffer);
	glGenTextures(1, bufferId);
	reflectTextureId = bufferId[0];//颜色缓冲区
	glBindTexture(GL_TEXTURE_2D, reflectTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectTextureId, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glGenTextures(1, bufferId);//深度缓冲区
	glBindTexture(GL_TEXTURE_2D, bufferId[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferId[0], 0);

	// Initialize Refract Framebuffer初始化折射帧缓冲区
	glGenFramebuffers(1, bufferId);
	refractFrameBuffer = bufferId[0];
	glBindFramebuffer(GL_FRAMEBUFFER, refractFrameBuffer);
	glGenTextures(1, bufferId);
	refractTextureId = bufferId[0];
	glBindTexture(GL_TEXTURE_2D, refractTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, refractTextureId, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glGenTextures(1, bufferId);
	glBindTexture(GL_TEXTURE_2D, bufferId[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferId[0], 0);
}

void init_Water(GLFWwindow* window) {
	renderingProgramSURFACE = createShaderProgram(WATER_SURFACE);
	renderingProgramFLOOR = createShaderProgram(WATER_FLOOR);
	renderingProgramCubeMap = createShaderProgram(WATER_CUBE);

	lightLoc = glm::vec3(-10.0f, 10.0f, -50.0f);

	setupVertices_Water();

	//用纹理坐标实现天空盒，则解除注释
	//skyboxTexture = loadTexture("texture/alien.jpg");
	skyboxTexture = loadTexture("texture/3a24142dce7b271799b6501fabc4ee19_r.jpg"); 

	//skyboxTexture = loadCubeMap("cubeMap");
	//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	createReflectRefractBuffersWater(window);

	generateNoiseWater();
	noiseTexture = buildNoiseTextureWater();
	prevTime = glfwGetTime();
}

void prepForSkyBoxRenderWater() {
	glUseProgram(renderingProgramCubeMap);

	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), vec3(15.0, 15.0, 15.0))
		* glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	mvMat = vMat * mMat;

	vLoc = glGetUniformLocation(renderingProgramCubeMap, "v_matrix");
	projLoc = glGetUniformLocation(renderingProgramCubeMap, "p_matrix");
	aboveLoc = glGetUniformLocation(renderingProgramCubeMap, "isAbove");

	glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	if (cameraHeight >= surfacePlaneHeight)
		glUniform1i(aboveLoc, 1);
	else
		glUniform1i(aboveLoc, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	
	//用纹理坐标实现天空盒，则解除注释
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[4]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, skyboxTexture);
	

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
}

void prepForTopSurfaceRenderWater() {
	glUseProgram(renderingProgramSURFACE);

	mvLoc = glGetUniformLocation(renderingProgramSURFACE, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgramSURFACE, "proj_matrix");
	nLoc = glGetUniformLocation(renderingProgramSURFACE, "norm_matrix");
	aboveLoc = glGetUniformLocation(renderingProgramSURFACE, "isAbove");

	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, surfacePlaneHeight, 150.0f)) * glm::scale(glm::mat4(1.0f), vec3(3.0, 1.0, 10.0));
	mvMat = vMat * mMat;
	invTrMat = glm::transpose(glm::inverse(mvMat));

	currentLightPos = glm::vec3(lightLoc.x, lightLoc.y, lightLoc.z);
	installLights_Water(vMat, renderingProgramSURFACE);

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));

	if (cameraHeight > surfacePlaneHeight)
		glUniform1i(aboveLoc, 1);
	else
		glUniform1i(aboveLoc, 0);

	dOffsetLoc = glGetUniformLocation(renderingProgramSURFACE, "depthOffset");
	glUniform1f(dOffsetLoc, depthLookup);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[2]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[3]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, reflectTextureId);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, refractTextureId);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_3D, noiseTexture);
}

void prepForFloorRenderWater() {
	glUseProgram(renderingProgramFLOOR);

	mvLoc = glGetUniformLocation(renderingProgramFLOOR, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgramFLOOR, "proj_matrix");
	nLoc = glGetUniformLocation(renderingProgramFLOOR, "norm_matrix");
	aboveLoc = glGetUniformLocation(renderingProgramFLOOR, "isAbove");

	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, floorPlaneHeight, 150.0f)) * glm::scale(glm::mat4(1.0f), vec3(1.0, 1.0, 10.0));
	mvMat = vMat * mMat;
	invTrMat = glm::transpose(glm::inverse(mvMat));

	currentLightPos = glm::vec3(lightLoc.x, lightLoc.y, lightLoc.z);
	installLights_Water(vMat, renderingProgramFLOOR);

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));

	if (cameraHeight >= surfacePlaneHeight)
		glUniform1i(aboveLoc, 1);
	else
		glUniform1i(aboveLoc, 0);

	dOffsetLoc = glGetUniformLocation(renderingProgramFLOOR, "depthOffset");
	glUniform1f(dOffsetLoc, depthLookup);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[2]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[3]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, noiseTexture);
}

void display_Water(GLFWwindow* window, double currentTime) {
	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(toRadians(120.0f), aspect, 0.1f, 1000.0f);

	depthLookup += (currentTime - prevTime) * .05f;
	prevTime = currentTime;

	// render reflection scene to reflection buffer ------------------------------------
	//将反射场景渲染给反射缓冲区（如果相机在水面之上）
	if (cameraHeight > surfacePlaneHeight) {
		//反射视角矩阵正好是主相机y轴位置和倾斜度取反
		vMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -(surfacePlaneHeight - cameraHeight), cameraZ))
			* glm::rotate(glm::mat4(1.0f), toRadians(-cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f));

		glBindFramebuffer(GL_FRAMEBUFFER, reflectFrameBuffer);
		glClear(GL_DEPTH_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT);
		prepForSkyBoxRenderWater();
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);	// cube is CW, but we are viewing the inside
		glDisable(GL_DEPTH_TEST);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glEnable(GL_DEPTH_TEST);
	}

	// render refraction scene to refraction buffer ----------------------------------------
	//将折射场景渲染给折射缓冲区
	//折射视图矩阵和主相机相同
	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -(-(surfacePlaneHeight - cameraHeight)), cameraZ))
		* glm::rotate(glm::mat4(1.0f), toRadians(cameraPitch ), glm::vec3(1.0f, 0.0f, 0.0f));

	glBindFramebuffer(GL_FRAMEBUFFER, refractFrameBuffer);
	glClear(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);

	//渲染合适的物体到折射缓冲区
	if (cameraHeight >= surfacePlaneHeight) {
		prepForFloorRenderWater();
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	else {
		prepForSkyBoxRenderWater();
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);	// cube is CW, but we are viewing the inside
		glDisable(GL_DEPTH_TEST);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glEnable(GL_DEPTH_TEST);
	}
	// now render the entire scene ########################################################
	//切换回标准着色器，准备组装整个场景
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);

	// draw cube map  ======================================================================
	prepForSkyBoxRenderWater();
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);	// cube is CW, but we are viewing the inside
	glDisable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glEnable(GL_DEPTH_TEST);

	// draw water top (surface) =================================================================
	prepForTopSurfaceRenderWater();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	if (cameraHeight >= surfacePlaneHeight)
		glFrontFace(GL_CCW);
	else
		glFrontFace(GL_CW);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// draw water bottom (floor)  ==================================================================
	prepForFloorRenderWater();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glFrontFace(GL_CCW);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

int main(void) {
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(800, 800, "Chapter 40", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	glfwSetWindowSizeCallback(window, window_size_callback);

	init_Water(window);

	while (!glfwWindowShouldClose(window)) {
		display_Water(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}