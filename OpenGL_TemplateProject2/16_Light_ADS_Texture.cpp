#include <string>
#include <iostream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "ModelLoader.h"
#include "CommonLibrary.h"
#include <map>

using namespace std;
namespace light_ADS_texture {
	float toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }

#define numVAOs 1
#define numVBOs 3

	float cameraX, cameraY, cameraZ;
	float torLocX, torLocY, torLocZ;
	GLuint renderingProgram2;
	GLuint vao2[numVAOs];
	GLuint vbo2[numVBOs];
	GLuint brickTexture;


	glm::vec3 initialLightLoc = glm::vec3(5.0f, 2.0f, 2.0f);
	float amt = 1.0f;

	// variable allocation for display
	GLuint mvLoc, projLoc, nLoc;
	GLuint globalAmbLoc, ambLoc, diffLoc, specLoc, posLoc, mambLoc, mdiffLoc, mspecLoc, mshiLoc;
	int width, height;
	float aspect;
	glm::mat4 pMat, vMat, mMat, mvMat, invTrMat, rMat;
	glm::vec3 currentLightPos, transformed;
	float lightPos[3];

	// white light
	float globalAmbient[4] = { 2.8f, 2.8f, 2.8f, 1.0f };
	float lightAmbient[4] = { 0.14f, 0.23f, 0.10f, 1.0f };
	float lightDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float lightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	// gold material
	float* matAmb = silverAmbient();
	float* matDif = silverDiffuse();
	float* matSpe = silverSpecular();
	float matShi = silverShininess();


	void window_size_callback2(GLFWwindow* win, int newWidth, int newHeight) {
		aspect = (float)newWidth / (float)newHeight;
		glViewport(0, 0, newWidth, newHeight);
		pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
	}

	//ImportedModel myModel("models/shuttle.obj");//飞船
	ImportedModel myModel("models/dolphinHighPoly.obj");//海豚
}

using namespace light_ADS_texture;

void loadVertices2(void) {
	std::vector<glm::vec3> vert = myModel.getVertices();
	std::vector<glm::vec2> tex = myModel.getTextureCoords();
	std::vector<glm::vec3>norm = myModel.getNormals();
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

	//创建VAO并返回他们的整数型ID，存进数组vao，
	//参数表示要创建多少个ID，参数二表示用来保存返回的ID的数组
	glGenVertexArrays(1, vao2);
	//将制定的VAO标记为活跃，这样生成的缓冲区就会和这个VAO相关联
	glBindVertexArray(vao2[0]);

	//创建VBO并返回他们的整数型ID，存进数组vao，
	glGenBuffers(3, vbo2);
	//将制定的VBO标记为活跃，将顶点放入缓冲区0
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]);
	//将包含四棱锥顶点数据的数组pyramidPositions复制进活跃缓冲区（这里是第1个vbo）
	//&pvalues[0]取容器第一个元素的地址
	glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);
	//将纹理坐标放入缓冲区1
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[1]);
	glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);
	//将法向量放入缓冲区2
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[2]);
	glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);
}

void installLights2(glm::mat4 vMatrix) {
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

void init_light2(GLFWwindow* window) {
	renderingProgram2 = createShaderProgram(ADS_TEXTURE);
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 100.0f;
	torLocX = 0.1f; torLocY = 0.1f; torLocZ = 0.1f;

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 2500.0f);

	loadVertices2();
	//brickTexture = loadTexture("texture/spstob_1.jpg");
	brickTexture = loadTexture("texture/Dolphin_HighPolyUV.png");
}

void display_light2(GLFWwindow* window, double currentTime) {
	glClear(GL_DEPTH_BUFFER_BIT);
	//glClearColor(1.0, 1.0, sin(currentTime), 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(renderingProgram2);

	mvLoc = glGetUniformLocation(renderingProgram2, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram2, "proj_matrix");
	nLoc = glGetUniformLocation(renderingProgram2, "norm_matrix");

	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX , -cameraY, -cameraZ));
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(torLocX * sin(currentTime),
		torLocY * cos(currentTime), torLocZ *sin(currentTime)));
	rMat = glm::rotate(glm::mat4(1.0f), 5.75f * (float)currentTime, glm::vec3(1.0f, 1.0f, 0.0f));
	mMat = mMat * rMat;

	currentLightPos = glm::vec3(initialLightLoc.x, initialLightLoc.y, initialLightLoc.z);
	rMat = glm::rotate(glm::mat4(1.0f), toRadians(currentTime), glm::vec3(1.0f, 1.0f, 1.0f));
	currentLightPos = glm::vec3(rMat * glm::vec4(currentLightPos, 1.0f));

	installLights2(vMat);

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

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[2]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glActiveTexture(GL_TEXTURE0);//激活第0个纹理单元
	glBindTexture(GL_TEXTURE_2D, brickTexture);//绑定纹理贴图

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, myModel.getNumVertices());
}


int main16(void) {
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(800, 800, "Chapter 7 - program 1", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	glfwSetWindowSizeCallback(window, window_size_callback2);

	init_light2(window);

	//map<int*, double> mymap;
	//int* p = nullptr;
	//int x = 1;
	//p = &x;
	//mymap.insert(pair<int*, double>(nullptr, 123));
	//mymap.insert(pair<int*, double>(nullptr, 543));
	//auto n = mymap.find(nullptr);
	//cout << n->second << endl;

	while (!glfwWindowShouldClose(window)) {
		display_light2(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);

}