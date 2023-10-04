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

	ImportedModel myModel("models/shuttle.obj");//飞船
	//ImportedModel myModel("models/dolphinHighPoly.obj");//海豚
}

using namespace LoadModel;

void loadVertices(void) {
	std::vector<glm::vec3> vert = myModel.getVertices();
	std::vector<glm::vec2> tex = myModel.getTextureCoords();
	std::vector<glm::vec3>norm = myModel.getNormals();
	int numObjVertices = myModel.getNumVertices();

	std::vector<float> pvalues;//顶点位置
	std::vector<float> tvalues;//纹理坐标
	std::vector<float> nvalues;//法向量

	//由索引去找对应顶点坐标、纹理坐标、法向量
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

	//创建VAO并返回他们的整数型ID，存进数组vao，
	//参数表示要创建多少个ID，参数二表示用来保存返回的ID的数组
	glGenVertexArrays(1, vao2);
	//将制定的VAO标记为活跃，这样生成的缓冲区就会和这个VAO相关联
	glBindVertexArray(vao2[0]);

	//创建VBO并返回他们的整数型ID，存进数组vao，
	glGenBuffers(3, vbo2);
	//将制定的VBO标记为活跃，将顶点放入缓冲区0
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]);
	//将包含四棱锥顶点数据的数组pyramidPositions复制进活跃缓冲区（这里是第1个vbo）
	//&pvalues[0]取容器第一个元素的地址
	glBufferData(GL_ARRAY_BUFFER, pvalues.size()*4, &pvalues[0], GL_STATIC_DRAW);
	//将纹理坐标放入缓冲区1
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[1]);
	glBufferData(GL_ARRAY_BUFFER, tvalues.size()*4, &tvalues[0], GL_STATIC_DRAW);
	//将法向量放入缓冲区2
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[2]);
	glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);
}

void init3D_Model(GLFWwindow* window) {
	renderingProgram2 = createShaderProgram(SHOW_TEXTURE);
	//相机位置
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 0.0f;

	//构建透视矩阵
	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	//参数1：视场（纵向角度）Field Of View,FOV；参数2：纵横比aspect radio；参数3：近裁平面距离；参数4：远裁平面距离
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

	//获取MV矩阵和投影矩阵的统一变量
	mvLoc = glGetUniformLocation(renderingProgram2, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram2, "proj_matrix");

	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	//将视图矩阵压入栈
	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
	//绘制立方体（使用0号缓冲区）
	rMat = glm::rotate(glm::mat4(1.0f), 5.75f * (float)currentTime, glm::vec3(1.0f,1.0f, 0.0f));
	//rMat = glm::rotate(glm::mat4(1.0f), 30.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	mMat = glm::translate(glm::mat4(1.0f), 
		glm::vec3(cubeLocX, cubeLocY, -200 + cubeLocZ * (sin(currentTime) + 1) / 2));
	mMat *= glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 3.0f, 3.0f));
	mMat = mMat * rMat;
	//计算mv矩阵
	mvMat = vMat * mMat;

	//将透视矩阵和MV矩阵复制给相应的统一变量
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));

//---------------------------------------- 四棱锥 == 太阳 -----------------------------------------------------
	glBindBuffer(GL_ARRAY_BUFFER, vbo2[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glActiveTexture(GL_TEXTURE0);//激活第0个纹理单元
	glBindTexture(GL_TEXTURE_2D, brickTexture);//绑定纹理贴图

	glEnable(GL_CULL_FACE);//背面剔除
	glFrontFace(GL_CCW);//CCW显示指定顶点顺序逆时针为正面（默认为CCW），CW指定顺时针为正面
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, myModel.getNumVertices());
}

int main14() {
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
	init3D_Model(window);
	//init(window,1);
	//渲染循环
	while (!glfwWindowShouldClose(window)) {
		//glfwGetTime()返回GLFW初始化之后经过的时间
		display3D_Model(window, glfwGetTime());

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