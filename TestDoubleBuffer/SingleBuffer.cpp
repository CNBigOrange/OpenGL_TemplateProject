/*˫������һ�����ڼ�����Ļ��˸
���ִ�������Ϸ������ʹ�õļ���
*/
#include "Common.h"

int main1() {
	Viewport buffer = { 0 };

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
		CleanScreen(&buffer);
		//�����壬�Ƚ�ͼ�λ��Ƶ�������
		RenderBall(&buffer ,&b1);
		//Sleep(100);
		//�����壬�Ƚ�ͼ�λ��Ƶ�������
		RenderBall(&buffer ,&b2);
		//Sleep(100);

		gotoxy(0, 0);
		printf("%d", frame);
		//Sleep(100);

		//���������Ϻ󣬽�������Ƶ���Ļ
		ViewportToScreen(&buffer);

		frame++;
	}

	return 0;
}