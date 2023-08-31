/*��һ����*/
#include "CommonLibrary.h"

using namespace std;

int main2() {
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	//���ô��ڵ�ѡ��
	//���汾��
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//���汾��
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//�����������ֱ������� ����ȫ�� �� ��Դ����
	GLFWwindow* window = glfwCreateWindow(1000, 600, "Chapter2 - program1", NULL, NULL);
	//�������������Ĺ�������
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	//����������ֱͬ��
	glfwSwapInterval(1);

	init(window,1);
	int i = 0;
	//��Ⱦѭ��
	while (!glfwWindowShouldClose(window) && i < 1000) {
		//glfwGetTime()����GLFW��ʼ��֮�󾭹���ʱ��
		display(window, glfwGetTime());
		//������Ļ
		glfwSwapBuffers(window);
		//�������¼����簴���¼���
		glfwPollEvents();
		i++;
		cout << i << endl;
	}

	//�رմ��ڣ���ֹ����
	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}