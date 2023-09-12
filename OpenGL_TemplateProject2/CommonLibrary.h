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
#include <SOIL2/SOIL2.h>//加载贴图的库

using namespace std;

//Vertex Array Object 顶点数组对象
#define DISABLE 0
#define ENABLE_TESSELLATION 1
#define ENABLE_GEOMETRY 2
#define ENABLE_GEOMETRY_ONLY 3
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
#define TESSELLATION_GRID_ONLY 16
#define TESSELLATION_BEZIER 17
#define TESSELLATION_HEIGHT_MAPPED 18
#define TESSELLATION_HEIGHT_MAPPED_LIGHT 19
#define TESSELLATION_HEIGHT_MAPPED_LOD 20
#define GEOM_MOD_INFLATE 21
#define GEOM_MOD_INFLATE_ONLY 22
#define GEOM_MOD_INFLATE_ADD 23
#define GEOM_MOD_INFLATE_CHANGE 24
#define FOG 25 
#define BLENDING 26

//GLuint 是unsigned int 简写，许多OpenGL结构体都是整数类型引用
static GLuint renderingProgram;
//VAO 顶点数组对象
static GLuint vao[numVAOs];

void printShaderLog(GLuint shader);
void printProgramLog(int prog);
bool checkOpenGLError();

GLuint createShaderProgram();
GLuint createShaderProgram(int fun_id, int tessllation = DISABLE);
//GLuint createShaderProgram(const char* VS,const char* FS);

void init(GLFWwindow* window);
void init3D_Cube(GLFWwindow* window);
void init(GLFWwindow* window, int fun_id);

void display(GLFWwindow* window, double currentTime);
void display3Points(GLFWwindow* window, double currentTime);
void display3D_Cube(GLFWwindow* window, double currentTime);

string readShaderSource(const char* filePath);

//*****************************
//GLSL变换矩阵
//构建并返回平移坐标
using namespace glm;

glm::mat4 buildTranslate(float x, float y, float z);

//构建并返回绕x轴的旋转矩阵
mat4 buildRotateY(float rad);

mat4 buildRotateX(float rad);

mat4 buildRotateZ(float rad);

//加载贴图
GLuint loadTexture(const char* texImagePath);



/*******************************光照和材质********************************/
//黄金材质：环境光、漫反射、镜面反射和光泽度
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