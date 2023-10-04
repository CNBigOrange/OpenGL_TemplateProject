#include "CommonLibrary.h"
#include <stack>

namespace StackMat4 {

	float cameraX, cameraY, cameraZ;
	float cubeLocX, cubeLocY, cubeLocZ, pyrLocX, pyrLocY, pyrLocZ;
	GLuint renderingProgram2;
	GLuint vao2[numVAOs];
	GLuint vbo2[numVBOs];

	//������ display(��������ʹ�õı����ռ䣬�������ǾͲ�������Ⱦ�����з���
	GLuint mvLoc, projLoc;
	int width{ 600 }, height{ 600 };
	float aspect;
	glm::mat4 pMat, vMat, mMat, mvMat, rMat;
	stack<glm::mat4> mvStack;

	void window_reshape_callback(GLFWwindow* window, int newWidth, int newHeight) {
		aspect = (float)newWidth / (float)newHeight;
		glViewport(0, 0, newWidth, newHeight);
		pMat = glm::perspective(1.0472f, aspect, 0.1f, 1500.0f);//1.0472 radians = 60 degress
	}
}

using namespace StackMat4;

void setupVertices5(void) {
	//36�����㣬12�������Σ�����˷�����ԭ�㴦��2x2x2������
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
	//����׶��18�����㣬��6����������ɣ�����4��������2����
	float pyramidPositions[54] = {
		-1.0f,-1.0f,1.0f,1.0f,-1.0f,1.0f,0.0f,1.0f,0.0f,//ǰ��
		1.0f,-1.0f,1.0f,1.0f,-1.0f,-1.0f,0.0f,1.0f,0.0f,//����
		1.0f,-1.0f,-1.0f,-1.0f,-1.0f,-1.0f,0.0f,1.0f,0.0f,//����
		-1.0f,-1.0f,-1.0f,-1.0f,-1.0f,1.0f,0.0f,1.0f,0.0f,//����
		-1.0f,-1.0f,-1.0f,1.0f,-1.0f,1.0f,-1.0f,-1.0f,1.0f,//������ǰ
		1.0f,-1.0f,1.0f,-1.0f,-1.0f,-1.0f,1.0f,-1.0f,-1.0f//�����Һ�
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
	//�����������嶥�����ݵ�����vertexPositions���ƽ���Ծ�������������ǵ�0��vbo��
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
	//����������׶�������ݵ�����pyramidPositions���ƽ���Ծ�������������ǵ�1��vbo��
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidPositions), pyramidPositions, GL_STATIC_DRAW);
}

void init3D_Shape5(GLFWwindow* window) {
	renderingProgram2 = createShaderProgram(SHOW_3D_CUBE);
	//���λ��
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 500.0f;

	//����͸�Ӿ���
	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	//����1���ӳ�������Ƕȣ�Field Of View,FOV������2���ݺ��aspect radio������3������ƽ����룻����4��Զ��ƽ�����
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1500.0f);//1.0472 radians = 60 degress

	setupVertices5();
}

void display3D_Shape5(GLFWwindow* window, double currentTime) {
	glClear(GL_DEPTH_BUFFER_BIT);
	//glClearColor(1.0, 1.0, sin(currentTime), 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(renderingProgram2);

	//��ȡMV�����ͶӰ�����ͳһ����
	mvLoc = glGetUniformLocation(renderingProgram2, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram2, "proj_matrix");

	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	//����ͼ����ѹ��ջ
	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
	mvStack.push(vMat);

//---------------------------------------- ����׶ == ̫�� -----------------------------------------------------
	//�ٷ���һ��ջ������
	mvStack.push(mvStack.top());
	//̫��λ�� ջ������ƽ�ƾ���
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -500.0));
	mvStack.push(mvStack.top());
	//̫����ת
	mvStack.top() *= glm::rotate(glm::mat4(1.0f), 15.75f * (float)currentTime, glm::vec3( 1.0f, 1.0f, 0.0f));
	mvStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(2.25f, 2.25f, 2.25f));

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LEQUAL);
	//����̫��
	glDrawArrays(GL_TRIANGLES, 0, 18);
	//��ջ���Ƴ�̫������ת
	mvStack.pop();

	//------------------------------------ �������� == ���� -----------------------------------------------------
	mvStack.push(mvStack.top());
	mvStack.top() *=
		glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)currentTime) * 4.0,
			sin((float)currentTime) * 4.0, cos((float)currentTime) * 4.0));
	mvStack.push(mvStack.top());
	//������ת
	mvStack.top() *= glm::rotate(glm::mat4(1.0f), 15.75f * (float)currentTime, glm::vec3(0.0, 1.0, 0.0));
	
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	//����̫��
	glDrawArrays(GL_TRIANGLES, 0, 36);
	//��ջ���Ƴ�̫������ת
	mvStack.pop();

	//------------------------------------ С������ == ���� -----------------------------------------------------
	mvStack.push(mvStack.top());
	mvStack.top() *=
		glm::translate(glm::mat4(1.0f), glm::vec3( 0.0,
			sin((float)currentTime) * 2.0, cos((float)currentTime) * 2.0));
	//������ת
	mvStack.top() *= glm::rotate(glm::mat4(1.0f), 15.75f * (float)currentTime, glm::vec3(0.0, 0.0, 1.0));
	//������СһЩ
	mvStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f));
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	//����̫��
	glDrawArrays(GL_TRIANGLES, 0, 36);
	//��ջ���Ƴ�̫������ת
	mvStack.pop();


	//------------------------------------ С������ == �߿� -----------------------------------------------------
	mvStack.push(mvStack.top());
	mvStack.top() *=
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0,
			sin((float)currentTime) * 2.0, cos((float)currentTime) * 2.0));
	//������ת
	mvStack.top() *= glm::rotate(glm::mat4(1.0f), 15.75f * (float)currentTime, glm::vec3(0.0, 1.0, 1.0));
	//������СһЩ
	mvStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(0.35f, 0.35f, 0.35f));
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	//����̫��
	glDrawArrays(GL_LINE_LOOP, 0, 36);
	//��ջ���Ƴ�̫������ת
	mvStack.pop();
}

int main10() {
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	//���ô��ڵ�ѡ��
	//���汾��
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//���汾��
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//�����������ֱ������� ����ȫ�� �� ��Դ����
	GLFWwindow* window = glfwCreateWindow(1000, 600, "Chapter4 - program6", NULL, NULL);
	//�������������Ĺ�������
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	//����������ֱͬ��
	glfwSwapInterval(1);
	glfwSetWindowSizeCallback(window, window_reshape_callback);
	init3D_Shape5(window);
	//init(window,1);
	//��Ⱦѭ��
	while (!glfwWindowShouldClose(window)) {
		//glfwGetTime()����GLFW��ʼ��֮�󾭹���ʱ��
		display3D_Shape5(window, glfwGetTime());

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