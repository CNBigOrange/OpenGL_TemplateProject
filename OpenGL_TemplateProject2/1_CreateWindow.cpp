/*��glfw����һ������*/
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
	//��ʼ��glfw��
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	//���ô��ڵ�ѡ��
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);//���汾��
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//���汾��
	GLFWwindow* window = glfwCreateWindow(1000, 600, "Chapter2 - program1", NULL, NULL);//�����������ֱ������� ����ȫ�� �� ��Դ����
	glfwMakeContextCurrent(window);//�������������Ĺ�������

	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	init1(window);
	int i = 0;
	//��Ⱦѭ��
	while (!glfwWindowShouldClose(window)&& i<1000) {
		display1(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
		i++;
		cout << i << endl;
	}

	//�رմ��ڣ���ֹ����
	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}
