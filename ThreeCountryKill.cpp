// ThreeCountryKill.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include<stdio.h>
#include<string.h>
#include<time.h>
#include<graphics.h>//easyx的图形库的头文件，需要安装easyx图形库

#define WIN_WIDTH 1200//窗口的宽
#define WIN_HEIGHT 900

struct cards
{
    int index;//1:杀，2：闪，3：桃，4：酒
    struct cards* next;
};
IMAGE imgCards[4];

struct general
{
    int index;
    struct cards* Cards;
    int goal;
    struct cards* currentCard;
    int blood;
    int k;//判断是要出哪一张牌
    struct cards* q;
    bool flag;//判断是否酒杀
    int cardCount;//卡牌数量
    bool start;//是否开始摸牌
    bool round;//是否在当前回合
    bool ifKill;//判断是否有杀
    bool ifAlcoholic;//判断是否有酒
};
struct general MyGeneral;//我的武将
struct general Generals[4];//武将
IMAGE imgGame;
IMAGE imgGeneral[4];
IMAGE imgGame2;

IMAGE imgText[4];
IMAGE imgAbandon;

int whoseRound = 0;//谁的回合
void gameInit()
{
    /*BeginBatchDraw();*/
   
    initgraph(WIN_WIDTH, WIN_HEIGHT,1); //创建游戏窗口
    loadimage(&imgGame, "res/game.jpg");

    char name[64];
    for (int i = 0; i < 4; i++)
    {
        sprintf_s(name, sizeof(name), "res/general/general%d.jpg", i + 1);
        loadimage(&imgGeneral[i], name);
    }
    srand(time(NULL));

    loadimage(&imgCards[0], "res/cards/杀.jpg");
    loadimage(&imgCards[1], "res/cards/闪.jpg");
    loadimage(&imgCards[2], "res/cards/桃.jpg");
    loadimage(&imgCards[3], "res/cards/酒.jpg");

    loadimage(&imgGame2, "res/game2.jpg");

    for (int i = 0; i < 4; i++)
    {
        Generals[i].blood = 4;
    }

    //设置字体
    LOGFONT f;
    gettextstyle(&f);
    f.lfHeight = 30;
    f.lfWeight = 15;
    strcpy(f.lfFaceName, "Segoe UI Black");
    f.lfQuality = ANTIALIASED_QUALITY;//抗锯齿效果
    settextstyle(&f);
    setbkmode(TRANSPARENT);//设置背景透明
    setcolor(RED);

    for (int i = 0; i < 4; i++)
    {
        sprintf_s(name,sizeof(name), "res/text/text%d.jpg", i + 1);
        loadimage(&imgText[i], name);
    }

    loadimage(&imgAbandon, "res/abandon.jpg");
    whoseRound = 0;
    /*EndBatchDraw();*/
}

void startUI()//选择武将菜单
{
    IMAGE imgBg, imgBg1, imgBg2, imgBg3, imgBg4;
    loadimage(&imgBg, "res/yyy.jpg");
    loadimage(&imgBg1, "res/yyy1.jpg");
    loadimage(&imgBg2, "res/yyy2.jpg");
    loadimage(&imgBg3, "res/yyy3.jpg");
    loadimage(&imgBg4, "res/yyy4.jpg");
    putimage(0, 0, &imgBg);
    int flag = 0;
    while (1)
    {
       
        ExMessage msg;
        if (peekmessage(&msg))
        {
            if (msg.message == WM_LBUTTONDOWN && msg.x > 100 && msg.x < 310 && msg.y>50 && msg.y < 350)//收到鼠标点击左键的信息时
            {
                putimage(0, 0, &imgBg1);
                MyGeneral.index = 1;
                flag = 1;
            }
            else if (msg.message == WM_LBUTTONDOWN && msg.x > 355 && msg.x < 580 && msg.y>50 && msg.y < 350)//收到鼠标点击左键的信息时
            {
                putimage(0, 0, &imgBg2);
                MyGeneral.index = 2;
                flag = 1;
            }
            else if (msg.message == WM_LBUTTONDOWN && msg.x > 640 && msg.x < 860 && msg.y>50 && msg.y < 350)//收到鼠标点击左键的信息时
            {
                putimage(0, 0, &imgBg3);
                MyGeneral.index = 3;
                flag = 1;
            }
            else if (msg.message == WM_LBUTTONDOWN && msg.x > 915 && msg.x < 1120 && msg.y>50 && msg.y < 350)//收到鼠标点击左键的信息时
            {
                putimage(0, 0, &imgBg4);
                MyGeneral.index = 4;
                flag = 1;
            }
            //else if (msg.message == WM_LBUTTONUP /*&& flag*/)//收到鼠标左键松开的信息时
            //{
            //    continue;
            //}
            else if (flag !=0&& msg.message == WM_LBUTTONDOWN && msg.x > 850 && msg.x < 1010 && msg.y>485 && msg.y < 655)
            {
                return;
            }
          
        }
    }
}

void GeneralLocation()//武将位置
{
    BeginBatchDraw();
    putimage(0, 0, &imgGame);
    int ret = MyGeneral.index;
    for (int i = 0; i < 4; i++)
    {
        if (ret == i+1)
        {
            putimage(955, 620, &imgGeneral[i]);
        }
    }
    int x = 0;
    while (1)
    {
        x = rand() % 4 + 1;
        if (x != ret)
        {
            putimage(955, 195, &imgGeneral[x-1]);
            break;
        }
    }
    int y = 0;
    while (1)
    {
        y = rand() % 4 + 1;
        if (y != ret&&y!=x)
        {
            putimage(0, 70, &imgGeneral[y-1]);
            break;
        }
    }
    int z = 0;
    while (1)
    {
        z = rand() % 4 + 1;
        if (z != ret && z!=x && z!=y)
        {
            putimage(245,10, &imgGeneral[z-1]);
            break;
        }
    }
     EndBatchDraw();
}

void startGame()
{
    for (int k = 0; k < 4; k++)
    {
        Generals[k].Cards = (cards*)malloc(sizeof(cards));
        Generals[k].Cards->index = 0;
        Generals[k].Cards->next = NULL;
        for (int i = 0; i < 4; i++)
        {
            struct cards* Card = (cards*)malloc(sizeof(cards));
            Card->index = rand() % 4 + 1;
            Card->next = Generals[k].Cards->next;
            Generals[k].Cards->next = Card;
        }
        Generals[k].cardCount = 4;
    }
    putimage(750, 620, &imgAbandon);
}



void touchCards()
{
    for (int i = 0; i < 2; i++)
    {
        struct cards* Card = (cards*)malloc(sizeof(cards));
        Card->index = rand() % 4 + 1;
        Card->next = Generals[whoseRound].Cards->next;
        Generals[whoseRound].Cards->next = Card;
    }
    Generals[whoseRound].cardCount += 2;
}

void drawText()//渲染字体
{
    
    char bloodText[20];
    putimage(1020, 570, &imgText[0]);
    putimage(1020, 150, &imgText[1]);
    putimage(280, 350, &imgText[2]);
    putimage(20, 400, &imgText[3]);
    sprintf_s(bloodText, sizeof(bloodText), "血量 ：%d", Generals[0].blood);
    outtextxy(1020, 570, bloodText);//输出字体
    sprintf_s(bloodText, sizeof(bloodText), "血量 ：%d", Generals[1].blood);
    outtextxy(1020, 150, bloodText);//输出字体
    sprintf_s(bloodText, sizeof(bloodText), "血量 ：%d", Generals[2].blood);
    outtextxy(280, 350, bloodText);//输出字体
    sprintf_s(bloodText, sizeof(bloodText), "血量 ：%d", Generals[3].blood);
    outtextxy(20, 400, bloodText);//输出字体
}

void display()
{
    putimage(0, 700, &imgGame2);
    struct cards* p = p = Generals[0].Cards->next;
    int z = 0;
    while (p != NULL)
    {
        putimage(0 + z * 140, 720, &imgCards[p->index - 1]);
        z++;
        p = p->next;
    }
}

void deleteCard()
{
    if (MyGeneral.q->next != NULL)
    {
        struct cards* n = MyGeneral.q->next;
        MyGeneral.q->next = n->next;
        putimage(500, 350, &imgCards[n->index - 1]);

        free(n);
    }
    Generals[0].cardCount--;
}

void guardCards()//杀和闪
{
    if (Generals[0].currentCard->index == 1 && MyGeneral.goal != 1)
    {
        deleteCard();
       
        struct cards* q = Generals[MyGeneral.goal - 1].Cards;
        while (q->next != NULL)
        {
            if (q->next->index == 2)
            {
                break;
            }
            q = q->next;
        }
        if (q->next == NULL)
        {
            Generals[MyGeneral.goal - 1].blood = Generals[MyGeneral.goal - 1].blood-(Generals[0].flag?2:1);
            Generals[0].flag = false;
        }
        else
        {
            struct cards* n = q->next;
            q->next = n->next;
            putimage(500, 350, &imgCards[n->index - 1]);
            free(n);
            Generals[0].flag = false;
        }
        
    }//杀
    

}



void peachCard()//桃
{
    if (Generals[0].currentCard->index == 3 && MyGeneral.goal != 1)
    {
        if (Generals[MyGeneral.goal - 1].blood == 0)
        {
            deleteCard();
           
            Generals[MyGeneral.goal - 1].blood += 1;
        }
            return;
      
    }
    else if (Generals[0].currentCard->index == 3 && MyGeneral.goal == 1)
    {
        if (Generals[0].blood < 4)
        {
            deleteCard();
            Generals[0].blood += 1;
        }
        return;
    }
    return;
}

void alcoholicCard()//酒
{
    if (Generals[0].currentCard->index == 4 && MyGeneral.goal == 1 && Generals[MyGeneral.goal - 1].blood == 0)
    {
        deleteCard();
        Generals[MyGeneral.goal - 1].blood += 1;
        return;
    }
    else if (Generals[0].currentCard->index == 4 && MyGeneral.goal == 1)
    {
        deleteCard();
        Generals[0].flag = true;
        return;
    }
    return;

}

void playCards()
{
   
    MyGeneral.k = 0;
    ExMessage msg;
    while (1)
    {
        if (peekmessage(&msg))
        {
            if (msg.message == WM_LBUTTONDOWN && msg.x < 140 && msg.x>0 && msg.y > 720)
            {
                MyGeneral.k = 0;
            }
            else if (msg.message == WM_LBUTTONDOWN && msg.x > 140 && msg.x < 280 && msg.y > 720)
            {
                MyGeneral.k = 1;
            }
            else if (msg.message == WM_LBUTTONDOWN && msg.x > 280 && msg.x < 420 && msg.y > 720)
            {
                MyGeneral.k = 2;
            }
            else if (msg.message == WM_LBUTTONDOWN && msg.x > 420 && msg.x < 560 && msg.y > 720)
            {
                MyGeneral.k = 3;
            }
            else if (msg.message == WM_LBUTTONDOWN && msg.x > 560 && msg.x < 700 && msg.y > 720)
            {
                MyGeneral.k = 4;
            }
            else if (msg.message == WM_LBUTTONDOWN && msg.x > 700 && msg.x < 840 && msg.y > 720)
            {
                MyGeneral.k = 5;
            }
            //if (msg.message == WM_LBUTTONDOWN)
            //{
            //    if (msg.y > 720)
            //    {
            //        int temp_k = (msg.x) / 140;
            //        if (temp_k >= 0 && temp_k < 6) {  // 假设有6个卡片区域
            //            MyGeneral.k = temp_k;
            //        }
            //    }
            //}
            else if (msg.message == WM_LBUTTONDOWN && msg.x > 955 && msg.x < 1200 && msg.y>620 && msg.y < 900)//收到鼠标点击左键的信息时
            {
                MyGeneral.goal = 1;
                break;
            }
            else if (msg.message == WM_LBUTTONDOWN && msg.x > 955 && msg.x < 1200 && msg.y>195 && msg.y < 495)//收到鼠标点击左键的信息时
            {
                MyGeneral.goal = 2;
                break;
            }
            else if (msg.message == WM_LBUTTONDOWN && msg.x > 245 && msg.x < 465 && msg.y>10 && msg.y < 310)//收到鼠标点击左键的信息时
            {
                MyGeneral.goal = 3;
                break;
            }
            else if (msg.message == WM_LBUTTONDOWN && msg.x > 0 && msg.x < 220 && msg.y>70 && msg.y < 370)//收到鼠标点击左键的信息时
            {
                
                MyGeneral.goal = 4;
             
                break;
            }
        }
    }
   
   /* struct cards* q = Generals[0].Cards;
    while (k != 0)
    {
        k--;
        q = q->next;
    }
    putimage(500, 350, &imgCards[q->next->index - 1]);
    struct cards* n = q->next;
    q->next = n->next;
    free(n);*/

    MyGeneral.q = Generals[0].Cards;
    while (MyGeneral.k != 0 && MyGeneral.q->next != NULL)
    {
        MyGeneral.k--;
        MyGeneral.q = MyGeneral.q->next;
    }
    Generals[0].currentCard = (cards*)malloc(sizeof(cards));
    Generals[0].currentCard->index = MyGeneral.q->next->index;
    Generals[0].currentCard->next = NULL;
    peachCard();
    guardCards();
    alcoholicCard();
    /*if (MyGeneral.q->next != NULL)
    {
        struct cards* n = MyGeneral.q->next;
        MyGeneral.q->next = n->next;
        putimage(500, 350, &imgCards[n->index - 1]);
        
        free(n);
    }*/
    display();
    
 }

 
void myCard()//我的回合
{
    if (Generals[0].start == true)
    {
        touchCards();//摸牌
        Generals[0].start = false;
    }
    display();
    ExMessage msg;
    while (1)
    {
        if (peekmessage(&msg))
        {
            if (msg.message == WM_LBUTTONDOWN && msg.x > 750 && msg.x < 910 && msg.y > 620 && msg.y < 660)
            {
                if (Generals[0].cardCount <= Generals[0].blood)
                {
                    Generals[0].round = false;
                    return;
                }
                else
                {
                    for (int i = 0; i < Generals[0].cardCount - Generals[0].blood; i++)
                    {
                        int k = -1;
                        ExMessage msg;
                        while (1)
                        {
                            if (peekmessage(&msg))
                            {
                                // 检查是否点击了某张卡片
                                if (msg.message == WM_LBUTTONDOWN)
                                {
                                    if (msg.y > 720)
                                    {
                                        k = (msg.x - 0) / 140; // 根据点击的x坐标计算k的值
                                    }
                                }
                                else if (msg.message == WM_LBUTTONUP)
                                {
                                    if (k != -1 && k < 6) // 确保有卡片被选中
                                    {
                                        break; // 跳出内部循环
                                    }
                                }
                            }
                        }
                        struct cards* q = Generals[0].Cards;
                        while (k != 0)
                        {
                            k--;
                            q = q->next;
                        }
                        putimage(500, 350, &imgCards[q->next->index - 1]);
                        struct cards* n = q->next;
                        q->next = n->next;
                        free(n);
                        display();
                    }
                    Generals[0].cardCount = Generals[0].blood;
                    Generals[0].round = false;
                    return;
                }
            }//弃牌
            else if (msg.message == WM_LBUTTONDOWN)
            {
                playCards();
                return;
            }//出牌
        }
    }
    
}

void elseDeleteCard()
{
  

        if (Generals[whoseRound].q->next != NULL)
        {
            struct cards* n = Generals[whoseRound].q->next;
            Generals[whoseRound].q->next = n->next;
            putimage(500, 350, &imgCards[n->index - 1]);

            free(n);
        }
        Generals[whoseRound].cardCount--;

}

void elseGuardCards()//杀和闪
{
    if (Generals[whoseRound].q->next->index == 1)
    {
        elseDeleteCard();

        struct cards* q = Generals[Generals[whoseRound].goal - 1].Cards;
        while (q->next != NULL)
        {
            if (q->next->index == 2)
            {
                break;
            }
            q = q->next;
        }
        if (q->next == NULL)
        {
            Generals[Generals[whoseRound].goal - 1].blood = Generals[Generals[whoseRound].goal - 1].blood - (Generals[whoseRound].flag ? 2 : 1);
            Generals[whoseRound].flag = false;
        }
        else
        {
            struct cards* n = q->next;
            q->next = n->next;
            putimage(500, 350, &imgCards[n->index - 1]);
            free(n);
            Generals[whoseRound].flag = false;
        }

    }//杀


}

void alcoKillAndnormalKill()
{
    while (1)
    {
        Generals[whoseRound].q = Generals[whoseRound].Cards;
        while (Generals[whoseRound].q->next != NULL)
        {
            if (Generals[whoseRound].q->next->index == 1)
            {
                Generals[whoseRound].ifKill = true;
                break;
            }
            Generals[whoseRound].q = Generals[whoseRound].q->next;
        }//找杀

        if (Generals[whoseRound].ifKill == false)
        {
            return;
        }

        Generals[whoseRound].q = Generals[whoseRound].Cards;
        while (Generals[whoseRound].q->next != NULL)
        {

            if (Generals[whoseRound].q->next->index == 4 && Generals[whoseRound].ifKill == true)
            {
                elseDeleteCard();
                Generals[whoseRound].flag = true;
                break;
            }
            Generals[whoseRound].q = Generals[whoseRound].q->next;
        }//找酒
        while (1)
        {
            Generals[whoseRound].goal = rand() % 4 + 1;
            if (Generals[whoseRound].goal - 1 != whoseRound)
            {
                break;
            }
        }
        Generals[whoseRound].q = Generals[whoseRound].Cards;
        while (Generals[whoseRound].q->next != NULL)
        {
            if (Generals[whoseRound].q->next->index == 1)
            {
                elseGuardCards();
                break;
            }
            Generals[whoseRound].q = Generals[whoseRound].q->next;
        }
    }
    

}
void elseCard()
{
    if (Generals[whoseRound].start == true)
    {
        touchCards();//摸牌
        Generals[whoseRound].start = false;
    }

    alcoKillAndnormalKill();//酒杀和普通杀
    
    Generals[whoseRound].q = Generals[whoseRound].Cards;
    while (Generals[whoseRound].q->next != NULL)
    {

        if (Generals[whoseRound].q->next->index == 3 && Generals[whoseRound].blood < 4)
        {
            elseDeleteCard();
            Generals[whoseRound].blood++;
        }
        Generals[whoseRound].q = Generals[whoseRound].q->next;
    }//桃
    
    if (Generals[whoseRound].cardCount > Generals[whoseRound].blood)
    {
        for (int i = 0; i < Generals[whoseRound].cardCount - Generals[whoseRound].blood; i++)
        {
            Generals[whoseRound].q = Generals[whoseRound].Cards;
            elseDeleteCard();
        }
    }//弃牌

    Generals[whoseRound].round = false;
}

int main()
{
    gameInit();
    startUI();
    GeneralLocation();
    startGame();
    
    
    display();
   
    while (1)
    {
        Generals[whoseRound].start = true;
        Generals[whoseRound].round = true;
        if (whoseRound == 0)
        {
            while (1)
            {
                drawText();
                /* playCards();*/
                myCard();
                if (Generals[0].round == false)
                {
                    break;
                }
            }
        }
        else
        {
            
                drawText();
                elseCard();
                if (Generals[whoseRound].round == false)
                {
                    break;
                }
          
        }
        if (whoseRound < 3)
        {
            whoseRound++;
        }
        else
        {
            whoseRound = 0;
        }
    }
    
    
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
