/*˫������һ�����ڼ�����Ļ��˸
���ִ�������Ϸ������ʹ�õļ���
*/
#include "Common.h"
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

//int main() {
//	fstream file("test.txt", ios::in);
//	string content;
//	string line = "";
//	while (!file.eof())
//	{
//		getline(file, line);
//		content.append(line + "\n");
//		cout << line << endl;
//	}
//	getline(file, line);
//	file.close();
//	return 0;
//}


int main() {
	Viewport buffer[2] = {0};
	int front_index{ 0 }, back_index{ 1 };
	

	Ball b1, b2;

	b1.m_x = 10;
	b1.m_y = 10;
	b1.m_char = 'b';//�ַ�������''���ַ�����char *��""

	b2.m_x = 20;
	b2.m_y = 10;
	b2.m_char = 'B';

	int frame = 0;

	//game loop
	for (;;)
	{
		CleanScreen(&buffer[back_index]);
		//�����壬�Ƚ�ͼ�λ��Ƶ�������
		RenderBall(&buffer[back_index], &b1);
		//Sleep(100);
		//�����壬�Ƚ�ͼ�λ��Ƶ�������
		RenderBall(&buffer[back_index], &b2);
		//Sleep(100);

		gotoxy(0, 0);
		printf("%d", frame);
		//Sleep(100);

		//���������Ϻ󣬽�������Ƶ���Ļ
		ViewportToScreen(&buffer[back_index],&buffer[front_index]);

		//����buffer
		{
			int temp = back_index;
			back_index = front_index;
			front_index = temp;
		}

		frame++;
	}

	return 0;
}