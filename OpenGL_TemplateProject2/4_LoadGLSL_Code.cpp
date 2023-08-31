#include "CommonLibrary.h"

int main4() {
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	//设置窗口的选项
	//主版本号
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//副版本号
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//后两个参数分别是设置 允许全屏 和 资源共享
	GLFWwindow* window = glfwCreateWindow(1000, 600, "Chapter2 - program1", NULL, NULL);
	//将窗口与上下文关联起来
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	//用来开启垂直同步
	glfwSwapInterval(1);

	init(window, LOAD_SHADER_FROM_FILE);
	//渲染循环
	while (!glfwWindowShouldClose(window)) {
		//glfwGetTime()返回GLFW初始化之后经过的时间
		display3Points(window, glfwGetTime());
		//绘制屏幕
		glfwSwapBuffers(window);
		//处理窗口事件（如按键事件）
		glfwPollEvents();
	}

	//关闭窗口，终止运行
	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}