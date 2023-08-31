/*双缓冲是一种用于减少屏幕闪烁
被现代各种游戏引擎所使用的技术
*/
#include "Common.h"

int main1() {
	Viewport buffer = { 0 };

	Ball b1, b2;

	b1.m_x = 10;
	b1.m_y = 10;
	b1.m_char = 'b';//字符类型用''，字符串和char *用""

	b2.m_x = 20;
	b2.m_y = 10;
	b2.m_char = 'B';

	int frame = 0;

	//game loop
	for (;;)
	{
		CleanScreen(&buffer);
		//单缓冲，先将图形绘制到缓冲区
		RenderBall(&buffer ,&b1);
		//Sleep(100);
		//单缓冲，先将图形绘制到缓冲区
		RenderBall(&buffer ,&b2);
		//Sleep(100);

		gotoxy(0, 0);
		printf("%d", frame);
		//Sleep(100);

		//缓冲绘制完毕后，将缓冲绘制到屏幕
		ViewportToScreen(&buffer);

		frame++;
	}

	return 0;
}