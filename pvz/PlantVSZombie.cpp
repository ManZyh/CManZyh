// PlantVSZombie.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include<stdio.h>
#include<string.h>
#include<time.h>
#include<graphics.h>//easyx的图形库的头文件，需要安装easyx图形库
#include "tools.h"
#include"vector2.h"
#include<math.h>
#include<mmsystem.h>
#pragma comment(lib,"winmm.lib")

enum{WAN_DOU,XIANG_RI_KUI,ZHI_WU_COUNT};//植物种类，最后一个为种类个数。
int curX, curY;//当前选中的植物在移动中的位置
int curZhiWu;//>0,0:表示没有选中植物，1：表示选中第一种植物
int sun[ZHI_WU_COUNT] = { 100,50 };//存放各种植物种植所需的阳光值

IMAGE imgBg;//表示图片背景
IMAGE imgBar;//游戏上面工具栏
IMAGE imgCards[ZHI_WU_COUNT];//工具栏中的植物图片
IMAGE* imgZhiWu[ZHI_WU_COUNT][20];//存放植物帧照片

enum{GOING,WIN,FAIL};
int killCount;//已经杀掉的僵尸个数
int ZMCount;//已经出现的僵尸个数
int gameStatus;//游戏状态
#define ZM_MAX 10
//植物数据类型
struct zhiwu
{
    int type; //0:没有植物，1：表示第一种植物
    int frameIndex;//植物摇摆时序列帧的序号
    bool catched;//植物是否正在被僵尸吃
    int deadTime;//植物要被吃多久
    int timer;//计时器，计时向日葵喷射阳光的时间
    int x, y;//植物的坐标
    int shootTime;//植物发射豌豆的计数器
};
struct zhiwu map[3][9];//map[3][9]用来表示种植物的地方

//阳光球的四种状态，阳光下降，阳光不动，阳光收集，阳光生产
enum{SUMSHINE_DOWN,SUNSHINE_GROUND,SUNSHINE_COLLECT,SUNSHINE_PRODUCT};
//阳光球数据类型
struct sunshineBall
{
    int x, y;//阳光球在飘落过程中的位置（x不变）
    int frameIndex;//当前显示阳光球图片的帧序号
    int destY;//飘落的目标位置的y坐标
    bool used;//用来判断sunshineBall里面的阳光球是否正在被使用
    int timer;//用来计时阳光停留时间
    float xoff;//
    float yoff;
    float t;//贝塞尔曲线的时间，从0到1
    vector2 p1, p2, p3, p4;//贝塞尔曲线的起点，控制点，控制点，终点
    vector2 pCur;//阳光球当前时刻的位置
    float speed;//阳光球速度
    int status;//阳光球状态
};
struct sunshineBall balls[10];//用来存放阳光球
IMAGE imgSunshineBall[29];//表示阳光球的帧图片
int sunshine;//初始阳光值

//僵尸数据类型
struct zm
{
    int x, y;//僵尸坐标
    int frameIndex;//僵尸走路的帧
    bool used;//判断zms里面的僵尸是否正在被使用
    int speed;//僵尸的速度
    int row;//僵尸所在行
    int blood;//僵尸血量
    bool dead;//僵尸是否死亡
    bool eating;//僵尸是否正在吃植物
};//僵尸
struct zm zms[10];//僵尸池
IMAGE imgZM[22];//存放僵尸帧图片
IMAGE imgZMDead[20];//存放僵尸死亡的图片帧
IMAGE imgZMEat[21];//存放僵尸吃植物的图片帧
IMAGE imgZMStand[11];//存放僵尸站立时的图片帧

//子弹数据类型
struct bullet
{
    int x, y;//子弹坐标
    int row;//子弹所在行
    bool used;//判断bullets中的子弹是否被使用
    int speed;//子弹的速度
    bool blast;//判断子弹是否爆炸
    int frameIndex;//子弹爆炸时的帧序号
};
struct bullet bullets[30];//子弹池
IMAGE imgBulletNormal;//子弹常规形态的照片
IMAGE imgBullBlast[4];//子弹爆炸时的帧图片



#define WIN_WIDTH 900//窗口的宽
#define WIN_HEIGHT 600//窗口的高

bool fileExist(const char* name)
{
    FILE* fp = fopen(name, "r");
    if (fp == NULL)
    {
        return false;
    }
    else
    {
        fclose(fp);
        return true;
    }
}

void gameInit()//初始化
{
    //加载背景图片
    loadimage(&imgBg,"res/bg.jpg");//记得把字符集转化为多字节字符集
    //加载工具栏图片
    loadimage(&imgBar, "res/bar5.png");
    //加载植物卡牌
    memset(imgZhiWu, 0, sizeof(imgZhiWu));//植物拖动时
    memset(map, 0, sizeof(map));
    /*memset` 是一个在 C/C++ 中常用的函数，用于将指定的内存区域设置为特定的值。
     以下是 `memset` 函数的一般用法：
        void* memset(void* ptr, int value, size_t num);
         参数说明：
          - `ptr`：指向要设置的内存区域的指针。
          - `value`：要设置的值，通常是一个整数。
          - `num`：要设置的字节数。*/

    ZMCount = 0;
    killCount = 0;
    gameStatus = GOING;

    char name[64];
    for (int i = 0; i < ZHI_WU_COUNT; i++)
    {
     /*   int sprintf_s(char* buffer, size_t sizeOfBuffer, const char* format, ...)
        参数说明：
            - `buffer`：指向目标字符数组的指针，用于存储格式化后的字符串。
            - `sizeOfBuffer`：目标字符数组的大小。
            - `format`：格式化字符串，包含了要在最终字符串中插入的文本和占位符。
            - `...`：可变数量的参数，用于填充格式化字符串中的占位符*/
        sprintf_s(name, sizeof(name), "res/Cards/card_%d.png", i + 1);
        loadimage(&imgCards[i], name);

        for (int j = 0; j < 20; j++)
        {
            sprintf_s(name, sizeof(name), "res/zhiwu/%d/%d.png", i, j + 1);
            //先判断文件存在
            if (fileExist(name))
            {
                imgZhiWu[i][j] = new IMAGE;
                loadimage(imgZhiWu[i][j], name);
            }
            else
            {
                break;
            }
        }
    }
    
    curZhiWu = 0;
    sunshine = 50;
    memset(balls, 0, sizeof(balls));
    for (int i = 0; i < 29; i++)
    {
        sprintf_s(name, sizeof(name), "res/sunshine/%d.png", i + 1);
        loadimage(&imgSunshineBall[i], name);
    }//将阳光球的图片加载
    srand(time(NULL));
    initgraph(WIN_WIDTH, WIN_HEIGHT,1); //创建游戏窗口

    //设置字体
    LOGFONT f;
    gettextstyle(&f);
    f.lfHeight = 30;
    f.lfWeight = 15;
    strcpy(f.lfFaceName, "Segoe UI Black");
    f.lfQuality = ANTIALIASED_QUALITY;//抗锯齿效果
    settextstyle(&f);
    setbkmode(TRANSPARENT);//设置背景透明
    setcolor(BLACK);

    //初始化僵尸
    memset(zms, 0, sizeof(zms));
    for (int i = 0; i < 22; i++)
    {
        sprintf_s(name, sizeof(name), "res/zm/%d.png", i + 1);
        loadimage(&imgZM[i], name);
    }
     
    //初始化子弹
    loadimage(&imgBulletNormal, "res/bullets/bullet_normal.png");
    memset(bullets, 0, sizeof(bullets));

    //初始化子弹爆炸帧图片
    loadimage(&imgBullBlast[3], "res/bullets/bullet_blast.png");
    for (int i = 0; i < 3; i++)
    {
        float k = (i + 1) * 0.2;
        loadimage(&imgBullBlast[i], "res/bullets/bullet_blast.png"
            , imgBullBlast[3].getwidth() * k
            , imgBullBlast[3].getheight() * k
            , true);
    }

    //初始化僵尸死亡图片
    for (int i = 0; i < 20; i++)
    {
        sprintf_s(name,sizeof(name), "res/zm_dead/%d.png", i + 1);
        loadimage(&imgZMDead[i], name);
    }

    //初始化僵尸吃植物的图片
    for (int i = 0; i < 21; i++)
    {
        sprintf_s(name, sizeof(name), "res/zm_eat/%d.png", i + 1);
        loadimage(&imgZMEat[i], name);
    }

    //初始化僵尸站立的图片
    for (int i = 0; i < 11; i++)
    {
        sprintf_s(name, sizeof(name), "res/zm_stand/%d.png", i + 1);
        loadimage(&imgZMStand[i], name);
    }
}

void drawZM()//绘制僵尸
{
    int zmCount = sizeof(zms) / sizeof(zms[0]);
    for (int i = 0; i < zmCount; i++)
    {
        if (zms[i].used)
        {
            IMAGE* img = NULL;
            if (zms[i].dead)img = imgZMDead;
            else if (zms[i].eating)img = imgZMEat;
            else img = imgZM;
            img += zms[i].frameIndex;//让指针指到对应图片
            putimagePNG(zms[i].x, zms[i].y - img->getheight(), img);
        }
    }
}

void drawSunshines()//渲染阳光球
{
    int ballMax = sizeof(balls) / sizeof(balls[0]);
    for (int i = 0; i < ballMax; i++)
    {
        if (balls[i].used/*||balls[i].xoff*/)
        {
            IMAGE* img = &imgSunshineBall[balls[i].frameIndex];
            /*putimagePNG(balls[i].x, balls[i].y, img);*/
            putimagePNG(balls[i].pCur.x, balls[i].pCur.y, img);
        }
    }
}
void drawCards()//卡牌渲染
{
    for (int i = 0; i < ZHI_WU_COUNT; i++)
    {
        int x = i * 65 + 338;
        int y = 6;
        putimage(x, y, &imgCards[i]);
    }
}

void drawZhiWu()//植物渲染
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (map[i][j].type > 0)
            {
                /*int x = 256 + j * 81;
                int y = 179 + i * 102+14;*/
                int zhiWuType = map[i][j].type - 1;
                int index = map[i][j].frameIndex;
                /*putimagePNG(x, y, imgZhiWu[zhiWuType][index]);*/
                putimagePNG(map[i][j].x, map[i][j].y, imgZhiWu[zhiWuType][index]);
            }
        }
    }
    //渲染拖动过程中的植物
    if (curZhiWu > 0)
    {
        IMAGE* img = imgZhiWu[curZhiWu - 1][0];
        putimagePNG(curX - img->getwidth() / 2, curY - img->getheight() / 2, img);
    }
}

void drawText()//渲染字体
{
    char scoreText[8];
    sprintf_s(scoreText, sizeof(scoreText), "%d", sunshine);
    outtextxy(276, 67, scoreText);//输出字体
}

void drawBullets()//渲染子弹
{
    int bulletMax = sizeof(bullets) / sizeof(bullets[0]);
    for (int i = 0; i < bulletMax; i++)
    {

        if (bullets[i].used)
        {
            if (bullets[i].blast)
            {
                IMAGE* img = &imgBullBlast[bullets[i].frameIndex];
                putimagePNG(bullets[i].x, bullets[i].y, img);
            }
            else
            {
                putimagePNG(bullets[i].x, bullets[i].y, &imgBulletNormal);
            }

        }
    }//渲染子弹
}

void updateWindow()//渲染
{
    BeginBatchDraw();//开始缓冲，把要打印的东西先放在缓冲区
    //把图片放在窗口上，前2个数字代表x轴和y轴分别表示到窗口左边和到窗口上边的距离
    putimage(-112, 0, &imgBg);
    putimagePNG(250, 0, &imgBar);

    drawCards();//卡牌渲染
   
    drawZhiWu();//植物渲染

    drawSunshines();//渲染阳光球

    drawText();//渲染字体

    drawZM();//绘制僵尸

    drawBullets();//渲染子弹

    EndBatchDraw();//结束双缓冲
}
    
void collectSunshine(ExMessage* msg)//收集阳光
{
    int count = sizeof(balls) / sizeof(balls[0]);
    int w = imgSunshineBall[0].getwidth();//阳光的宽
    int h = imgSunshineBall[0].getheight();//阳光的高
    for (int i = 0; i < count; i++)
    {
        if (balls[i].used)
        {
           /* int x = balls[i].x;
            int y = balls[i].y;*/
            int x = balls[i].pCur.x;
            int y = balls[i].pCur.y;
            if (msg->x > x && msg->x<x + w && msg->y>y && msg->y<y + h)
            {
                /*balls[i].used = false;*/
                balls[i].status = SUNSHINE_COLLECT;
              /*  sunshine += 25;*/
                //mciSendString("play res/sunshine.mp3",0,0,0);//音乐
                PlaySound("res/sunshine.wav", NULL, SND_FILENAME | SND_ASYNC);
               // //设置阳光球的偏移量
               ///* float destY = 0;
               // float destX = 262;
               // float angle = atan((y - destY) / (x - destX));
               // balls[i].xoff = 4 * cos(angle);
               // balls[i].yoff = 4 * sin(angle);*/
                balls[i].p1 = balls[i].pCur;
                balls[i].p4 = vector2(262, 0);
                balls[i].t = 0;
                float distance = dis(balls[i].p1 - balls[i].p4);//起点和终点的距离
                float off = 8;//偏移量
                balls[i].speed = 1.0 / (distance / off);
                break;
            }
        }
    }
}

void userClick()//用户点击时
{
    ExMessage msg;
    static int status = 0;
    if (peekmessage(&msg))
    {
        if (msg.message == WM_LBUTTONDOWN)//收到鼠标点击左键的信息时
        {
            if (msg.x > 338 && msg.x < 338 + 65 * ZHI_WU_COUNT && msg.y < 96)
            {
                int index = (msg.x - 338) / 65;
                status = 1;
                curZhiWu = index + 1;
            }
            else
            {
                collectSunshine(&msg);//收集阳光
            }
        }
        else if (msg.message == WM_MOUSEMOVE && status==1)//收到鼠标移动的信息时
        {
            curX = msg.x;
            curY = msg.y;
        }
        else if (msg.message == WM_LBUTTONUP && status == 1 && sunshine>= sun[curZhiWu - 1])//收到鼠标左键松开的信息时
        {
            if (msg.x > 256-112 && msg.y > 170 && msg.y < 489)
            {
                int row = (msg.y - 179) / 102;
                int col = (msg.x - 256+112) / 81;
                if (map[row][col].type == 0)
                {
                    map[row][col].type = curZhiWu;
                    map[row][col].frameIndex = 0;
                    map[row][col].shootTime = 0;
                    map[row][col].x = 256 -112 + col * 81;
                    map[row][col].y = 179 + row * 102 + 14;
                    sunshine -= sun[curZhiWu-1];
                }
            }
            curZhiWu = 0;
            status = 0;
        }
        else if (msg.message == WM_LBUTTONUP && status == 1 && sunshine < sun[curZhiWu - 1])
        {
            curZhiWu = 0;
            status = 0;
        }
    }
}

void createSunshine()//创建阳光
{
    //随机掉落阳光
    static int count = 0;
    static int fre = 800;
    count++;//用来避免阳光生成过快
    if (count >= fre)
    {
        fre = 400 + rand() % 400;
        count = 0;
        //从阳光池中取一个可以使用的
        int ballMax = sizeof(balls) / sizeof(balls[0]);
        int i = 0;
        for (i = 0; i < ballMax && balls[i].used; i++);
        if (i > ballMax)return;
        balls[i].used = true;
        balls[i].frameIndex = 0;
        /*balls[i].x = 260 + rand() % (900 - 260);
        balls[i].y = 60;
        balls[i].destY = 200 + (rand() % 4) * 90;*/
        balls[i].timer = 0;
       /* balls[i].xoff = 0;
        balls[i].yoff = 0;*/
        balls[i].status = SUMSHINE_DOWN;
        balls[i].t = 0;
        balls[i].p1 = vector2(260-112 + rand() % (900 - 260+112), 60);
        balls[i].p4 = vector2(balls[i].p1.x, 200 + (rand() % 4) * 90);
        int off = 2;//偏移量，每次移动的距离
        float distance = balls[i].p4.y - balls[i].p1.y;
        balls[i].speed = 1.0 / (distance / off);
    }
    //向日葵生产阳光
    int ballMax = sizeof(balls) / sizeof(balls[0]);
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (map[i][j].type == XIANG_RI_KUI + 1)
            {
                map[i][j].timer++;
                if (map[i][j].timer > 400)
                {
                    map[i][j].timer = 0;

                    int k;
                    for (k = 0; k < ballMax && balls[k].used; k++);
                    if (k >= ballMax)return;
                    balls[k].used = true;
                    balls[k].p1 = vector2(map[i][j].x, map[i][j].y);
                    int w = (100 + rand() % 50) * (rand() % 2 ? 1 : -1);
                    balls[k].p4 = vector2(map[i][j].x + w,
                        map[i][j].y + imgZhiWu[XIANG_RI_KUI][0]->getheight() - imgSunshineBall[0].getheight());
                    balls[k].p2 = vector2(balls[k].p1.x + w * 0.3, balls[k].p1.y - 100);
                    balls[k].p3 = vector2(balls[k].p1.x + w * 0.7, balls[k].p1.y - 100);
                    balls[k].status = SUNSHINE_PRODUCT;
                    balls[k].speed = 0.05;
                    balls[k].t = 0;
                }
            }
        }
    }
}

void updateSunshine()
{
//    int ballMax = sizeof(balls) / sizeof(balls[0]);
//   
//    for (int i = 0; i < ballMax && balls[i].used; i++)
//    {
//       
//        if (balls[i].used)
//        {
//            balls[i].frameIndex = (balls[i].frameIndex + 1) % 29;//改变帧图片
//            if (balls[i].timer == 0) { balls[i].y += 2; }//下移阳光
//            if (balls[i].y >= balls[i].destY)
//            {
//                balls[i].timer++;
//                if (balls[i].timer >= 100)
//                {
//                    balls[i].used = false;//让阳光停留一会
//                }   
//            }
//        }
//        //else if (balls[i].xoff)
//        //{
//        //    //设置阳光偏移量
//        //    float destY = 0;
//        //    float destX = 262;
//        //    float angle = atan((balls[i].y - destY) / (balls[i].x - destX));
//        //    balls[i].xoff = 4 * cos(angle);
//        //    balls[i].yoff = 4 * sin(angle);//在这里设置每帧都会计算一遍，更准确
//
//        //    balls[i].x -= balls[i].xoff;
//        //    balls[i].y -= balls[i].yoff;
//        //    if (balls[i].y < 0 || balls[i].x < 262)
//        //    {
//        //        balls[i].xoff = 0;
//        //        balls[i].yoff = 0;
//        //        sunshine += 25;
//        //    }
//        //}
//    }
   int ballMax = sizeof(balls) / sizeof(balls[0]);
  
   for (int i = 0; i < ballMax && balls[i].used; i++)
   {
    if (balls[i].used)
    {
        balls[i].frameIndex = (balls[i].frameIndex + 1) % 29;//改变帧图片
        if (balls[i].status == SUMSHINE_DOWN)
        {
            struct sunshineBall* sun = &balls[i];
            sun->t += sun->speed;
            sun->pCur = sun->p1 + sun->t * (sun->p4 - sun->p1);
            if (sun->t >= 1)
            {
                sun->status = SUNSHINE_GROUND;
                sun->timer = 0;
            }
        }
        else if (balls[i].status == SUNSHINE_GROUND)
        {
            balls[i].timer++;
            if (balls[i].timer > 100)
            {
                balls[i].used = false;
                balls[i].timer = 0;
            }
        }
        else if (balls[i].status == SUNSHINE_COLLECT)
        {
            struct sunshineBall* sun = &balls[i];
            sun->t += sun->speed;
            sun->pCur = sun->p1 + sun->t * (sun->p4 - sun->p1);
            if (sun->t >= 1)
            {
                sun->used = false;
                sunshine += 25;
            }
        }
        else if (balls[i].status == SUNSHINE_PRODUCT)
        {
            struct sunshineBall* sun = &balls[i];
            sun->t += sun->speed;
            sun->pCur = calcBezierPoint(sun->t, sun->p1, sun->p2, sun->p3, sun->p4);
            if (sun->t >= 1)
            {
                sun->status = SUNSHINE_GROUND;
                sun->timer = 0;
            }
        }
    }
   }
}

void createZM()//创建僵尸
{
    if (ZMCount >= ZM_MAX)return;
    static int zmFre = 500;
    static int count = 0;
    count++;
    if (count >= zmFre)
    {
        count = 0;
        zmFre = rand() % 200 + 300;
        int i = 0;
        int zmMax = sizeof(zms) / sizeof(zms[0]);
        for (i = 0; i < zmMax && zms[i].used; i++);
        if (i < zmMax)
        {
            memset(&zms[i], 0,sizeof(zms[i]));
            zms[i].used = true;
            zms[i].x = WIN_WIDTH;
            zms[i].row = rand() % 3;
            zms[i].y = 172 + (1 + zms[i].row) * 100;
            zms[i].speed = 1;
            zms[i].blood = 100;
            zms[i].dead = false;
            ZMCount ++;
        }
    }
   

}

void  updateZM()//更新僵尸的状态
{
    int zmMax = sizeof(zms) / sizeof(zms[0]);
    //更新僵尸的位置
    static int count = 0;
    count++;
    if (count > 2*2)
    {
        count = 0;
        for (int i = 0; i < zmMax; i++)
        {
            if (zms[i].used)
            {
                zms[i].x -= zms[i].speed;
                if (zms[i].x < 170-112)
                {
                    //printf("game over");
                    //MessageBox(NULL, "over", "over", 0);//待优化
                    //exit(0);//待优化
                    gameStatus = FAIL;
                }
            }
        }
    }//僵尸走路速度
    static int count2 = 0;
    count2++;
    if (count2 > 4*2)
    {
        count2 = 0;
        for (int i = 0; i < zmMax; i++)
        {
            if (zms[i].used)
            {
                if (zms[i].dead)
                {
                    zms[i].frameIndex++;
                    if (zms[i].frameIndex >= 20)
                    {
                        zms[i].used = false;
                        killCount++;
                        if (killCount == ZM_MAX)
                        {
                            gameStatus = WIN;
                        }
                    }
                }
                else if (zms[i].eating)
                {
                    zms[i].frameIndex = (zms[i].frameIndex + 1) % 21;
                }
                else
                {
                    zms[i].frameIndex = (zms[i].frameIndex + 1) % 22;
                }
                
            }
        }
    }//僵尸走路是帧变化
    
  
}

void shoot()//发射豌豆子弹
{
    static int count = 0;
    if (++count < 2)return;
    count = 0;//减缓速度
    int lines[3] = { 0 };
    int zmCount = sizeof(zms) / sizeof(zms[0]);
    int bulletMax = sizeof(bullets) / sizeof(bullets[0]);
    int dangerX = WIN_WIDTH - imgZM[0].getwidth();
    for (int i = 0; i < zmCount; i++)
    {
        if (zms[i].used && zms[i].x < dangerX)
        {
            lines[zms[i].row] = 1;
        }
    }
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (map[i][j].type == WAN_DOU + 1 && lines[i])
            {
                map[i][j].shootTime++;
              
                if (map[i][j].shootTime > 20)
                {
                    map[i][j].shootTime = 0;
                    int k=0;
                    for (k = 0; k < bulletMax && bullets[k].used; k++);
                    if (k < bulletMax)
                    {
                        bullets[k].used = true;
                        bullets[k].row = i;
                        bullets[k].speed = 6;
                        bullets[k].blast = false;
                        bullets[k].frameIndex = 0;
                        int zwX = 256 -112 + j * 81;//植物的x坐标
                        int zwY = 179 + i * 102 + 14;
                        bullets[k].x = zwX + imgZhiWu[map[i][j].type - 1][0]->getwidth()-10;
                        bullets[k].y = zwY + 5;
                    }

                }
            }
        }
    }
}

void updateBullets()//更新豌豆子弹状态
{
    static int count = 0;
    if (++count < 2)return;
    count = 0;
    int countMax = sizeof(bullets) / sizeof(bullets[0]);
    for (int i = 0; i < countMax; i++)
    {
        if (bullets[i].used)
        {
            bullets[i].x += bullets[i].speed;

            if (bullets[i].x > WIN_WIDTH)
            {
                bullets[i].used = false;
            }

            if (bullets[i].blast)
            {
                bullets[i].frameIndex++;
                if (bullets[i].frameIndex >= 4)
                {
                    bullets[i].used = false;
                }
            }
        }
    } 
}

void checkBulletToZm()//检测子弹对僵尸的碰撞
{
    int bCount = sizeof(bullets) / sizeof(bullets[0]);
    int zmCount = sizeof(zms) / sizeof(zms[0]);
    for (int i = 0; i < bCount; i++)
    {
        if (bullets[i].used == false || bullets[i].blast)continue;
        for (int k = 0; k < zmCount; k++)
        {
            if (zms[k].used == false)continue;
            int x1 = zms[k].x + 80;
            int x2 = zms[k].x + 110;
            int x = bullets[i].x;
            if (zms[k].dead == false && bullets[i].row == zms[k].row && x > x1 && x < x2)
            {
                zms[k].blood -= 10;
                bullets[i].blast = true;
                bullets[i].speed = 0;

                if (zms[k].blood <= 0)
                {
                    zms[k].dead = true;
                    zms[k].speed = 0;
                    zms[k].frameIndex = 0;
                }
                break;
            }
        }
    }
}

void checkZmToZhiWu()//检测僵尸对植物的碰撞
{
    int zCount = sizeof(zms) / sizeof(zms[0]);
    for (int i = 0; i < zCount; i++)
    {
        if (zms[i].dead)continue;
        int row = zms[i].row;
        for (int k = 0; k < 9; k++)
        {
            if (map[row][k].type == 0)
            {
                continue;
            }
            int zhiWuX = 256 -112 + k * 81;
            int x1 = zhiWuX + 10;
            int x2 = zhiWuX + 60;
            int x3 = zms[i].x + 80;
            if (x3 > x1 && x3 < x2)
            {
                if (map[row][k].catched)
                {
                    map[row][k].deadTime++;
                    if (map[row][k].deadTime > 100)
                    {
                        map[row][k].type = 0;
                        map[row][k].deadTime = 0;
                        zms[i].eating = false;
                        zms[i].speed = 1;
                        zms[i].frameIndex = 0;
                    }
                }
                else
                {
                    map[row][k].catched = true;
                    map[row][k].deadTime = 0;
                    zms[i].frameIndex = 0;
                    zms[i].eating = true;
                    zms[i].speed = 0;

                }
            }
        }
       
    }
}
void collisionCheck()//实现豌豆子弹和僵尸的碰撞检测
{
    checkBulletToZm();
    checkZmToZhiWu();
}

void updateZhiWu()
{
    //更新植物帧画面，实现植物摆动
    static int count = 0;
    if (++count < 2)return;
    count = 0;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (map[i][j].type > 0)
            {
                map[i][j].frameIndex++;
                int zhiWuType = map[i][j].type - 1;
                int index = map[i][j].frameIndex;
                if (imgZhiWu[zhiWuType][index] == NULL)
                {
                    map[i][j].frameIndex = 0;
                }
            }
        }
    }
}

void updateGame()
{

    updateZhiWu();//更新植物帧画面，实现植物摆动

    createSunshine();//创建阳光
    updateSunshine();//更新阳光的状态

    createZM();//创建僵尸
    updateZM();//更新僵尸的状态

    shoot();//发射豌豆子弹
    updateBullets();//更新豌豆子弹状态

    collisionCheck();//实现豌豆子弹和僵尸的碰撞检测
}

void startUI()//开始菜单
{
    IMAGE imgBg, imgMenu1,imgMenu2;
    loadimage(&imgBg, "res/menu.png");
    loadimage(&imgMenu1, "res/menu1.png");
    loadimage(&imgMenu2, "res/menu2.png");
    int flag = 0;
    while (1)
    {
        BeginBatchDraw();
        putimage(0, 0, &imgBg);
        putimagePNG(474, 75, flag ? &imgMenu2 : &imgMenu1);
        ExMessage msg;
        if (peekmessage(&msg))
        {
            if (msg.message == WM_LBUTTONDOWN && msg.x > 474 && msg.x < 474 + 300 && msg.y>75 && msg.y < 75 + 140)//收到鼠标点击左键的信息时
            {
                flag = 1;
            }
            else if (msg.message == WM_LBUTTONUP&&flag)//收到鼠标左键松开的信息时
            {
             
                return;
            }
        }
        EndBatchDraw();
    }
}

void  viewScence()//查看场景
{
    int xMin = WIN_WIDTH - imgBg.getwidth();
    vector2 points[9]{
        {550,80},{530,160},{630,170},{530,200},{515,270},
        {565,370},{605,340},{705,280},{690,340}
    };
    int index[9];
    for (int i = 0; i < 9; i++)
    {
        index[i] = rand() % 11;
    }
    int count = 0;
    for (int x = 0; x >= xMin; x -= 3)
    {
        BeginBatchDraw();
        putimagePNG(x, 0, &imgBg);
        count++;
        for (int k = 0; k < 9; k++)
        {
            putimagePNG(points[k].x - xMin + x, points[k].y, &imgZMStand[index[k]]);
            if (count > 5)
            {
                index[k] = (index[k] + 1) % 11;
            }
           
        }
        if (count > 5)
        {
            count = 0;
        }
        EndBatchDraw();
        Sleep(5);
    }
    
    for (int i = 0; i < 100; i++)
    {
        BeginBatchDraw();
        putimagePNG(xMin, 0, &imgBg);
        count++;
        for (int k = 0; k < 9; k++)
        {
            putimagePNG(points[k].x, points[k].y, &imgZMStand[index[k]]);
            if (count > 5)
            {
                index[k] = (index[k] + 1) % 11;
            }

        }
        if (count > 5)
        {
            count = 0;
        }
        EndBatchDraw();
        Sleep(5);
    }

    for (int x = xMin; x <=-112; x += 3)
    {
        BeginBatchDraw();
        putimagePNG(x, 0, &imgBg);
        count++;
        for (int k = 0; k < 9; k++)
        {
            putimagePNG(points[k].x - xMin + x, points[k].y, &imgZMStand[index[k]]);
            if (count > 5)
            {
                index[k] = (index[k] + 1) % 11;
            }

        }
        if (count > 5)
        {
            count = 0;
        }
        EndBatchDraw();
        Sleep(5);
    }
}

void  barsDown()//下滑工具栏
{
    int height = imgBar.getheight();
    for (int y = -height; y <= 0; y++)
    {
        BeginBatchDraw();
        putimage(-112, 0, &imgBg);
        putimagePNG(250, y, &imgBar);
        for (int i = 0; i < ZHI_WU_COUNT; i++)
        {
            int x = 338 + i * 65;
            putimagePNG(x, 6+y, &imgCards[i]);
        }
        EndBatchDraw();
        Sleep(5);
    }
}

bool checkOver()//检查游戏是否结束
{
    int ret = false;
    if (gameStatus == WIN)
    {
        Sleep(2000);
        loadimage(0, "res/win2.png");
        ret = true;
    }
    else if (gameStatus == FAIL)
    {
        Sleep(2000);
        loadimage(0, "res/fail2.png");
        ret = true;
    }
    return ret;
}
int main()
{
    gameInit();
    startUI();
    viewScence();//查看场景
    barsDown();//下滑工具栏
    int timer = 0;
    bool flag = true;
    while (1)
    {
        userClick();
        timer += getDelay();
        //getDelay是作者直接写的在tool.h里面，意思为与上一次用户操作的时间间隔
        if (timer > 10)
        {
            flag = true;
            timer = 0;
        }
        if (flag)
        {
            flag = false;
         
            updateWindow();
            updateGame();
            if(checkOver())break;
        }
       
    }
   
    system("pause");
    return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
