#include <string>
#include <iostream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "ModelLoader.h"
#include "CommonLibrary.h"
#include <map>

void passOne(double currentTime);
void passTwo(double currentTime);

using namespace std;
namespace light_Shadow {

	float toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }

#define numVAOs 1
#define numVBOs 6

	GLuint renderingProgram2, renderingProgram3, renderingProgram4;
	GLuint vao2[numVAOs];
	GLuint vbo2[numVBOs];
	GLuint brickTexture, brickTexture2;

	float amt = 1.0f;

	glm::vec3 pyrLoc(0.0f, -0.6f, 0.0f);
	glm::vec3 modelLoc(0.0f, 0.1f, 2.5f);
	glm::vec3 cameraLoc(0.0f, -0.25f, 100.0f);
	glm::vec3 lightLoc( 0.0f, 3.5f, 5.5f);

	// variable allocation for display
	GLuint mvLoc, projLoc, nLoc,sLoc;
	int width, height;
	float aspect;
	glm::mat4 pMat, vMat, mMat, mMat2, mvMat, mvMat2, invTrMat, rMat;
	glm::vec3 currentLightPos, transformed;
	float lightPos[3];
	GLuint globalAmbLoc, ambLoc, diffLoc, specLoc, posLoc, mambLoc, mdiffLoc, mspecLoc, mshiLoc;
	glm::vec3 origin(0.0f, 0.0f, 0.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);
	int numPyramidVertices;
	// white light
	float globalAmbient[4] = { 2.8f, 2.8f, 2.8f, 1.0f };
	float lightAmbient[4] = { 0.14f, 0.23f, 0.10f, 1.0f };
	float lightDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float lightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	// gold material
	float* gMatAmb = silverAmbient();
	float* gMatDif = silverDiffuse();
	float* gMatSpe = silverSpecular();
	float gMatShi = silverShininess();

	//bronze material
	float* bMatAmb = bronzeAmbient();
	float* bMatDif = bronzeDiffuse();
	float* bMatSpe = bronzeSpecular();
	float bMatShi = bronzeShininess();

	float thisAmb[4], thisDif[4], thisSpe[4], matAmb[4], matDif[4], matSpe[4];
	float thisShi, matShi;

	//shadow stuff
	int scSizeX, scSizeY;
	GLuint shadowTex, shadowBuffer;
	glm::mat4 lightVmatrix;
	glm::mat4 lightPmatrix;
	glm::mat4 shadowMVP1;
	glm::mat4 shadowMVP2;
	glm::mat4 shadowMVP3;
	glm::mat4 b;

	void window_size_callback2(GLFWwindow* win, int newWidth, int newHeight) {
		aspect = (float)newWidth / (float)newHeight;
		glViewport(0, 0, newWidth, newHeight);
		pMat = glm::perspective(1.0472f, aspect, 0.1f, 2000.0f);
	}

	//ImportedModel myModel("models/shuttle.obj");//�ɴ�
	//ImportedModel myModel("models/dolphinHighPoly.obj");//����
	ImportedModel pyramid("models/shuttle.obj");
	ImportedModel myModel("models/dolphinHighPoly.obj");
}

using namespace light_Shadow;

void loadVertices3(void) {
	std::vector<glm::vec3> vert = myModel.getVertices();
	std::vector<glm::vec2> tex = myModel.getTextureCoords();
	std::vector<glm::vec3> norm = myModel.getNormals();
	int numObjVertices = myModel.getNumVertices();

	std::vector<float> pvalues;//����λ��
	std::vector<float> tvalues;//��������
	std::vector<float> nvalues;//������

	//������ȥ�Ҷ�Ӧ�������ꡢ�������ꡢ������
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

	// pyramid definition
	numPyramidVertices = pyramid.getNumVertices();
	std::vector<glm::vec3> vert2 = pyramid.getVertices();
	std::vector<glm::vec3> norm2 = pyramid.getNormals();
	std::vector<glm::vec2> tex2 = pyramid.getTextureCoords();
	std::vector<float> pyramidPvalues;
	std::vector<float> pyramidNvalues;
	std::vector<float> pyramidTvalues;

	for (int i = 0; i < numPyramidVertices; i++) {
		pyramidPvalues.push_back((vert2[i]).x);
		pyramidPvalues.push_back((vert2[i]).y);
		pyramidPvalues.push_back((vert2[i]).z);

		pyramidNvalues.push_back((norm2[i]).x);
		pyramidNvalues.push_back((norm2[i]).y);
		pyramidNvalues.push_back((norm2[i]).z);

		pyramidTvalues.push_back((tex2[i]).s);
		pyramidTvalues.push_back((tex2[i]).t);
	}

	//����VAO���������ǵ�������ID���������vao��
	//������ʾҪ�������ٸ�ID����������ʾ�������淵�ص�ID������
	glGenVertexArrays(1, vao2);
	//���ƶ���VAO���Ϊ��Ծ���������ɵĻ������ͻ�����VAO�����
	glBindVertexArray(vao2[0]);

	//����VBO���������ǵ�������ID���������vao��
	glGenBuffers(numVBOs, vbo2);
	//���ƶ���VBO���Ϊ��Ծ����������뻺����0
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]);
	//����������׶�������ݵ�����pyramidPositions���ƽ���Ծ�������������ǵ�1��vbo��
	//&pvalues[0]ȡ������һ��Ԫ�صĵ�ַ
	glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);
	//������������뻺����1
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[1]);
	glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);
	//�����������뻺����2
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[2]);
	glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[3]);
	glBufferData(GL_ARRAY_BUFFER, pyramidPvalues.size() * 4, &pyramidPvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[4]);
	glBufferData(GL_ARRAY_BUFFER, pyramidNvalues.size() * 4, &pyramidNvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[5]);
	glBufferData(GL_ARRAY_BUFFER, pyramidTvalues.size() * 4, &pyramidTvalues[0], GL_STATIC_DRAW);
}

void installLights3(int renderingProgram,glm::mat4 vMatrix) {
	transformed = glm::vec3(vMatrix * glm::vec4(currentLightPos, 1.0));
	lightPos[0] = transformed.x;
	lightPos[1] = transformed.y;
	lightPos[2] = transformed.z;

	matAmb[0] = thisAmb[0]; matAmb[1] = thisAmb[1]; matAmb[2] = thisAmb[2]; matAmb[3] = thisAmb[3];
	matDif[0] = thisDif[0]; matDif[1] = thisDif[1]; matDif[2] = thisDif[2]; matDif[3] = thisDif[3];
	matSpe[0] = thisSpe[0]; matSpe[1] = thisSpe[1]; matSpe[2] = thisSpe[2]; matSpe[3] = thisSpe[3];
	matShi = thisShi;

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
//����Ȼ��������Ƶ�����
void setupShadowBuffers(GLFWwindow* window) {
	
	//���֡��������С
	glfwGetFramebufferSize(window, &width, &height);
	scSizeX = width;
	scSizeY = height;
	//�����Զ���֡������������Ӱ�������ӵ�������
	glGenFramebuffers(1, &shadowBuffer);
	//������Ӱ�����������洢�����Ϣ
	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32,
		scSizeX, scSizeY, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	// may reduce shadow border artifacts������Ӱ�߽�αӰ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void init_light3(GLFWwindow* window) {
	renderingProgram2 = createShaderProgram(SHADOWPASS1);
	//renderingProgram3 = createShaderProgram(SHADOWPASS2);//Ƭ����ɫ��Ϊ������Ӱ
	renderingProgram3 = createShaderProgram(SHADOWPASS3);//Ƭ����ɫ��Ϊ4��������PCF�ٷֱ��ڽ��˲�
	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	loadVertices3();
	setupShadowBuffers(window);
	brickTexture2 = loadTexture("texture/spstob_1.jpg");
	//brickTexture = loadTexture("texture/Dolphin_HighPolyUV.png");
	brickTexture = loadTexture("texture/Dolphin_HighPolyUV.png");

	//���������[-1��+1]����������[0,1]
	b = glm::mat4(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f
	);
}

void display_light3(GLFWwindow* window, double currentTime) {
	glClear(GL_DEPTH_BUFFER_BIT);
	//glClearColor(1.0, 1.0, sin(currentTime), 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	currentLightPos = glm::vec3(lightLoc);
	//�ӹ�Դ�ӽǳ�ʼ���Ӿ������Լ�͸�Ӿ����Ա��ڵ�һ����ʹ��
	lightVmatrix = glm::lookAt(currentLightPos, origin, up);//�ӹ�Դ��ԭ��ľ���
	lightPmatrix = glm::perspective(toRadians(60.0f), aspect, 0.1f, 1000.0f);
	//ʹ���Զ���֡������������Ӱ�������ŵ�����
	glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowTex, 0);
	//�رջ�����ɫ��ͬʱ������ȼ���
	glDrawBuffer(GL_NONE);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_POLYGON_OFFSET_FILL);//�ڵ�һ�ּ����н�ÿ��������΢�����Դ��������Ȳ������αӰ
	//glPolygonOffset(2.1f, 3.0f);//�����ƫ��

	passOne(currentTime);

	//glDisable(GL_POLYGON_OFFSET_FILL);

	//ʹ����ʾ�������������¿�������
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	glDrawBuffer(GL_FRONT);//���¿���������ɫ

	passTwo(currentTime);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void passOne(double currentTime) {
	//renderingProgram1 ������һ���еĶ�����ɫ����Ƭ����ɫ��
	glUseProgram(renderingProgram2);
	//����������δӹ�Դ�Ƕ���Ⱦ��������Ȼ�����

	mMat = glm::translate(glm::mat4(1.0f), modelLoc);
	rMat = glm::rotate(glm::mat4(1.0f), (float)(10 * currentTime), glm::vec3(1.0f, 1.0f, 0.0f));
	mMat = mMat * rMat;
	//�ӹ�Դ�ǶȻ��ƣ����ʹ�ù�Դ�ľ���P,V
	shadowMVP1 = lightPmatrix * lightVmatrix * mMat;
	sLoc = glGetUniformLocation(renderingProgram2, "shadowMVP");
	glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP1));

	//��һ��������ֻ�軷��Ķ��㻺������������Ҫ��������������
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, myModel.getNumVertices());

	// draw the pyramid

	mMat2 = glm::translate(glm::mat4(1.0f), pyrLoc);
	mMat2 = glm::rotate(mMat2, toRadians(1800.0f*currentTime), glm::vec3(1.0f, -1.0f, 1.0f));
	mMat2 = glm::scale(mMat2, glm::vec3(0.8, 0.8, 0.8));
	mvMat = vMat * mMat2;

	shadowMVP1 = lightPmatrix * lightVmatrix * mMat2;
	glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP1));

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[3]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, pyramid.getNumVertices());
}

void passTwo(double currentTime) {
	//renderingProgram1 ������һ���еĶ�����ɫ����Ƭ����ɫ��
	glUseProgram(renderingProgram3);

	mvLoc = glGetUniformLocation(renderingProgram3, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram3, "proj_matrix");
	nLoc = glGetUniformLocation(renderingProgram3, "norm_matrix");
	sLoc = glGetUniformLocation(renderingProgram3, "shadowMVP");

	thisAmb[0] = gMatAmb[0]; thisAmb[1] = gMatAmb[1]; thisAmb[2] = gMatAmb[2];  // gold
	thisDif[0] = gMatDif[0]; thisDif[1] = gMatDif[1]; thisDif[2] = gMatDif[2];
	thisSpe[0] = gMatSpe[0]; thisSpe[1] = gMatSpe[1]; thisSpe[2] = gMatSpe[2];
	thisShi = gMatShi;

	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraLoc.x, -cameraLoc.y, -cameraLoc.z));


	currentLightPos = glm::vec3(lightLoc);
	currentLightPos = glm::vec3(rMat * glm::vec4(currentLightPos, 1.0f));

	installLights3(renderingProgram3, vMat);

	mMat = glm::translate(glm::mat4(1.0f), modelLoc);
	rMat = glm::rotate(glm::mat4(1.0f), (float)(10 * currentTime), glm::vec3(1.0f, 1.0f, 0.0f));
	mMat = mMat * rMat;

	mvMat = vMat * mMat;
	//��mv�������ת�þ���������ȷ�ķ�����
	invTrMat = glm::transpose(glm::inverse(mvMat));
	//������Դ�ӽǻ����MV����
	shadowMVP2 = b * lightPmatrix * lightVmatrix * mMat;
	//��MV�Լ�PROJ��������Ӧ��ͳһ����
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP2));

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[2]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glActiveTexture(GL_TEXTURE1);//�����1��������Ԫ
	glBindTexture(GL_TEXTURE_2D, brickTexture);//��������ͼ

	//glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, myModel.getNumVertices());

	// draw the pyramid
	//glUseProgram(renderingProgram3);
	//mvLoc = glGetUniformLocation(renderingProgram3, "mv_matrix");
	//projLoc = glGetUniformLocation(renderingProgram3, "proj_matrix");
	//nLoc = glGetUniformLocation(renderingProgram3, "norm_matrix");
	//sLoc = glGetUniformLocation(renderingProgram3, "shadowMVP");

	//thisAmb[0] = bMatAmb[0]; thisAmb[1] = bMatAmb[1]; thisAmb[2] = bMatAmb[2];  // bronze
	//thisDif[0] = bMatDif[0]; thisDif[1] = bMatDif[1]; thisDif[2] = bMatDif[2];
	//thisSpe[0] = bMatSpe[0]; thisSpe[1] = bMatSpe[1]; thisSpe[2] = bMatSpe[2];
	//thisShi = bMatShi;

	installLights3(renderingProgram3, vMat);

	mMat2 = glm::translate(glm::mat4(1.0f), pyrLoc);
	mMat2 = glm::rotate(mMat2, toRadians(1800.0f * currentTime), glm::vec3(1.0f, -1.0f, 1.0f));
	mMat2 = glm::scale(mMat2, glm::vec3(0.8, 0.8, 0.8));
	mvMat = vMat * mMat2;
	invTrMat = glm::transpose(glm::inverse(mvMat));
	shadowMVP3 = b * lightPmatrix * lightVmatrix * mMat2;

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP3));

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[3]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[5]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[4]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindTexture(GL_TEXTURE_2D, brickTexture2);//��������ͼ

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, pyramid.getNumVertices());
}

int main17(void) {
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(800, 800, "Chapter 7 - program 1", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	glfwSetWindowSizeCallback(window, window_size_callback2);

	init_light3(window);

	while (!glfwWindowShouldClose(window)) {
		display_light3(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);

}