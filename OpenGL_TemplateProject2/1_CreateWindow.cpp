/*用glfw创建一个窗口*/
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

using namespace std;

void init1(GLFWwindow* window) {}

void display1(GLFWwindow* window, double currentTime) {
		glClearColor(1.0, 1.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT); 
}

int main1() {
	//初始化glfw库
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	//设置窗口的选项
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);//主版本号
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//副版本号
	GLFWwindow* window = glfwCreateWindow(1000, 600, "Chapter2 - program1", NULL, NULL);//后两个参数分别是设置 允许全屏 和 资源共享
	glfwMakeContextCurrent(window);//将窗口与上下文关联起来

	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	init1(window);
	int i = 0;
	//渲染循环
	while (!glfwWindowShouldClose(window)&& i<1000) {
		display1(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
		i++;
		cout << i << endl;
	}

	//关闭窗口，终止运行
	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}
