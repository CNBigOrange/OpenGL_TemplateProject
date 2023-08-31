#pragma once

#include <stdio.h>
#include <windows.h>

#define WIDTH (80)
#define HEIGHT (24)

typedef struct testball
{
	short m_x;
	short m_y;
	char m_char;
} Ball;

typedef struct buffer
{
	char m_buffer[WIDTH * HEIGHT];

}Viewport;

//��������õ���Ӧ����
static void gotoxy(short x, short y)
{
	COORD cd = { x,y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cd);
}

static void CleanScreen(Viewport* viewport)
{
	//*viewport->m_buffer = { 0 };
	memset(viewport, 0, sizeof(*viewport));//����0��viewport��sizeof(*viewport)���ֽ�
}

static void CleanScreen()
{
	system("cls");
}
//���Ƶ�����
static void RenderBall(Viewport* viewport,Ball* b)
{
	int index = b->m_x + b->m_y * WIDTH;
	viewport->m_buffer[index] = b->m_char;
}
//���Ƶ���Ļ
static void RenderBall(Ball* b)
{
	gotoxy(b->m_x, b->m_y);
	printf("%c", b->m_char);
}

static void ViewportToScreen(Viewport* viewport)
{
	for(int y=0;y<HEIGHT;y++)
		for (int x = 0; x < WIDTH; x++)
		{
			int index = x + y * WIDTH;
			char ch = viewport->m_buffer[index];
			if (ch)
			{
				gotoxy(x, y);
				printf("%c", ch);
			}
		}
}

static void ViewportToScreen(Viewport* back_buffer, Viewport* front_buffer)
{
	for (int y = 0; y < HEIGHT; y++)
		for (int x = 0; x < WIDTH; x++)
		{
			int index = x + y * WIDTH;
			char ch = back_buffer->m_buffer[index];
			//ֻ��Ⱦ���λ��岻ͬ���ֵ���Ļ
			if (ch!=front_buffer->m_buffer[index])
			{
				gotoxy(x, y);
				printf("%c", ch);
				front_buffer->m_buffer[index] = ch;
			}
		}
}