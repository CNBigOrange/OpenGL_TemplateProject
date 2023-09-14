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
#include "ModelLoader.h"

using namespace std;
namespace Stripes3Dtex {
	float toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }

#define numVAOs 1
#define numVBOs 5

	float cameraX, cameraY, cameraZ;
	float torLocX, torLocY, torLocZ;
	GLuint renderingProgram2;
	GLuint vao2[numVAOs];
	GLuint vbo2[numVBOs];

	ImportedModel dolphinObj("models/dolphinHighPoly.obj");
	int numDolphinVertices;

	GLuint stripesTexture, stripesTexture2;
	//注注注：长宽高数据量不能太大，否则会出现“不是一个有效的win32应用程序”错误
	const int texHeight = 300;
	const int texWidth = 300;
	const int texDepth = 300;
	double tex3Dpattern[texHeight][texWidth][texDepth];
	double tex3Dpattern2[texHeight][texWidth][texDepth];

	// variable allocation for display
	GLuint mvLoc, projLoc;
	int width, height;
	float aspect;
	glm::mat4 pMat, vMat, mMat, mvMat;

	void window_size_callback(GLFWwindow* win, int newWidth, int newHeight) {
		aspect = (float)newWidth / (float)newHeight;
		glViewport(0, 0, newWidth, newHeight);
		pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
	}
}

using namespace Stripes3Dtex;

void fillDataArray(GLubyte data[]) {
	for (int i = 0; i < texHeight; i++) {
		for (int j = 0; j < texWidth; j++) {
			for (int k = 0; k < texDepth; k++) {
				if (tex3Dpattern[i][j][k] == 1.0) {
					//计算机至少要用1个字节来控制一个颜色，1byte = 8bit，表示0000 0000到1111 1111，换算为十进制为0~255
					// yellow color
					data[i * (texWidth * texHeight * 4) + j * (texHeight * 4) + k * 4 + 0] = (GLubyte)255; //red
					data[i * (texWidth * texHeight * 4) + j * (texHeight * 4) + k * 4 + 1] = (GLubyte)255; //green
					data[i * (texWidth * texHeight * 4) + j * (texHeight * 4) + k * 4 + 2] = (GLubyte)255; //blue
					data[i * (texWidth * texHeight * 4) + j * (texHeight * 4) + k * 4 + 3] = (GLubyte)0; //alpha
				}
				else {
					// blue color
					data[i * (texWidth * texHeight * 4) + j * (texHeight * 4) + k * 4 + 0] = (GLubyte)55; //red
					data[i * (texWidth * texHeight * 4) + j * (texHeight * 4) + k * 4 + 1] = (GLubyte)111; //green
					data[i * (texWidth * texHeight * 4) + j * (texHeight * 4) + k * 4 + 2] = (GLubyte)33; //blue
					data[i * (texWidth * texHeight * 4) + j * (texHeight * 4) + k * 4 + 3] = (GLubyte)0; //alpha
				}
			}
		}
	}
}

int build3DTexture() {
	GLuint textureID;
	GLubyte* data = new GLubyte[texHeight * texWidth * texDepth * 4];

	fillDataArray(data);

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_3D, textureID);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexStorage3D(GL_TEXTURE_3D, 1, GL_RGBA8, texWidth, texHeight, texDepth);
	glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, texWidth, texHeight, texDepth, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, data);

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

void generate3Dpattern() {
	int xStep = 0; int yStep = 0; int zStep = 0;int sumSteps = 0;
	for (int x = 0; x < texWidth/1.01; x++) {
		for (int y = 0; y < texHeight/1.01; y++) {
			for (int z = 0; z < texDepth/1.01; z++) {
				xStep = (x / 10) % 2;
				yStep = (y / 10) % 2;
				zStep = (z / 10) % 2;
				sumSteps = xStep + yStep + zStep;
				if ((sumSteps % 2) == 0)
					tex3Dpattern[x][y][z] = 0.0;
				else
					tex3Dpattern[x][y][z] = 1.0;
			}
		}
	}
}

void setupVertices_Stripes3Dtex(void) {

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

	//三角形顺时针缠绕的方式
	float CubeVertexs[] = {
	1.0,1.0,-1.0,  1.0,1.0,1.0, 1.0,-1.0,-1.0, 1.0,-1.0,-1.0, 1.0,1.0,1.0, 1.0,-1.0,1.0,
	-1.0,-1.0,1.0,1.0,1.0,1.0,  -1.0,1.0,1.0, -1.0,-1.0,1.0, 1.0,-1.0,1.0,1.0,1.0,1.0,
	-1.0,-1.0,-1.0,1.0,-1.0,1.0, -1.0,-1.0,1.0,  -1.0,-1.0,-1.0,  1.0,-1.0,-1.0, 1.0,-1.0,1.0 ,
	-1.0,-1.0,-1.0, -1.0,-1.0,1, -1.0,1.0,-1.0,  -1.0,1.0,-1.0,  -1.0,-1.0,1.0, -1.0,1.0,1.0,
	-1.0,1.0,-1.0,  1.0,1.0,1.0, 1.0,1.0,-1.0, -1.0,1.0,-1.0, -1.0,1.0,1.0, 1.0,1.0,1.0,
	-1.0,-1.0,-1.0, -1.0,1.0,-1.0, 1.0,-1.0,-1.0,  -1.0,1.0,-1.0,   1.0,1.0,-1.0,1.0,-1.0,-1.0,
	};

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(CubeVertexs), CubeVertexs, GL_STATIC_DRAW);
}

void init_lightStripes3Dtex(GLFWwindow* window) {
	//renderingProgram2 = Utils::createShaderProgram("vertShaderGeomModInflate.glsl", "fragShaderGeomModInflate.glsl");
	renderingProgram2 = createShaderProgram(STRIPES_3D_TEX);
	//renderingProgram2 = Utils::createShaderProgram("BlinnPhongShaders/vertShader.glsl", "BlinnPhongShaders/fragShader.glsl");
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 100.0f;
	torLocX = 0.0f; torLocY = 0.0f; torLocZ = 0.0f;

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	setupVertices_Stripes3Dtex();

	generate3Dpattern();
	stripesTexture = build3DTexture();
}

void display_Stripes3Dtex(GLFWwindow* window, double currentTime) {
	glClear(GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(renderingProgram2);

	mvLoc = glGetUniformLocation(renderingProgram2, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram2, "proj_matrix");

	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX , -cameraY, -cameraZ));
	vMat = glm::rotate(vMat, toRadians(300 ), glm::vec3(0.0, 1.0, 0.0));


	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(torLocX , torLocY , torLocZ ));
	mMat = glm::rotate(mMat, toRadians(350.0f * -currentTime), glm::vec3(0.0f, 1.0f, 0.0f));
	mMat = glm::scale(mMat, glm::vec3(1. , 1. , 1.));

	mvMat = vMat * mMat;
	
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, stripesTexture);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glFrontFace(GL_CCW);
	glDrawArrays(GL_TRIANGLES, 0, numDolphinVertices);

	//立方体

	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(torLocX+0.4, torLocY+0.4, torLocZ));
	mMat = glm::rotate(mMat, toRadians(3500.0f * -currentTime), glm::vec3(1.0f, 1.0f, 0.0f));
	mMat = glm::scale(mMat, glm::vec3(0.2, 0.2, 0.2));
	//vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
	mvMat = vMat * mMat;

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	//glDisable(GL_CULL_FACE);
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[3]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glFrontFace(GL_CW);
	glDrawArrays(GL_TRIANGLES, 0, 36);

}

int main34(void) {
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(800, 800, "Chapter 34", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	glfwSetWindowSizeCallback(window, window_size_callback);

	init_lightStripes3Dtex(window);

	while (!glfwWindowShouldClose(window)) {
		display_Stripes3Dtex(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}