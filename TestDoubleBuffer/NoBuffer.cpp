/*˫������һ�����ڼ�����Ļ��˸
���ִ�������Ϸ������ʹ�õļ���
*/

#include "Common.h"

int main2() {
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
		CleanScreen();

		//�޻��壬ÿһ����Ⱦ�������Ƶ���ʾ��
		RenderBall(&b1);
		//Sleep(100);
		//�޻��壬ÿһ����Ⱦ�������Ƶ���ʾ��
		RenderBall(&b2);
		//Sleep(100);

		gotoxy(0, 0);
		printf("%d", frame);
		//Sleep(100);

		frame++;
	}

	return 0;
}