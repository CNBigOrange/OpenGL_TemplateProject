#include "CommonLibrary.h"
#include <stack>
#include "ModelLoader.h"

namespace LoadModel {

	float cameraX, cameraY, cameraZ;
	float cubeLocX, cubeLocY, cubeLocZ, pyrLocX, pyrLocY, pyrLocZ;
	GLuint renderingProgram2;
	GLuint vao2[numVAOs];
	GLuint vbo2[3];
	GLuint brickTexture;

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

	ImportedModel myModel("models/shuttle.obj");//�ɴ�
	//ImportedModel myModel("models/dolphinHighPoly.obj");//����
}

using namespace LoadModel;

void loadVertices(void) {
	std::vector<glm::vec3> vert = myModel.getVertices();
	std::vector<glm::vec2> tex = myModel.getTextureCoords();
	std::vector<glm::vec3>norm = myModel.getNormals();
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

	//����VAO���������ǵ�������ID���������vao��
	//������ʾҪ�������ٸ�ID����������ʾ�������淵�ص�ID������
	glGenVertexArrays(1, vao2);
	//���ƶ���VAO���Ϊ��Ծ���������ɵĻ������ͻ�����VAO�����
	glBindVertexArray(vao2[0]);

	//����VBO���������ǵ�������ID���������vao��
	glGenBuffers(3, vbo2);
	//���ƶ���VBO���Ϊ��Ծ����������뻺����0
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]);
	//����������׶�������ݵ�����pyramidPositions���ƽ���Ծ�������������ǵ�1��vbo��
	//&pvalues[0]ȡ������һ��Ԫ�صĵ�ַ
	glBufferData(GL_ARRAY_BUFFER, pvalues.size()*4, &pvalues[0], GL_STATIC_DRAW);
	//������������뻺����1
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[1]);
	glBufferData(GL_ARRAY_BUFFER, tvalues.size()*4, &tvalues[0], GL_STATIC_DRAW);
	//�����������뻺����2
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[2]);
	glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);
}

void init3D_Model(GLFWwindow* window) {
	renderingProgram2 = createShaderProgram(SHOW_TEXTURE);
	//���λ��
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 0.0f;

	//����͸�Ӿ���
	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	//����1���ӳ�������Ƕȣ�Field Of View,FOV������2���ݺ��aspect radio������3������ƽ����룻����4��Զ��ƽ�����
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1500.0f);//1.0472 radians = 60 degress

	loadVertices();
	brickTexture = loadTexture("texture/spstob_1.jpg");
	//brickTexture = loadTexture("texture/Dolphin_HighPolyUV.png");
}

void display3D_Model(GLFWwindow* window, double currentTime) {
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
	//���������壨ʹ��0�Ż�������
	rMat = glm::rotate(glm::mat4(1.0f), 5.75f * (float)currentTime, glm::vec3(1.0f,1.0f, 0.0f));
	//rMat = glm::rotate(glm::mat4(1.0f), 30.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	mMat = glm::translate(glm::mat4(1.0f), 
		glm::vec3(cubeLocX, cubeLocY, -200 + cubeLocZ * (sin(currentTime) + 1) / 2));
	mMat *= glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 3.0f, 3.0f));
	mMat = mMat * rMat;
	//����mv����
	mvMat = vMat * mMat;

	//��͸�Ӿ����MV�����Ƹ���Ӧ��ͳһ����
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));

//---------------------------------------- ����׶ == ̫�� -----------------------------------------------------
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glActiveTexture(GL_TEXTURE0);//�����0������Ԫ
	glBindTexture(GL_TEXTURE_2D, brickTexture);//��������ͼ

	glEnable(GL_CULL_FACE);//�����޳�
	glFrontFace(GL_CCW);//CCW��ʾָ������˳����ʱ��Ϊ���棨Ĭ��ΪCCW����CWָ��˳ʱ��Ϊ����
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, myModel.getNumVertices());
}

int main14() {
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
	init3D_Model(window);
	//init(window,1);
	//��Ⱦѭ��
	while (!glfwWindowShouldClose(window)) {
		//glfwGetTime()����GLFW��ʼ��֮�󾭹���ʱ��
		display3D_Model(window, glfwGetTime());

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