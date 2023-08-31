#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL2/SOIL2.h>//������ͼ�Ŀ�

using namespace std;

//Vertex Array Object �����������
#define numVAOs 1
#define numVBOs 2
#define DEBUG_MODE 1
#define LOAD_SHADER_FROM_FILE 2
#define SHOW_ANIMATION 3
#define SHOW_3D_CUBE 4
#define SHOW_3D_CUBE_INSTANCE 5
#define SHOW_TEXTURE 6
#define ADS_TEXTURE 7
#define SHADOWPASS1 8
#define SHADOWPASS2 9
#define SHADOWPASS3 10
#define SKY_CUBE 11
#define PROC_BUMP 12
#define NORMAL_MAPPING 13
#define NORMAL_MAPPING_MOON 14
#define HEIGHT_MAPPING 15

//GLuint ��unsigned int ��д������OpenGL�ṹ�嶼��������������
static GLuint renderingProgram;
//VAO �����������
static GLuint vao[numVAOs];

void printShaderLog(GLuint shader);
void printProgramLog(int prog);
bool checkOpenGLError();

GLuint createShaderProgram();
GLuint createShaderProgram(int fun_id);
//GLuint createShaderProgram(const char* VS,const char* FS);

void init(GLFWwindow* window);
void init3D_Cube(GLFWwindow* window);
void init(GLFWwindow* window, int fun_id);

void display(GLFWwindow* window, double currentTime);
void display3Points(GLFWwindow* window, double currentTime);
void display3D_Cube(GLFWwindow* window, double currentTime);

string readShaderSource(const char* filePath);

//*****************************
//GLSL�任����
//����������ƽ������
using namespace glm;

glm::mat4 buildTranslate(float x, float y, float z);

//������������x�����ת����
mat4 buildRotateY(float rad);

mat4 buildRotateX(float rad);

mat4 buildRotateZ(float rad);

//������ͼ
GLuint loadTexture(const char* texImagePath);



/*******************************���պͲ���********************************/
//�ƽ���ʣ������⡢�����䡢���淴��͹����
float* goldAmbient();
float* goldDiffuse();
float* goldSpecular();
float goldShininess();

float* silverAmbient();
float* silverDiffuse();
float* silverSpecular();
float silverShininess();

float* bronzeAmbient();
float* bronzeDiffuse();
float* bronzeSpecular();
float bronzeShininess();