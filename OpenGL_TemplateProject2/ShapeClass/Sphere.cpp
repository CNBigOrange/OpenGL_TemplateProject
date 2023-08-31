#include "../ToolsLibrary.h"
#include "Sphere.h"

Sphere::Sphere() {
	init(48);
}

Sphere::Sphere(int prec) {
	init(prec);
}

void Sphere::init(int prec) {
	//纹理贴图左右边缘在球体相交的位置需要同一个位置不同的顶点所以+1
	numVertices = (prec + 1) * (prec + 1);
	//索引数
	numIndices = prec * prec * 6;
	//std::vector::push_back()在向量的末尾增加一个新元素，并将向量长度加1
	for (int i = 0; i < numVertices; i++)vertices.push_back(glm::vec3());
	for (int i = 0; i < numVertices; i++)texCoords.push_back(glm::vec2());
	for (int i = 0; i < numVertices; i++)normals.push_back(glm::vec3());
	for (int i = 0; i < numVertices; i++)tangents.push_back(glm::vec3());
	for (int i = 0; i < numIndices; i++)indices.push_back(0);

	//计算三角形顶点
	for (int i = 0; i <= prec; i++) {
		for (int j = 0; j <= prec; j++) {
			//切片高度位置
			float y = (float)cos(toRadians(180.0f - i * 180.0f / prec));
			//(float)abs(cos(asin(y))) 切片高度位置对应的切片大小系数
			float x = -(float)cos(toRadians(j * 360.0f / prec)) * (float)abs(cos(asin(y)));
			float z = (float)sin(toRadians(j * 360.0f / prec)) * (float)abs(cos(asin(y)));
			vertices[i * (prec + 1) + j] = glm::vec3(x, y, z);//顶点坐标
			texCoords[i * (prec + 1) + j] = glm::vec2(((float)j / prec), ((float)i / prec));//纹理坐标
			normals[i * (prec + 1) + j] = glm::vec3(x, y, z);//顶点法线

			//计算切向量
			if (((x == 0) && (y == 1) && (z == 0)) || ((x == 0) && (y == -1) && (z == 0))) {
				tangents[i * (prec + 1) + j] = glm::vec3(0.0f, 0.0f, -1.0f);
			}
			else {
				tangents[i * (prec + 1) + j] = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z));
			}
		}
	}

	//计算三角形索引
	for (int i = 0; i < prec; i++) {
		for (int j = 0; j < prec; j++) {
			indices[6 * (i * prec + j) + 0] = i * (prec + 1) + j;
			indices[6 * (i * prec + j) + 1] = i * (prec + 1) + j + 1;
			indices[6 * (i * prec + j) + 2] = (i + 1) * (prec + 1) + j;
			indices[6 * (i * prec + j) + 3] = i * (prec + 1) + j + 1;
			indices[6 * (i * prec + j) + 4] = (i + 1) * (prec + 1) + j + 1;
			indices[6 * (i * prec + j) + 5] = (i + 1) * (prec + 1) + j;
		}
	}
}
