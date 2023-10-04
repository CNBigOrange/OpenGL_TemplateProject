#include "CommonLibrary.h"
#include <stack>

namespace StackMat4 {

	float cameraX, cameraY, cameraZ;
	float cubeLocX, cubeLocY, cubeLocZ, pyrLocX, pyrLocY, pyrLocZ;
	GLuint renderingProgram2;
	GLuint vao2[numVAOs];
	GLuint vbo2[numVBOs];

	//分配在 display(）函数中使用的变量空间，这样它们就不必在渲染过程中分配
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
	//36个顶点，12个三角形，组成了放置在原点处的2x2x2立方体
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
	//四棱锥有18个顶点，由6个三角形组成（侧面4个，底面2个）
	float pyramidPositions[54] = {
		-1.0f,-1.0f,1.0f,1.0f,-1.0f,1.0f,0.0f,1.0f,0.0f,//前面
		1.0f,-1.0f,1.0f,1.0f,-1.0f,-1.0f,0.0f,1.0f,0.0f,//右面
		1.0f,-1.0f,-1.0f,-1.0f,-1.0f,-1.0f,0.0f,1.0f,0.0f,//后面
		-1.0f,-1.0f,-1.0f,-1.0f,-1.0f,1.0f,0.0f,1.0f,0.0f,//左面
		-1.0f,-1.0f,-1.0f,1.0f,-1.0f,1.0f,-1.0f,-1.0f,1.0f,//底面左前
		1.0f,-1.0f,1.0f,-1.0f,-1.0f,-1.0f,1.0f,-1.0f,-1.0f//底面右后
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
	//将包含立方体顶点数据的数组vertexPositions复制进活跃缓冲区（这里是第0个vbo）
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
	//将包含四棱锥顶点数据的数组pyramidPositions复制进活跃缓冲区（这里是第1个vbo）
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidPositions), pyramidPositions, GL_STATIC_DRAW);
}

void init3D_Shape5(GLFWwindow* window) {
	renderingProgram2 = createShaderProgram(SHOW_3D_CUBE);
	//相机位置
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 500.0f;

	//构建透视矩阵
	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	//参数1：视场（纵向角度）Field Of View,FOV；参数2：纵横比aspect radio；参数3：近裁平面距离；参数4：远裁平面距离
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1500.0f);//1.0472 radians = 60 degress

	setupVertices5();
}

void display3D_Shape5(GLFWwindow* window, double currentTime) {
	glClear(GL_DEPTH_BUFFER_BIT);
	//glClearColor(1.0, 1.0, sin(currentTime), 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(renderingProgram2);

	//获取MV矩阵和投影矩阵的统一变量
	mvLoc = glGetUniformLocation(renderingProgram2, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram2, "proj_matrix");

	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	//将视图矩阵压入栈
	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
	mvStack.push(vMat);

//---------------------------------------- 四棱锥 == 太阳 -----------------------------------------------------
	//再放入一个栈顶矩阵
	mvStack.push(mvStack.top());
	//太阳位置 栈顶乘以平移矩阵
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -500.0));
	mvStack.push(mvStack.top());
	//太阳旋转
	mvStack.top() *= glm::rotate(glm::mat4(1.0f), 15.75f * (float)currentTime, glm::vec3( 1.0f, 1.0f, 0.0f));
	mvStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(2.25f, 2.25f, 2.25f));

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LEQUAL);
	//绘制太阳
	glDrawArrays(GL_TRIANGLES, 0, 18);
	//从栈中移除太阳的自转
	mvStack.pop();

	//------------------------------------ 大立方体 == 地球 -----------------------------------------------------
	mvStack.push(mvStack.top());
	mvStack.top() *=
		glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)currentTime) * 4.0,
			sin((float)currentTime) * 4.0, cos((float)currentTime) * 4.0));
	mvStack.push(mvStack.top());
	//地球旋转
	mvStack.top() *= glm::rotate(glm::mat4(1.0f), 15.75f * (float)currentTime, glm::vec3(0.0, 1.0, 0.0));
	
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	//绘制太阳
	glDrawArrays(GL_TRIANGLES, 0, 36);
	//从栈中移除太阳的自转
	mvStack.pop();

	//------------------------------------ 小立方体 == 月球 -----------------------------------------------------
	mvStack.push(mvStack.top());
	mvStack.top() *=
		glm::translate(glm::mat4(1.0f), glm::vec3( 0.0,
			sin((float)currentTime) * 2.0, cos((float)currentTime) * 2.0));
	//月球旋转
	mvStack.top() *= glm::rotate(glm::mat4(1.0f), 15.75f * (float)currentTime, glm::vec3(0.0, 0.0, 1.0));
	//让月球小一些
	mvStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f));
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	//绘制太阳
	glDrawArrays(GL_TRIANGLES, 0, 36);
	//从栈中移除太阳的自转
	mvStack.pop();


	//------------------------------------ 小立方体 == 线框 -----------------------------------------------------
	mvStack.push(mvStack.top());
	mvStack.top() *=
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0,
			sin((float)currentTime) * 2.0, cos((float)currentTime) * 2.0));
	//月球旋转
	mvStack.top() *= glm::rotate(glm::mat4(1.0f), 15.75f * (float)currentTime, glm::vec3(0.0, 1.0, 1.0));
	//让月球小一些
	mvStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(0.35f, 0.35f, 0.35f));
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	//绘制太阳
	glDrawArrays(GL_LINE_LOOP, 0, 36);
	//从栈中移除太阳的自转
	mvStack.pop();
}

int main10() {
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	//设置窗口的选项
	//主版本号
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//副版本号
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//后两个参数分别是设置 允许全屏 和 资源共享
	GLFWwindow* window = glfwCreateWindow(1000, 600, "Chapter4 - program6", NULL, NULL);
	//将窗口与上下文关联起来
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	//用来开启垂直同步
	glfwSwapInterval(1);
	glfwSetWindowSizeCallback(window, window_reshape_callback);
	init3D_Shape5(window);
	//init(window,1);
	//渲染循环
	while (!glfwWindowShouldClose(window)) {
		//glfwGetTime()返回GLFW初始化之后经过的时间
		display3D_Shape5(window, glfwGetTime());

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