#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <graphics.h>
#include <stdio.h>
#include <conio.h>
#include <Windows.h>
#include <stdlib.h>
#include <time.h>
//包含Winmm.lib可以使用mciSendString函数播放mp3文件
#pragma comment(lib,"Winmm.lib")
#define W 1040
#define H 700
IMAGE img_bk;
IMAGE img_loading1, img_loading2;
IMAGE img_score;
IMAGE img_bell1, img_bell1_, img_bell2, img_bell2_, img_bell3, img_bell3_, img_bell4, img_bell4_;
IMAGE img_ld, img_lu, img_rd, img_ru, img_tl, img_tr;
IMAGE img_ld_, img_lu_, img_rd_, img_ru_, img_tl_, img_tr_;
MOUSEMSG m;
int score=0,bestscore=0;
int rabbit_x, rabbit_y=630;
int bell_x[4] = { 20,500,300,980 }, bell_y[4] = { 0,130,260,390 };
int direction1;//定义一个方向变量1：-1为向左，0为静止，1为向右。
int OldXPosition = 0;//定义一个旧鼠标横坐标值，用来存储上一轮循环m.x的值，可以通过与现在的m.x值比较，得到方向。
int OldYPosition = 0;//定义一个旧的小兔纵坐标值，用来存储上一轮循环rabbit_y的值，可以通过与现在的rabbit_y值比较，得到方向。
int direction2=1;//定义一个方向变量2，用来判断跳跃时是向上还是向下：-1为向下，1为向上。
int count1=0,count2=0;
int OlderXPosition=0;//再定义一个更旧的鼠标坐标值，用来存储上两轮循环m.x的值，可以使方向的判断更加严格。
int facing=1;//定义一个facing变量，决定静止时的面向：-1为向左，1为向右。
int state = 0;//定义一个state变量，决定游戏是否开始：0为未开始，1为开始。
int ring=1,slowdown=0;//定义一个ring变量，共有值1，2，3，4，5，6，7，8，决定一个铃铛的状态。slowndown帮助减慢铃铛摇动速度。
int i, n;//遍历用的变量。
int slowdown1 = 0;
int g = 25;//实现模拟重力加速度效果。
int upordown=1;//定义一个upordown变量，判断小兔是向上还是向下：-1为向下，1为向上。
int firsttimeornot = 1;//判断是不是第一次，如果是为1，否则为0.
int j, k=0,f;//用于遍历查找符合条件的铃铛。
int newground = 630;//假象新地面，高度就是跳到的那个铃铛的高度。
int height = -632;//实现场景的改变。
MOUSEMSG m2;

void readRecordFile()//读取历史最佳成绩。
{
	FILE* fp;
	fp = fopen(".\\gameRecord.dat", "r");
	fscanf(fp, "%d", &bestscore);
	fclose(fp);
}
void writeRecordFile()//如果创造纪录，写入新成绩。
{
	FILE* fp;
	fp = fopen(".\\gameRecord.dat", "w");
	fprintf(fp, "%d", bestscore);
	fclose(fp);
}
void gameover()//直接借用FlappyBird2的框架。
{
	EndBatchDraw();
	_getch();
	closegraph();
}
void startup()
{
	initgraph(W, H);

	loadimage(&img_loading1, "..\\res\\logo.jpg");
	loadimage(&img_loading2, "..\\res\\instruction.jpg");
	loadimage(&img_bk, "..\\res\\background.jpg");
	loadimage(&img_score, "..\\res\\score.jpg");
	loadimage(&img_bell1, "..\\res\\bells\\bell1.jpg");
	loadimage(&img_bell2, "..\\res\\bells\\bell2.jpg");
	loadimage(&img_bell3, "..\\res\\bells\\bell3.jpg");
	loadimage(&img_bell4, "..\\res\\bells\\bell4.jpg");
	loadimage(&img_bell1_, "..\\res\\bells\\bell10.jpg");
	loadimage(&img_bell2_, "..\\res\\bells\\bell20.jpg");
	loadimage(&img_bell3_, "..\\res\\bells\\bell30.jpg");
	loadimage(&img_bell4_, "..\\res\\bells\\bell40.jpg");
	loadimage(&img_ld, "..\\res\\rabbits\\jumpld.jpg");
	loadimage(&img_lu, "..\\res\\rabbits\\jumplu.jpg");
	loadimage(&img_rd, "..\\res\\rabbits\\jumprd.jpg");
	loadimage(&img_ru, "..\\res\\rabbits\\jumpru.jpg");
	loadimage(&img_tl, "..\\res\\rabbits\\standtl.jpg");
	loadimage(&img_tr, "..\\res\\rabbits\\standtr.jpg");
	loadimage(&img_ld_, "..\\res\\rabbits\\jumpld0.jpg");
	loadimage(&img_lu_, "..\\res\\rabbits\\jumplu0.jpg");
	loadimage(&img_rd_, "..\\res\\rabbits\\jumprd0.jpg");
	loadimage(&img_ru_, "..\\res\\rabbits\\jumpru0.jpg");
	loadimage(&img_tl_, "..\\res\\rabbits\\standtl0.jpg");
	loadimage(&img_tr_, "..\\res\\rabbits\\standtr0.jpg");
	
	
	//循环播放背景音乐。
	mciSendString("open ..\\res\\bgm.mp3 alias bkmusic", NULL, 0, NULL);
	mciSendString("play bkmusic repeat", NULL, 0, NULL);
	
	BeginBatchDraw();
	
	//显示loading image 1，鼠标左键单击进入下一页。
	while(1)
	{
		putimage(0, 0, &img_loading1);
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN)
			break;
		FlushBatchDraw();
	}
	//显示loading image 2，鼠标左键单击进入正式游戏界面。
	while(1)
	{
		putimage(0, 0, &img_loading2);
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN)
			break;
		FlushBatchDraw();
	}
}
void show()
{
	putimage(0, height, &img_bk);
	
	//显示下落的铃铛，要求铃铛可以摇动，屏幕上一次最多显示4个铃铛。
	{
	if (ring == 1 || ring == 8)
	{
		putimage(bell_x[0], bell_y[0], &img_bell1_, NOTSRCERASE);
		putimage(bell_x[0], bell_y[0], &img_bell1, SRCINVERT);
	}
	else if (ring == 2 || ring == 7)
	{
		putimage(bell_x[0], bell_y[0], &img_bell2_, NOTSRCERASE);
		putimage(bell_x[0], bell_y[0], &img_bell2, SRCINVERT);
	}
	else if (ring == 3 || ring == 6)
	{
		putimage(bell_x[0], bell_y[0], &img_bell3_, NOTSRCERASE);
		putimage(bell_x[0], bell_y[0], &img_bell3, SRCINVERT);
	}
	else if (ring == 4 || ring == 5)
	{
		putimage(bell_x[0], bell_y[0], &img_bell4_, NOTSRCERASE);
		putimage(bell_x[0], bell_y[0], &img_bell4, SRCINVERT);
	}
	if (ring == 4 || ring == 5)
	{
		putimage(bell_x[1], bell_y[1], &img_bell1_, NOTSRCERASE);
		putimage(bell_x[1], bell_y[1], &img_bell1, SRCINVERT);
	}
	else if (ring == 3 || ring == 6)
	{
		putimage(bell_x[1], bell_y[1], &img_bell2_, NOTSRCERASE);
		putimage(bell_x[1], bell_y[1], &img_bell2, SRCINVERT);
	}
	else if (ring == 2 || ring == 7)
	{
		putimage(bell_x[1], bell_y[1], &img_bell3_, NOTSRCERASE);
		putimage(bell_x[1], bell_y[1], &img_bell3, SRCINVERT);
	}
	else if (ring == 1 || ring == 8)
	{
		putimage(bell_x[1], bell_y[1], &img_bell4_, NOTSRCERASE);
		putimage(bell_x[1], bell_y[1], &img_bell4, SRCINVERT);
	}
	if (ring == 1 || ring == 8)
	{
		putimage(bell_x[2], bell_y[2], &img_bell1_, NOTSRCERASE);
		putimage(bell_x[2], bell_y[2], &img_bell1, SRCINVERT);
	}
	else if (ring == 2 || ring == 7)
	{
		putimage(bell_x[2], bell_y[2], &img_bell2_, NOTSRCERASE);
		putimage(bell_x[2], bell_y[2], &img_bell2, SRCINVERT);
	}
	else if (ring == 3 || ring == 6)
	{
		putimage(bell_x[2], bell_y[2], &img_bell3_, NOTSRCERASE);
		putimage(bell_x[2], bell_y[2], &img_bell3, SRCINVERT);
	}
	else if (ring == 4 || ring == 5)
	{
		putimage(bell_x[2], bell_y[2], &img_bell4_, NOTSRCERASE);
		putimage(bell_x[2], bell_y[2], &img_bell4, SRCINVERT);
	}
	if (ring == 4 || ring == 5)
	{
		putimage(bell_x[3], bell_y[3], &img_bell1_, NOTSRCERASE);
		putimage(bell_x[3], bell_y[3], &img_bell1, SRCINVERT);
	}
	else if (ring == 3 || ring == 6)
	{
		putimage(bell_x[3], bell_y[3], &img_bell2_, NOTSRCERASE);
		putimage(bell_x[3], bell_y[3], &img_bell2, SRCINVERT);
	}
	else if (ring == 2 || ring == 7)
	{
		putimage(bell_x[3], bell_y[3], &img_bell3_, NOTSRCERASE);
		putimage(bell_x[3], bell_y[3], &img_bell3, SRCINVERT);
	}
	else if (ring == 1 || ring == 8)
	{
		putimage(bell_x[3], bell_y[3], &img_bell4_, NOTSRCERASE);
		putimage(bell_x[3], bell_y[3], &img_bell4, SRCINVERT);
	}
	//铃铛到树上面的位置就消失，同时有一个新的铃铛补入。
	//铃铛每一次循环，下落1个像素。
	}

	if (MouseHit())//用来判断游戏是否应该开始。“开始”是一种输出，“未开始”又是另一种输出。
	{
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN) state = 1;
	}
	if (state == 0)
	{
		{
		//下面显示游戏开始前的小兔。
		//要求：静止时显示img_tl或img_tr（根据鼠标横坐标m.x增加或减少判断）；
		//      运动时交替显示img_ru、img_rd（一组）和img_lu、img_ld（一组）（根据鼠标横坐标m.x增加或减少判断）。
		//      另外，为了美观，始终让兔头位置跟随鼠标。
			if (direction1 == 0 && facing == -1)
			{
				putimage(rabbit_x, 610, &img_tl_, NOTSRCERASE);
				putimage(rabbit_x, 610, &img_tl, SRCINVERT);
			}
			else if (direction1 == 0 && facing == 1)
			{
				putimage(rabbit_x-50, 610, &img_tr_, NOTSRCERASE);
				putimage(rabbit_x-50, 610, &img_tr, SRCINVERT);
			}
			else if (direction1 == -1)//left
			{
				count1++;
				if (count1 == 10)
				{
					count1 = 0;
					direction2 = -direction2;
				}//实现起落状态的交替。
				if (direction2 == 1)//up
				{
					putimage(rabbit_x, 630, &img_lu_, NOTSRCERASE);
					putimage(rabbit_x, 630, &img_lu, SRCINVERT);
				}
				else if (direction2 == -1)//down
				{
					putimage(rabbit_x, 630, &img_ld_, NOTSRCERASE);
					putimage(rabbit_x, 630, &img_ld, SRCINVERT);
				}
			}
			else if (direction1 == 1)//right
			{
				count2++;
				if (count2 == 10)
				{
					count2 = 0;
					direction2 = -direction2;
				}//实现起落状态的交替。
				if (direction2 == 1)//up
				{
					putimage(rabbit_x - 87, 630, &img_ru_, NOTSRCERASE);
					putimage(rabbit_x - 87, 630, &img_ru, SRCINVERT);
				}
				else if (direction2 == -1)//down
				{
					putimage(rabbit_x - 72, 630, &img_rd_, NOTSRCERASE);
					putimage(rabbit_x - 72, 630, &img_rd, SRCINVERT);
				}
			}
			//上面已经处理完游戏开始前的小兔显示。
		}
	}
	else if (state == 1)//游戏开始
	{
	//这里面放置游戏主体内容。
		//下面判断小兔何时跳起，并实现带有重力加速度的跳起。第一次无条件（或者说单击鼠标左键后）跳起，之后踩到铃铛跳起。
		//规定当且仅当n=1时小兔要跳起。
		//一次跳起N个像素。
		for (j = 0; j <= 3; j++)
		{
			if (bell_x[j] + 25 + 35>= rabbit_x && bell_x[j] + 25 - 35 <= rabbit_x && bell_y[j] <= rabbit_y&&bell_y[j]+30>=rabbit_y)
			{
				if (height <= -20)	height+=10;//实现背景的改变。
				mciSendString("close jpmusic", NULL, 0, NULL);
				mciSendString("open ..\\res\\ding.mp3 alias jpmusic", NULL, 0, NULL);
				mciSendString("play jpmusic", NULL, 0, NULL);
				k = k + 10;
				score += k;
				if (bell_y[j] < 500)//加速下落，利用下落速度不同，更新画面,产生向上“爬升的错觉”。
				{
					newground = bell_y[j];
					bell_y[0]+=130;
					bell_y[1]+=130;
					bell_y[2]+=130;
					bell_y[3]+=130;
					g = 22;
				}
				break;
			}
		}
		rabbit_y -= g--;
		if (rabbit_y < 0) rabbit_y = 0;//限制小兔不超过上界。
		if (rabbit_y >641) rabbit_y = 641;//限制小兔不超过下界。
		//这里写完后就可以给出判断gameover的条件了，即：如果小兔的坐标掉出了整个画布的最大宽度，即可判定游戏失败。
        //************************************************************************
		{//这段代码用来处理游戏开始后小兔图片的输出问题。
			if (direction1 == -1)//left
			{
				if (upordown == 1)//up
				{
					putimage(rabbit_x, rabbit_y, &img_lu_, NOTSRCERASE);
					putimage(rabbit_x, rabbit_y, &img_lu, SRCINVERT);
				}
				else if (upordown == -1)//down
				{
					putimage(rabbit_x, rabbit_y, &img_ld_, NOTSRCERASE);
					putimage(rabbit_x, rabbit_y, &img_ld, SRCINVERT);
				}
			}
			else if (direction1 == 1)//right
			{
				if (upordown == 1)//up
				{
					putimage(rabbit_x - 87, rabbit_y, &img_ru_, NOTSRCERASE);
					putimage(rabbit_x - 87, rabbit_y, &img_ru, SRCINVERT);
				}
				else if (upordown == -1)//down
				{
					putimage(rabbit_x - 72, rabbit_y, &img_rd_, NOTSRCERASE);
					putimage(rabbit_x - 72, rabbit_y, &img_rd, SRCINVERT);
				}
			}
			else if (direction1 == 0&& facing == 1)//right
			{
				if (upordown == 1)//up
				{
					putimage(rabbit_x - 87, rabbit_y, &img_ru_, NOTSRCERASE);
					putimage(rabbit_x - 87, rabbit_y, &img_ru, SRCINVERT);
				}
				else//down
				{
					putimage(rabbit_x - 72, rabbit_y, &img_rd_, NOTSRCERASE);
					putimage(rabbit_x - 72, rabbit_y, &img_rd, SRCINVERT);
				}
			}
			else if (direction1 == 0&& facing == -1)//left
			{
				if (upordown == 1)//up
				{
				putimage(rabbit_x , rabbit_y, &img_lu_, NOTSRCERASE);
				putimage(rabbit_x , rabbit_y, &img_lu, SRCINVERT);
				}
				else//down
				{
				putimage(rabbit_x , rabbit_y, &img_ld_, NOTSRCERASE);
				putimage(rabbit_x , rabbit_y, &img_ld, SRCINVERT);
				}
			}
		}
        //*************上面这段代码用来处理游戏开始后小兔图片的输出问题。*********
	}
	{//打印分数。
	settextcolor(WHITE);
	settextstyle(30, 0, _T("微软雅黑"));
	char s[50];
	sprintf_s(s, "%d", score);
	outtextxy(10, 10, s);
	}
	FlushBatchDraw();
	Sleep(16);
}
void updateWithoutInput()
{
	{//让铃铛摇起来。
	slowdown++;
	if (ring != 8 && slowdown == 13)
	{
		ring++;
		slowdown = 0;
	}
	else if (ring == 8 && slowdown == 13)
	{
		ring = 1;
		slowdown = 0;
	}
	}
	{//让铃铛往下掉落。//让铃铛的位置随机。
		srand(100*time(0));//借鉴重构飞机游戏，用当前时间来初始化随机数。
		slowdown1++;
		if (slowdown1 == 2)
		{
			for (i = 0; i <= 3; i++)
			{
				if (bell_y[i] < 500) bell_y[i] += 1;
				else if (bell_y[i] >= 500)
				{
					bell_y[i] = bell_y[i] - 500;
					bell_x[i] = rand() % 960 + 20;
				}
			}
			slowdown1 = 0;
		}
	}

}
void updateWithInput()
{
	if (!MouseHit())    direction1 = 0;
	else
	{	
		m = GetMouseMsg();
		if (m.uMsg == WM_MOUSEMOVE)
		{
			rabbit_x = m.x;
		}
		else if (m.uMsg == WM_LBUTTONDOWN) state = 1;
		
		//下面判断方向（左右和初始时的上下）。
		if (m.x == OldXPosition && OlderXPosition == OldXPosition)    direction1 = 0;
		else if (m.x > OldXPosition&& OldXPosition >= OlderXPosition) {direction1 = 1; facing = 1; }
		else if (m.x < OldXPosition && OldXPosition <= OlderXPosition) {direction1 = -1; facing = -1; }
		OlderXPosition = OldXPosition;
		OldXPosition = m.x;	
	}
	//下面判断方向（开始游戏后的上下）
	if (rabbit_y < OldYPosition)	upordown = 1;
	else if (rabbit_y > OldYPosition)	upordown = -1;
	OldYPosition = rabbit_y;
	if (rabbit_y >= 640)
	{
		readRecordFile();
		if (score > bestscore)
		{
			bestscore = score;
			writeRecordFile();
		}
		putimage(310, 160, &img_score);
		settextcolor(WHITE);
		settextstyle(30, 0, _T("微软雅黑"));
		char s1[50],s2[50];
		sprintf_s(s1, "%d", score);
		outtextxy(492, 300, s1);
		sprintf_s(s2, "%d", bestscore);
		outtextxy(492, 400, s2);
		FlushBatchDraw();
		gameover();
	}
}

int main()//主函数部分使用FlappyBird2的框架。
{
	
		startup();
		while (1)
		{
			show();
			updateWithoutInput();
			updateWithInput();
		}
	return 0;
}
