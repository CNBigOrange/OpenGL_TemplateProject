#include "CommonLibrary.h"

namespace MoreCubeInstance {

	float cameraX, cameraY, cameraZ;
	float cubeLocX, cubeLocY, cubeLocZ;
	GLuint renderingProgram2;
	GLuint vao2[numVAOs];
	GLuint vbo2[numVBOs];

	//������ display(��������ʹ�õı����ռ䣬�������ǾͲ�������Ⱦ�����з���
	GLuint vLoc, projLoc;
	int width{ 600 }, height{ 600 };
	float aspect;
	glm::mat4 pMat, vMat, mMat, mvMat, rMat;
}

using namespace MoreCubeInstance;

void setupVertices3(void) {//36�����㣬12�������Σ�����˷�����ԭ�㴦��2x2x2������
	float vertexPositions[108] = {
		-1.0f,1.0f,-1.0f,-1.0f,-1.0f,-1.0f,1.0f,-1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,1.0f,1.0f,-1.0f,-1.0f,1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,1.0f,-1.0f,1.0f,1.0f,1.0f,-1.0f,
		1.0f,-1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,-1.0f,
		1.0f,-1.0f,1.0f,-1.0f,-1.0f,1.0f,1.0f,1.0f,1.0f,
		-1.0f,-1.0f,1.0f,-1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,
		-1.0f,-1.0f,1.0f,-1.0f,-1.0f,-1.0f,-1.0f,1.0f,1.0f,
		-1.0f,-1.0f,-1.0f,-1.0f,1.0f,-1.0f,-1.0f,1.0f,1.0f,
		-1.0f,-1.0f,1.0f,1.0f,-1.0f,1.0f,1.0f,-1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,-1.0f,-1.0f,-1.0f,-1.0f,-1.0f,1.0f,
		-1.0f,1.0f,-1.0f,1.0f,1.0f,-1.0f,1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,-1.0f,1.0f,1.0f,-1.0f,1.0f,-1.0f,
	};


	//����VAO���������ǵ�������ID���������vao��
	//������ʾҪ�������ٸ�ID����������ʾ�������淵�ص�ID������
	glGenVertexArrays(1, vao2);
	//���ƶ���VAO���Ϊ��Ծ���������ɵĻ������ͻ�����VAO�����
	glBindVertexArray(vao2[0]);

	//����VBO���������ǵ�������ID���������vao��
	glGenBuffers(numVBOs, vbo2);
	//���ƶ���VBO���Ϊ��Ծ
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]);
	//�������������ݵ�����vertexPositions���ƽ���Ծ�������������ǵ�0��vbo��
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
}

void init3D_Cube3(GLFWwindow* window) {
	renderingProgram2 = createShaderProgram(SHOW_3D_CUBE_INSTANCE);
	//���λ��
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 3000.0f;
	//������λ��
	cubeLocX = 0.0f; cubeLocY = 0.0f; cubeLocZ = -1500.0f;//��y��������չʾ͸��
	setupVertices3();
}

void display3D_Cube3(GLFWwindow* window, double currentTime) {
	glClear(GL_DEPTH_BUFFER_BIT);
	//glClearColor(1.0, 1.0, sin(currentTime), 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(renderingProgram2);

	//��ȡMV�����ͶӰ�����ͳһ����
	vLoc = glGetUniformLocation(renderingProgram2, "v_matrix");
	projLoc = glGetUniformLocation(renderingProgram2, "proj_matrix");
	auto tfLoc = glGetUniformLocation(renderingProgram2, "tf");

	//����͸�Ӿ���
	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	//����1���ӳ�������Ƕȣ�Field Of View,FOV������2���ݺ��aspect radio������3������ƽ����룻����4��Զ��ƽ�����
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 30000.0f);//1.0472 radians = 60 degress

	//������ͼ����ģ�;����MV����
	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));

	float timeFactor;
	timeFactor = (float)currentTime;//ͳһʱ��������Ϣ

	//��͸�Ӿ����MV�����Ƹ���Ӧ��ͳһ����
	glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(vMat));//��ɫ����ͼ����
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniform1f(tfLoc, (float)timeFactor);

	//��VBO������������ɫ������Ӧ�Ķ�������
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	//����OpengGL���ã�����ģ��
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 36, 10000);
}

int main8() {
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	//���ô��ڵ�ѡ��
	//���汾��
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//���汾��
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//�����������ֱ������� ����ȫ�� �� ��Դ����
	GLFWwindow* window = glfwCreateWindow(1000, 800, "Chapter4 - program6", NULL, NULL);
	//�������������Ĺ�������
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	//����������ֱͬ��
	glfwSwapInterval(1);

	init3D_Cube3(window);
	//init(window,1);
	//��Ⱦѭ��
	while (!glfwWindowShouldClose(window)) {
		//glfwGetTime()����GLFW��ʼ��֮�󾭹���ʱ��
		display3D_Cube3(window, glfwGetTime());

		//������Ļ
		glfwSwapBuffers(window);
		//�������¼����簴���¼���
		glfwPollEvents();
	}

	//�رմ��ڣ���ֹ����
	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}