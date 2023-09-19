#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "CommonLibrary.h"

using namespace std;
namespace Reflection {
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
	GLuint skyboxTexture;

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
	float matShi = 250.0f;

	GLuint refractTextureId;
	GLuint reflectTextureId;
	GLuint refractFrameBuffer;
	GLuint reflectFrameBuffer;

	void window_size_callback(GLFWwindow* win, int newWidth, int newHeight) {
		aspect = (float)newWidth / (float)newHeight;
		glViewport(0, 0, newWidth, newHeight);
		pMat = glm::perspective(1.0472f, aspect, 0.1f, 1500.0f);
	}
}

using namespace Reflection;

void installLights_Reflection(glm::mat4 vMatrix, GLuint renderingProgram) {
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

void setupVertices_Reflection(void) {
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

void createReflectRefractBuffers(GLFWwindow* window) {
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

void init_Reflection(GLFWwindow* window) {
	renderingProgramSURFACE = createShaderProgram(SURFACE_REFLECTION);
	renderingProgramFLOOR = createShaderProgram(FLOOR_REFLECTION);
	renderingProgramCubeMap = createShaderProgram(CUBE_REFLECTION);

	lightLoc = glm::vec3(-10.0f, 10.0f, -50.0f);

	setupVertices_Reflection();

	//用纹理坐标实现天空盒，则解除注释
	skyboxTexture = loadTexture("texture/alien.jpg");

	//skyboxTexture = loadCubeMap("cubeMap");
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	createReflectRefractBuffers(window);
}

void prepForSkyBoxRender() {
	glUseProgram(renderingProgramCubeMap);

	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), vec3(15.0, 15.0, 15.0))
		* glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	mvMat = vMat * mMat;

	vLoc = glGetUniformLocation(renderingProgramCubeMap, "v_matrix");
	projLoc = glGetUniformLocation(renderingProgramCubeMap, "p_matrix");

	glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

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

void prepForTopSurfaceRender() {
	glUseProgram(renderingProgramSURFACE);

	mvLoc = glGetUniformLocation(renderingProgramSURFACE, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgramSURFACE, "proj_matrix");
	nLoc = glGetUniformLocation(renderingProgramSURFACE, "norm_matrix");
	aboveLoc = glGetUniformLocation(renderingProgramSURFACE, "isAbove");

	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, surfacePlaneHeight, 150.0f)) * glm::scale(glm::mat4(1.0f), vec3(3.0, 1.0, 3.0));
	mvMat = vMat * mMat;
	invTrMat = glm::transpose(glm::inverse(mvMat));

	currentLightPos = glm::vec3(lightLoc.x, lightLoc.y, lightLoc.z);
	installLights_Reflection(vMat, renderingProgramSURFACE);

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));

	if (cameraHeight > surfacePlaneHeight)
		glUniform1i(aboveLoc, 1);
	else
		glUniform1i(aboveLoc, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[2]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[3]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);
}

void prepForFloorRender() {
	glUseProgram(renderingProgramFLOOR);

	mvLoc = glGetUniformLocation(renderingProgramFLOOR, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgramFLOOR, "proj_matrix");
	nLoc = glGetUniformLocation(renderingProgramFLOOR, "norm_matrix");
	aboveLoc = glGetUniformLocation(renderingProgramFLOOR, "isAbove");

	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, floorPlaneHeight, 150.0f)) * glm::scale(glm::mat4(1.0f), vec3(1.0, 1.0, 10.0));
	mvMat = vMat * mMat;
	invTrMat = glm::transpose(glm::inverse(mvMat));

	currentLightPos = glm::vec3(lightLoc.x, lightLoc.y, lightLoc.z);
	installLights_Reflection(vMat, renderingProgramFLOOR);

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));

	if (cameraHeight >= surfacePlaneHeight)
		glUniform1i(aboveLoc, 1);
	else
		glUniform1i(aboveLoc, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[2]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[3]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);
}

void display_Reflection(GLFWwindow* window, double currentTime) {
	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(toRadians(120.0f), aspect, 0.1f, 1000.0f);

	//	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -(surfacePlaneHeight - cameraHeight), 0.0f))
	//		* glm::rotate(glm::mat4(1.0f), toRadians(-cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f));

		// render reflection scene to reflection buffer ------------------------------------
		//将反射场景渲染给反射缓冲区（如果相机在水面之上）
	if (cameraHeight > surfacePlaneHeight) {
		//反射视角矩阵正好是主相机y轴位置和倾斜度取反
		vMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -(surfacePlaneHeight - cameraHeight), cameraZ))
			* glm::rotate(glm::mat4(1.0f), toRadians(-cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f));

		glBindFramebuffer(GL_FRAMEBUFFER, reflectFrameBuffer);
		glClear(GL_DEPTH_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT);
		prepForSkyBoxRender();
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
		prepForFloorRender();
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	else {
		prepForSkyBoxRender();
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
	prepForSkyBoxRender();
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);	// cube is CW, but we are viewing the inside
	glDisable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glEnable(GL_DEPTH_TEST);

	// draw water top (surface) =================================================================
	prepForTopSurfaceRender();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, reflectTextureId);//反射缓冲区贴图
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, refractTextureId);//折射缓冲区贴图

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	if (cameraHeight >= surfacePlaneHeight)
		glFrontFace(GL_CCW);
	else
		glFrontFace(GL_CW);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// draw water bottom (floor)  ==================================================================
	prepForFloorRender();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glFrontFace(GL_CCW);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

int main40(void) {
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(800, 800, "Chapter 40", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	glfwSetWindowSizeCallback(window, window_size_callback);

	init_Reflection(window);

	while (!glfwWindowShouldClose(window)) {
		display_Reflection(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}