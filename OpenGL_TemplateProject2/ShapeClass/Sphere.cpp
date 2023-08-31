#include "../ToolsLibrary.h"
#include "Sphere.h"

Sphere::Sphere() {
	init(48);
}

Sphere::Sphere(int prec) {
	init(prec);
}

void Sphere::init(int prec) {
	//������ͼ���ұ�Ե�������ཻ��λ����Ҫͬһ��λ�ò�ͬ�Ķ�������+1
	numVertices = (prec + 1) * (prec + 1);
	//������
	numIndices = prec * prec * 6;
	//std::vector::push_back()��������ĩβ����һ����Ԫ�أ������������ȼ�1
	for (int i = 0; i < numVertices; i++)vertices.push_back(glm::vec3());
	for (int i = 0; i < numVertices; i++)texCoords.push_back(glm::vec2());
	for (int i = 0; i < numVertices; i++)normals.push_back(glm::vec3());
	for (int i = 0; i < numVertices; i++)tangents.push_back(glm::vec3());
	for (int i = 0; i < numIndices; i++)indices.push_back(0);

	//���������ζ���
	for (int i = 0; i <= prec; i++) {
		for (int j = 0; j <= prec; j++) {
			//��Ƭ�߶�λ��
			float y = (float)cos(toRadians(180.0f - i * 180.0f / prec));
			//(float)abs(cos(asin(y))) ��Ƭ�߶�λ�ö�Ӧ����Ƭ��Сϵ��
			float x = -(float)cos(toRadians(j * 360.0f / prec)) * (float)abs(cos(asin(y)));
			float z = (float)sin(toRadians(j * 360.0f / prec)) * (float)abs(cos(asin(y)));
			vertices[i * (prec + 1) + j] = glm::vec3(x, y, z);//��������
			texCoords[i * (prec + 1) + j] = glm::vec2(((float)j / prec), ((float)i / prec));//��������
			normals[i * (prec + 1) + j] = glm::vec3(x, y, z);//���㷨��

			//����������
			if (((x == 0) && (y == 1) && (z == 0)) || ((x == 0) && (y == -1) && (z == 0))) {
				tangents[i * (prec + 1) + j] = glm::vec3(0.0f, 0.0f, -1.0f);
			}
			else {
				tangents[i * (prec + 1) + j] = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z));
			}
		}
	}

	//��������������
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
