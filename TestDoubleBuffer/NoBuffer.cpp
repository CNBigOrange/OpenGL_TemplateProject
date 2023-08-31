/*双缓冲是一种用于减少屏幕闪烁
被现代各种游戏引擎所使用的技术
*/

#include "Common.h"

int main2() {
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
		CleanScreen();

		//无缓冲，每一次渲染都将绘制到显示屏
		RenderBall(&b1);
		//Sleep(100);
		//无缓冲，每一次渲染都将绘制到显示屏
		RenderBall(&b2);
		//Sleep(100);

		gotoxy(0, 0);
		printf("%d", frame);
		//Sleep(100);

		frame++;
	}

	return 0;
}