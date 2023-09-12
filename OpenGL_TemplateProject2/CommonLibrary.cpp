#include "CommonLibrary.h"

void display(GLFWwindow* window, double currentTime) {
	//��vfProgram����OpenGL���߽׶Σ���GPU�ϣ�
	//ע��glUseProgram()��û��������ɫ������ֻ�ǽ���ɫ�����ؽ�Ӳ��
	glClearColor(1.0, 1.0, sin(currentTime), 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(renderingProgram);
	//���õ��С����δ������Ĭ��Ϊһ�����ص��С
	glPointSize(300.0f);
	//glDrawArray�����������ߴ������
	//����һ����ʾͼԪ���ͣ�����������ʾ���ĸ����㿪ʼ�滭�����������ܹ�Ҫ���ƵĶ�����
	glDrawArrays(GL_POINTS, 0, 1);
}

//��������X��λ��
float x = 0.0f;
//�ƶ������ε�ƫ����
float inc = 0.01f;

void display3Points(GLFWwindow* window, double currentTime) {
	glClear(GL_DEPTH_BUFFER_BIT);//ÿ�ν���Ȼ��������

	glClearColor(0.0, (x + 1) / 2, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);//ÿ�ν���ɫ���������

	x += inc;
	//��x���ƶ�������
	if (x > 1.0f)inc = -0.01f;
	//�л����������������ƶ�
	if (x < -1.0f)inc = 0.01f;
	//��ȡָ��offset������ָ��
	GLuint offsetLoc = glGetUniformLocation(renderingProgram, "offset");
	//��x��ֵ���Ƹ�offset
	glProgramUniform1f(renderingProgram, offsetLoc, x);

	//��vfProgram����OpenGL���߽׶Σ���GPU�ϣ�
	//ע��glUseProgram()��û��������ɫ������ֻ�ǽ���ɫ�����ؽ�Ӳ��
	glUseProgram(renderingProgram);
	//���õ��С����δ������Ĭ��Ϊһ�����ص��С
	//glPointSize(300.0f);
	//glDrawArray�����������ߴ������
	//����һ����ʾͼԪ���ͣ�����������ʾ���ĸ����㿪ʼ�滭�����������ܹ�Ҫ���ƵĶ�����
	glDrawArrays(GL_TRIANGLES, 0, 3);
}


void init(GLFWwindow* window, int fun_id) {
	renderingProgram = createShaderProgram(fun_id);
	glGenVertexArrays(numVAOs, vao);
	glBindVertexArray(vao[0]);

}

void  printShaderLog(GLuint shader) {
	int len = 0;
	int chWrittn = 0;
	char* log;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		log = (char*)malloc(len);
		glGetShaderInfoLog(shader, len, &chWrittn, log);
		cout << "Shader Info Log:" << log << endl;
		free(log);
		log = nullptr;
	}
}

void printProgramLog(int prog) {
	int len = 0;
	int chWrittn = 0;
	char* log;
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		log = (char*)malloc(len);
		glGetProgramInfoLog(prog, len, &chWrittn, log);
		cout << "Program Info Log:" << log << endl;
		free(log);
		log = nullptr;
	}
}

bool checkOpenGLError() {
	bool foundError = false;
	int glErr = glGetError();
	while (glErr != GL_NO_ERROR) {
		cout << "glError:" << glErr << endl;
		foundError = true;
		glErr = glGetError();
	}
	return foundError;
}

bool loadShader(GLuint vShader, GLuint fShader, const char* mvertShaderStr, const char* mfragShaderStr)
{
	//��ȡ�ı���ɫ������
	string vertShaderStr = readShaderSource(mvertShaderStr);
	string fragShaderStr = readShaderSource(mfragShaderStr);
	//��������stringת��Ϊchar*
	const char* vertShaderSrc = vertShaderStr.c_str();
	const char* fragShaderSrc = fragShaderStr.c_str();

	glShaderSource(vShader, 1, &vertShaderSrc, NULL);
	glShaderSource(fShader, 1, &fragShaderSrc, NULL);

	return true;
}

bool loadShader(GLuint vShader, GLuint fShader, GLuint tcs, GLuint tes, const char* mvertShaderStr, const char* mfragShaderStr, const char* tCS, const char* tES)
{
	//��ȡ�ı���ɫ������
	string vertShaderStr = readShaderSource(mvertShaderStr);
	string fragShaderStr = readShaderSource(mfragShaderStr);
	string tcsShaderStr = readShaderSource(tCS);
	string tesShaderStr = readShaderSource(tES);

	//��������stringת��Ϊchar*
	const char* vertShaderSrc = vertShaderStr.c_str();
	const char* fragShaderSrc = fragShaderStr.c_str();
	const char* tcsShaderSrc = tcsShaderStr.c_str();
	const char* tesShaderSrc = tesShaderStr.c_str();

	glShaderSource(vShader, 1, &vertShaderSrc, NULL);
	glShaderSource(fShader, 1, &fragShaderSrc, NULL);
	glShaderSource(tcs, 1, &tcsShaderSrc, NULL);
	glShaderSource(tes, 1, &tesShaderSrc, NULL);

	return true;
}

bool loadShader(GLuint vShader, GLuint fShader, GLuint gShader, GLuint tcs, GLuint tes, const char* mvertShaderStr, const char* mfragShaderStr, const char* mgemoShader,const char* tCS, const char* tES)
{
	//��ȡ�ı���ɫ������
	string vertShaderStr = readShaderSource(mvertShaderStr);
	string tcsShaderStr = readShaderSource(tCS);
	string tesShaderStr = readShaderSource(tES);
	string gemoShaderStr = readShaderSource(mgemoShader);
	string fragShaderStr = readShaderSource(mfragShaderStr);

	//��������stringת��Ϊchar*
	const char* vertShaderSrc = vertShaderStr.c_str();
	const char* fragShaderSrc = fragShaderStr.c_str();
	const char* gemoShaderSrc = gemoShaderStr.c_str();
	const char* tcsShaderSrc = tcsShaderStr.c_str();
	const char* tesShaderSrc = tesShaderStr.c_str();

	glShaderSource(vShader, 1, &vertShaderSrc, NULL);
	glShaderSource(tcs, 1, &tcsShaderSrc, NULL);
	glShaderSource(tes, 1, &tesShaderSrc, NULL);
	glShaderSource(gShader, 1, &gemoShaderSrc, NULL);
	glShaderSource(fShader, 1, &fragShaderSrc, NULL);

	return true;
}

bool loadShader(GLuint vShader, GLuint fShader, GLuint gShader,  const char* mvertShaderStr, const char* mfragShaderStr, const char* mgemoShader)
{
	//��ȡ�ı���ɫ������
	string vertShaderStr = readShaderSource(mvertShaderStr);
	string gemoShaderStr = readShaderSource(mgemoShader);
	string fragShaderStr = readShaderSource(mfragShaderStr);

	//��������stringת��Ϊchar*
	const char* vertShaderSrc = vertShaderStr.c_str();
	const char* fragShaderSrc = fragShaderStr.c_str();
	const char* gemoShaderSrc = gemoShaderStr.c_str();

	glShaderSource(vShader, 1, &vertShaderSrc, NULL);
	glShaderSource(gShader, 1, &gemoShaderSrc, NULL);
	glShaderSource(fShader, 1, &fragShaderSrc, NULL);

	return true;
}

//���OpenGL�����ʾ��
GLuint createShaderProgram(int fun_id,int enable_shader ) {
	GLint vertCompiled;
	GLint fragCompiled;
	GLint linked;
	
	//OpenGL����ÿ����ɫ�����󣨳�ʼֵΪ�գ���ʱ�򣬻᷵��һ������ID��Ϊ���������������
	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint gShader = glCreateShader(GL_GEOMETRY_SHADER);
	
	GLuint tcShader = glCreateShader(GL_TESS_CONTROL_SHADER);
	GLuint teShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
	
	//glShaderSource��GLSL������ַ����������ɫ��������
	//����һ�����������ɫ������ɫ������;����������ɫ��Դ�������ַ���������
	//������������Դ������ַ���ָ�룻�����ģ�

	switch (fun_id) {
	case DEBUG_MODE: {
		//��������ɫ������Ϊ�ַ�������
		const char* vshaderSource =
			//ָ��OpenGL�汾
			"#version 430 \n"
			"void main(void) \n"
			//���ö�����3D�ռ��е�����
			"{ gl_Position vec4(0.0,0.0,0.0,1.0);}";
		//�ڶ�����ɫ����Ƭ����ɫ��֮�仹�й�դ����ɫ��ִ��
		//��Ƭ����ɫ������Ϊ�ַ�������
		const char* fshaderSource =
			"#version 430 \n"
			//out��ǩ����color���������
			"out vec4 color; \n"
			"void main(void) \n"
			//gl_FragCoord�����������ص������
			"{ color =vec4(gl_FragCoord.x/1000,gl_FragCoord.y/600,1.0,1.0);}";

		glShaderSource(vShader, 1, &vshaderSource, NULL);
		glShaderSource(fShader, 1, &fshaderSource, NULL);
		break; }

	case LOAD_SHADER_FROM_FILE: {
		//��ȡ�ı���ɫ������
		string vertShaderStr = readShaderSource("vertShader.glsl");
		string fragShaderStr = readShaderSource("fragShader.glsl");
		//��������stringת��Ϊchar*
		const char* vertShaderSrc = vertShaderStr.c_str();
		const char* fragShaderSrc = fragShaderStr.c_str();

		glShaderSource(vShader, 1, &vertShaderSrc, NULL);
		glShaderSource(fShader, 1, &fragShaderSrc, NULL);
		break; }
	case SHOW_ANIMATION: {
		//��ȡ�ı���ɫ������
		string vertShaderStr = readShaderSource("vertShaderAnimation.glsl");
		string fragShaderStr = readShaderSource("fragShaderAnimation.glsl");
		//��������stringת��Ϊchar*
		const char* vertShaderSrc = vertShaderStr.c_str();
		const char* fragShaderSrc = fragShaderStr.c_str();

		glShaderSource(vShader, 1, &vertShaderSrc, NULL);
		glShaderSource(fShader, 1, &fragShaderSrc, NULL);
		break;}
	case SHOW_3D_CUBE: {
		//��ȡ�ı���ɫ������
		string vertShaderStr = readShaderSource("vertShaderCube.glsl");
		string fragShaderStr = readShaderSource("fragShaderCube.glsl");
		//��������stringת��Ϊchar*
		const char* vertShaderSrc = vertShaderStr.c_str();
		const char* fragShaderSrc = fragShaderStr.c_str();

		glShaderSource(vShader, 1, &vertShaderSrc, NULL);
		glShaderSource(fShader, 1, &fragShaderSrc, NULL);
		break; }
	case SHOW_3D_CUBE_INSTANCE: {
		//��ȡ�ı���ɫ������
		string vertShaderStr = readShaderSource("vertShaderCubeInstance.glsl");
		string fragShaderStr = readShaderSource("fragShaderCube.glsl");
		//��������stringת��Ϊchar*
		const char* vertShaderSrc = vertShaderStr.c_str();
		const char* fragShaderSrc = fragShaderStr.c_str();

		glShaderSource(vShader, 1, &vertShaderSrc, NULL);
		glShaderSource(fShader, 1, &fragShaderSrc, NULL);
		break; }
	case SHOW_TEXTURE: {
		loadShader(vShader, fShader, "vertShaderTexture.glsl", "fragShaderTexture.glsl");
		break; }
	case ADS_TEXTURE: {
		loadShader(vShader, fShader, "BlinnPhongShaders/vertShader2.glsl", "BlinnPhongShaders/fragShader2.glsl");
		break; }
	case SHADOWPASS1: {
		loadShader(vShader, fShader, "./vertShadowShader.glsl", "./fragShadowShader.glsl");
		break; }
	case SHADOWPASS2: {
		loadShader(vShader, fShader, "./vertShadowShader2.glsl", "./fragShadowShader2.glsl");
		break; }
	case SHADOWPASS3: {
		loadShader(vShader, fShader, "./vertShadowShader2.glsl", "./fragShadowShaderPCF.glsl");
		break; }
	case SKY_CUBE: {
		loadShader(vShader, fShader, "./vertShaderSkyCube.glsl", "./fragShaderSkyCube.glsl");
		break; }
	case PROC_BUMP: {
		loadShader(vShader, fShader, "./vertShaderProcBump.glsl", "./fragShaderProcBump.glsl");
		break; }
	case NORMAL_MAPPING: {
		loadShader(vShader, fShader, "vertShaderNormal.glsl", "fragShaderNormal.glsl");
		break; }
	case NORMAL_MAPPING_MOON: {
		loadShader(vShader, fShader, "vertShaderNormalMoon.glsl", "fragShaderNormalMoon.glsl");
		break; }
	case HEIGHT_MAPPING: {
		loadShader(vShader, fShader, "vertShaderHeightMapping.glsl", "fragShaderHeightMapping.glsl");
		break; }
	case TESSELLATION_GRID_ONLY: {
		loadShader(vShader, fShader, tcShader, teShader,
			"vertShaderTess.glsl", "fragShaderTess.glsl","tessCShader.glsl","tessEShader.glsl");
		break; }
	case TESSELLATION_BEZIER: {
		loadShader(vShader, fShader, tcShader, teShader,
			"vertShaderTessBezier.glsl", "fragShaderTessBezier.glsl", "tessBezierCShader.glsl", "tessBezierEShader.glsl");
		break; }
	case TESSELLATION_HEIGHT_MAPPED: {
		loadShader(vShader, fShader, tcShader, teShader,
			"vertShaderTessHeightMapping.glsl", "fragShaderTessHeightMapping.glsl", "tessHeightMappingCShader.glsl", "tessHeightMappingEShader.glsl");
		break; }	
	case TESSELLATION_HEIGHT_MAPPED_LIGHT: {
		loadShader(vShader, fShader, tcShader, teShader,
			"vertShaderTessHeightMappingLight.glsl", "fragShaderTessHeightMappingLight.glsl", "tessHeightMappingCShaderLight.glsl", "tessHeightMappingEShaderLight.glsl");
		break; }
	case TESSELLATION_HEIGHT_MAPPED_LOD: {
		loadShader(vShader, fShader, tcShader, teShader,
			"vertShaderTessHeightMappingLight.glsl", "fragShaderTessHeightMappingLight.glsl", "tessHeightMappingCShaderLOD.glsl", "tessHeightMappingEShaderLight.glsl");
		break; }
	case GEOM_MOD_INFLATE: {
		loadShader(vShader, fShader, gShader, tcShader, teShader,
			"vertShaderGeomModInflate.glsl", "fragShaderGeomModInflate.glsl", "geomShaderModInflate.glsl", "tessCShaderNULL.glsl", "tessEShaderNULL.glsl");
		break; }
	case GEOM_MOD_INFLATE_ONLY: {
		loadShader(vShader, fShader, gShader, 
			"vertShaderGeomModInflate.glsl", "fragShaderGeomModInflate.glsl", "geomShaderModInflate.glsl");
		break; }
	case GEOM_MOD_INFLATE_ADD: {
		loadShader(vShader, fShader, gShader,
			"vertShaderGeomModAdd.glsl", "fragShaderGeomModInflate.glsl", "geomShaderModAdd.glsl");
		break; }
	case GEOM_MOD_INFLATE_CHANGE: {
		loadShader(vShader, fShader, gShader,
			"vertShaderGeomModAdd.glsl", "fragShaderGeomModInflate.glsl", "geomShaderModChange.glsl");
		break; }
	case FOG: {
		loadShader(vShader, fShader, "vertShaderFog.glsl", "fragShaderFog.glsl");
		break; }
	case BLENDING: {
		loadShader(vShader, fShader, "vertShaderBlending.glsl", "fragShaderBlending.glsl");
		break; }
	default: {}
	}

	//glCompileShader()����������ɫ��
	glCompileShader(vShader);
	if (enable_shader == ENABLE_TESSELLATION) {
		glCompileShader(tcShader);
		glCompileShader(teShader);
	}
	if (enable_shader == ENABLE_GEOMETRY) {
		glCompileShader(tcShader);
		glCompileShader(teShader);
		glCompileShader(gShader);
	}
	if (enable_shader == ENABLE_GEOMETRY_ONLY) {
		glCompileShader(gShader);
	}

	//���������ɫ��ʱ�Ĵ���
	checkOpenGLError();
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
	if (vertCompiled != 1) {
		cout << "vertex compilation failed" << endl;
		printShaderLog(vShader);
	}

	glCompileShader(fShader);
	//���������ɫ��ʱ�Ĵ���
	checkOpenGLError();
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &fragCompiled);
	if (fragCompiled != 1) {
		cout << "fragment compilation failed" << endl;
		printShaderLog(fShader);
	}

	//����vfProgram���󣬲�����ָ����������ID
	GLuint vfProgram = glCreateProgram();
	//����ɫ������������
	glAttachShader(vfProgram, vShader);
	if (enable_shader == ENABLE_TESSELLATION) {
		glAttachShader(vfProgram, tcShader);
		glAttachShader(vfProgram, teShader);
	}

	if (enable_shader == ENABLE_GEOMETRY) {
		glAttachShader(vfProgram, tcShader);
		glAttachShader(vfProgram, teShader);
		glAttachShader(vfProgram, gShader);
	}

	if (enable_shader == ENABLE_GEOMETRY_ONLY) {
		glAttachShader(vfProgram, gShader);
	}

	glAttachShader(vfProgram, fShader);

	//����GLSL����������ȷ����ɫ���ͳ���ļ�����
	glLinkProgram(vfProgram);
	//����������ɫ��ʱ�Ĵ���
	checkOpenGLError();
	glGetProgramiv(vfProgram, GL_LINK_STATUS, &linked);
	if (linked != 1) {
		cout << "linking failed" << endl;
		printProgramLog(vfProgram);
	}
	return vfProgram;
}

GLuint createShaderProgram() {
	//��������ɫ������Ϊ�ַ�������
	const char* vshaderSource =
		//ָ��OpenGL�汾
		"#version 430 \n"
		"void main(void) \n"
		//���ö�����3D�ռ��е�����
		"{ gl_Position = vec4(0.0,0.0,0.0,1.0);}";
	//�ڶ�����ɫ����Ƭ����ɫ��֮�仹�й�դ����ɫ��ִ��
	//��Ƭ����ɫ������Ϊ�ַ�������
	const char* fshaderSource =
		"#version 430 \n"
		//out��ǩ����color���������
		"out vec4 color; \n"
		"void main(void) \n"
		//gl_FragCoord�����������ص������
		"{ color =vec4(gl_FragCoord.x/1000,gl_FragCoord.y/600,1.0,1.0);}";
	//OpenGL����ÿ����ɫ�����󣨳�ʼֵΪ�գ���ʱ�򣬻᷵��һ������ID��Ϊ���������������
	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
	//glShaderSource��GLSL������ַ����������ɫ��������
	//����һ�����������ɫ������ɫ������;����������ɫ��Դ�������ַ���������
	//������������Դ������ַ���ָ�룻�����ģ�

	glShaderSource(vShader, 1, &vshaderSource, NULL);
	glShaderSource(fShader, 1, &fshaderSource, NULL);
	//glCompileShader()����������ɫ��
	glCompileShader(vShader);
	glCompileShader(fShader);
	//����vfProgram���󣬲�����ָ����������ID
	GLuint vfProgram = glCreateProgram();
	//����ɫ������������
	glAttachShader(vfProgram, vShader);
	glAttachShader(vfProgram, fShader);
	//����GLSL����������ȷ����ɫ���ͳ���ļ�����
	glLinkProgram(vfProgram);

	return vfProgram;
}

string readShaderSource(const char* filePath)
{
	string content;
	//���ļ�
	ifstream fileStream(filePath, ios::in);
	string line = "";
	while (!fileStream.eof())
	{
		getline(fileStream, line);
		content.append(line + "\n");
		cout << line << endl;
	}
	fileStream.close();
	return content;
}

//����������ƽ�ƾ���
mat4 buildTranslate(float x, float y, float z)
{
	glm::mat4 trans = glm::mat4(1.0, 0.0, 0.0, 0.0,//ע�⣺���������У����ǵ�һ��
					0.0, 1.0, 0.0, 0.0,
					0.0, 0.0, 1.0, 0.0,
					x, y, z, 1.0);
	return trans;
}
//������������x�����ת����
mat4 buildRotateX(float rad)
{
	mat4 xrot = mat4(1.0, 0.0, 0.0, 0.0,//ע�⣺���������У����ǵ�һ��
		0.0, cos(rad), -sin(rad), 0.0,
		0.0, sin(rad), cos(rad), 0.0,
		0.0, 0.0, 0.0, 1.0);
	return xrot;
}
//����������y�����ת��֤
mat4 buildRotateY(float rad)
{
	mat4 yrot = mat4(cos(rad), 0.0, sin(rad), 0.0,
					0.0, 1.0, 0.0, 0.0,
					-sin(rad), 0.0, cos(rad), 0.0,
					0.0, 0.0, 0.0, 1.0);
	return yrot;
}
//������������z�����ת����
mat4 buildRotateZ(float rad)
{
	mat4 zrot = mat4(cos(rad), -sin(rad), 0.0, 0.0,
				sin(rad), cos(rad), 0.0, 0.0,
				0.0, 0.0, 1.0, 0.0,
				0.0, 0.0, 0.0, 1.0);
	return zrot;
}
//�������������ž���
mat4 buildScale(float x,float y,float z)
{
	mat4 scale = mat4(x, 0.0, 0.0, 0.0,
		0.0, y, 0.0, 0.0,
		0.0, 0.0, z, 0.0,
		0.0, 0.0, 0.0, 1.0);
	return scale;
}
//������ͼ
GLuint loadTexture(const char* texImagePath)
{
	GLuint textureID;
	textureID = SOIL_load_OGL_texture(texImagePath,
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	if (textureID == 0)
		cout << "could not find texture file" << texImagePath << endl;
	//���ɶ༶��Զ����mipmapping��,����αӰ��ע�͵����������Լ���αӰ�������ǻ���ʧͼ��ϸ��
	glBindTexture(GL_TEXTURE_2D, textureID);
	//GL_LINEAR_MIPMAP_LINEAR    �����Թ���
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	//�����ʹ�ø������Թ���
	if (glewIsSupported("GL_EXT_texture_filter_anisotropic")) {
		GLfloat anisoSetting = 0.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisoSetting);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisoSetting);
	}

	return textureID;
}


/***************************************���պͲ���**************************************/
//�ƽ����
float* goldAmbient() {
	static float a[4] = { 0.2473f,0.1995f,0.0745f,1.0f };
	return (float*)a;
}
float* goldDiffuse() {
	static float a[4] = { 0.7516f,0.6065f,0.2265f,1.0f };
	return (float*)a;
}
float* goldSpecular() {
	static float a[4] = { 0.6283f,0.5559f,0.3661f,1.0f };
	return (float*)a;
}
float goldShininess() {
	return 51.2f;
}

//��������
float* silverAmbient() {
	static float a[4] = { 0.1923f,0.1923f,0.1923f,1.0f };
	return (float*)a;
}
float* silverDiffuse() {
	static float a[4] = { 0.5075f,0.5075f,0.5075f,1.0f };
	return (float*)a;
}
float* silverSpecular() {
	static float a[4] = { 0.5083f,0.5083f,0.5083f,1.0f };
	return (float*)a;
}
float silverShininess() {
	return 51.2f;
}

//��ͭ����
float* bronzeAmbient() {
	static float a[4] = { 0.2125f,0.1275f,0.0540f,1.0f };
	return (float*)a;
}
float* bronzeDiffuse() {
	static float a[4] = { 0.7140f,0.4284f,0.1814f,1.0f };
	return (float*)a;
}
float* bronzeSpecular() {
	static float a[4] = { 0.3936f,0.2719f,0.1667f,1.0f };
	return (float*)a;
}
float bronzeShininess() {
	return 25.6f;
}