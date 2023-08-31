////////////////////////////////////////////////////
//ImportedModel 和 ModelImporter 头文件

#pragma once
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class ImportedModel
{
public:
	ImportedModel(const char* filePath);
	int getNumVertices();
	std::vector<glm::vec3> getVertices();
	std::vector<glm::vec2> getTextureCoords();
	std::vector<glm::vec3> getNormals();
private:
	int numVertices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec3> normalVecs;

};

class ModelImporter
{
public:
	ModelImporter();
	void parseOBJ(const char* filePath);
	int getNumVertices();
	std::vector<float> getVertices();
	std::vector<float> getTextureCoordinates();
	std::vector<float> getNormals();
private:
	//从OBJ文件读取的数值
	std::vector<float> vertVals;
	std::vector<float> stVals;
	std::vector<float> normVals;

	//保存为顶点属性以供后续使用的数值
	std::vector<float> triangleVerts;
	std::vector<float> textureCoords;
	std::vector<float> normals;
};