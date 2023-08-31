#include "CommonLibrary.h"

namespace MoreCubeInstance {

	float cameraX, cameraY, cameraZ;
	float cubeLocX, cubeLocY, cubeLocZ;
	GLuint renderingProgram2;
	GLuint vao2[numVAOs];
	GLuint vbo2[numVBOs];

	//分配在 display(）函数中使用的变量空间，这样它们就不必在渲染过程中分配
	GLuint vLoc, projLoc;
	int width{ 600 }, height{ 600 };
	float aspect;
	glm::mat4 pMat, vMat, mMat, mvMat, rMat;
}

using namespace MoreCubeInstance;

void setupVertices3(void) {//36个顶点，12个三角形，组成了放置在原点处的2x2x2立方体
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


	//创建VAO并返回他们的整数型ID，存进数组vao，
	//参数表示要创建多少个ID，参数二表示用来保存返回的ID的数组
	glGenVertexArrays(1, vao2);
	//将制定的VAO标记为活跃，这样生成的缓冲区就会和这个VAO相关联
	glBindVertexArray(vao2[0]);

	//创建VBO并返回他们的整数型ID，存进数组vao，
	glGenBuffers(numVBOs, vbo2);
	//将制定的VBO标记为活跃
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]);
	//将包含顶点数据的数组vertexPositions复制进活跃缓冲区（这里是第0个vbo）
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
}

void init3D_Cube3(GLFWwindow* window) {
	renderingProgram2 = createShaderProgram(SHOW_3D_CUBE_INSTANCE);
	//相机位置
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 3000.0f;
	//立方体位置
	cubeLocX = 0.0f; cubeLocY = 0.0f; cubeLocZ = -1500.0f;//沿y轴下移以展示透视
	setupVertices3();
}

void display3D_Cube3(GLFWwindow* window, double currentTime) {
	glClear(GL_DEPTH_BUFFER_BIT);
	//glClearColor(1.0, 1.0, sin(currentTime), 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(renderingProgram2);

	//获取MV矩阵和投影矩阵的统一变量
	vLoc = glGetUniformLocation(renderingProgram2, "v_matrix");
	projLoc = glGetUniformLocation(renderingProgram2, "proj_matrix");
	auto tfLoc = glGetUniformLocation(renderingProgram2, "tf");

	//构建透视矩阵
	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	//参数1：视场（纵向角度）Field Of View,FOV；参数2：纵横比aspect radio；参数3：近裁平面距离；参数4：远裁平面距离
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 30000.0f);//1.0472 radians = 60 degress

	//构建视图矩阵、模型矩阵和MV矩阵
	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));

	float timeFactor;
	timeFactor = (float)currentTime;//统一时间因子信息

	//将透视矩阵和MV矩阵复制给相应的统一变量
	glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(vMat));//着色器视图矩阵
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniform1f(tfLoc, (float)timeFactor);

	//将VBO关联给顶点着色器中相应的顶点属性
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	//调整OpengGL设置，绘制模型
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 36, 10000);
}

int main8() {
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	//设置窗口的选项
	//主版本号
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//副版本号
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//后两个参数分别是设置 允许全屏 和 资源共享
	GLFWwindow* window = glfwCreateWindow(1000, 800, "Chapter4 - program6", NULL, NULL);
	//将窗口与上下文关联起来
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	//用来开启垂直同步
	glfwSwapInterval(1);

	init3D_Cube3(window);
	//init(window,1);
	//渲染循环
	while (!glfwWindowShouldClose(window)) {
		//glfwGetTime()返回GLFW初始化之后经过的时间
		display3D_Cube3(window, glfwGetTime());

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