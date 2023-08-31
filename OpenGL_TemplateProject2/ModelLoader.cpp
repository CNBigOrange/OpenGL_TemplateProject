#include "ModelLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

//---------------------ImportedModel��
ImportedModel::ImportedModel(const char* filePath) {
	ModelImporter modelImporter = ModelImporter();
	modelImporter.parseOBJ(filePath);//ʹ��modelImporter��ȡ������Ϣ
	numVertices = modelImporter.getNumVertices();
	std::vector<float> verts = modelImporter.getVertices();
	std::vector<float> tcs = modelImporter.getTextureCoordinates();
	std::vector<float>normals = modelImporter.getNormals();

	for (int i = 0; i < numVertices; i++) {
		vertices.push_back(glm::vec3(verts[i * 3], verts[i * 3 + 1], verts[i * 3 + 2]));
		texCoords.push_back(glm::vec2(tcs[i * 2], tcs[i * 2 + 1]));
		normalVecs.push_back(glm::vec3(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]));
	}
}

int ImportedModel::getNumVertices() { return numVertices; }
std::vector<glm::vec3> ImportedModel::getVertices() { return vertices; }
std::vector<glm::vec2> ImportedModel::getTextureCoords() { return texCoords; }
std::vector<glm::vec3> ImportedModel::getNormals() { return normalVecs; }

//---------------------ModelImporter��
ModelImporter::ModelImporter(){}

void ModelImporter::parseOBJ(const char* filePath) {
	float x, y, z;
	string content;
	ifstream fileStream(filePath, ios::in);
	string line = "";
	while (!fileStream.eof()) {
		//getline()�ӡ���������ȡ����
		getline(fileStream, line);
		if (line.compare(0, 2, "v ") == 0) {//"v "OBJ�ļ��ж���λ�ñ�ǩ
			stringstream ss(line.erase(0, 2));//�Ƴ����±��0���ַ���ʼ����Ϊ2���ַ���
			ss >> x; ss >> y; ss >> z;//��ȡ����λ����ֵ stringstream���Կո�Ͷ�������Ԫ��
			vertVals.push_back(x);
			vertVals.push_back(y);
			vertVals.push_back(z);
		}
		//���������ǩ��vt
		if (line.compare(0, 2, "vt") == 0) {
			stringstream ss(line.erase(0, 2));
			ss >> x; ss >> y;
			stVals.push_back(x);
			stVals.push_back(y);
		}
		//���㷨������ǩ��vn
		if (line.compare(0, 2, "vn") == 0) {
			stringstream ss(line.erase(0, 2));
			ss >> x; ss >> y; ss >> z;
			normVals.push_back(x);
			normVals.push_back(y);
			normVals.push_back(z);
		}
		//���������ǩ��f
		if (line.compare(0, 2, "f ") == 0) {
			string oneCorner, v, t, n;
			stringstream ss(line.erase(0, 2));
			for (int i = 0; i < 3; i++) {
				//���в���һ��ss���ַ������в�������' ')֮ǰ�����ݵ���������oneCorner)��
				getline(ss, oneCorner, ' ');
				stringstream oneCornerSS(oneCorner);
				getline(oneCornerSS, v, '/');
				getline(oneCornerSS, t, '/');
				getline(oneCornerSS, n, '/');
				//stoi(v)���ַ���vת��Ϊ����
				int vertRef, tcRef, normRef;
				try {
					vertRef = (stoi(v) - 1) * 3;
					tcRef = (stoi(t) - 1) * 2;
					normRef = (stoi(n) - 1) * 3;
				}
				catch (const exception& exception)
				{
					cout << "error:" << exception.what() << endl;
				}
				//������������
				triangleVerts.push_back(vertVals[vertRef]);
				triangleVerts.push_back(vertVals[vertRef + 1]);
				triangleVerts.push_back(vertVals[vertRef + 2]);
				//����������������
				textureCoords.push_back(stVals[tcRef]);
				textureCoords.push_back(stVals[tcRef + 1]);
				//������������
				normals.push_back(normVals[normRef]);
				normals.push_back(normVals[normRef + 1]);
				normals.push_back(normVals[normRef + 2]);
			}
		}
	}
}

int ModelImporter::getNumVertices() { return (triangleVerts.size() / 3); }//��ȡ����
std::vector<float>ModelImporter::getVertices() { return triangleVerts; }
std::vector<float>ModelImporter::getTextureCoordinates() { return textureCoords; }
std::vector<float>ModelImporter::getNormals() { return normals; }