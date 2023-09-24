#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "CommonLibrary.h"
#include "Utils.h"

using namespace std;
namespace ComputeShader {
	GLuint buffer[3];
	GLuint simpleComputeShader;

	int v1[] = { 10, 12, 16, 18, 50, 17 };
	int v2[] = { 30, 14, 80, 20, 51, 12 };
	int res[6];//用来存放结果的数组
}

using namespace ComputeShader;

void init_ComputeShader() {
	//simpleComputeShader = createShaderProgram(SIMPLE_COMPUTE_SHADER,ENABLE_COMPUTE_SHADER_ONLY);
	simpleComputeShader = Utils::createShaderProgram("simpleComputeShader.glsl");

	glGenBuffers(3, buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer[0]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(v1), v1, GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer[1]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(v2), v2, GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer[2]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(res), res, GL_STATIC_READ);
}

void computeSum() {
	glUseProgram(simpleComputeShader);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer[0]);//第一个输入矩阵
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, buffer[1]);//第二个输入矩阵
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, buffer[2]);//用来存储输出矩阵的缓冲区

	glDispatchCompute(6, 1, 1);//调用计算着色器6次——这些调用可以并行运行
	glMemoryBarrier(GL_ALL_BARRIER_BITS);//进入下一步之前确保着色器完成运算

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer[2]);
	//从结果缓冲区将结果取回数组res
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(res), res);
}


int main42(void) {
	int wait;
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(8, 8, "cs", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }

	init_ComputeShader();
	computeSum();
	std::cout << v1[0] << " " << v1[1] << " " << v1[2] << " " << v1[3] << " " << v1[4] << " " << v1[5] << std::endl;
	std::cout << v2[0] << " " << v2[1] << " " << v2[2] << " " << v2[3] << " " << v2[4] << " " << v2[5] << std::endl;
	std::cout << res[0] << " " << res[1] << " " << res[2] << " " << res[3] << " " << res[4] << " " << res[5] << std::endl;

	glfwDestroyWindow(window);
	glfwTerminate();
	std::cin >> wait;
	exit(EXIT_SUCCESS);
}