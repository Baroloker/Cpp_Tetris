#include "Tetris.h"
#include "Block.h"
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <vector>
#include <graphics.h>
#include <conio.h>
#include <fstream>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#define MAX_LEVEL 5;
#define RECORDER_FILE "recorder.txt"

//const int SPEED_NORMAL = 500;
const int SPEED_NORMAL[5] = { 500, 300, 150, 100, 80 };
const int SPEED_QUICK = 30;

Tetris::Tetris(int rows, int cols, int left, int top, int blockSize)
{
	this->rows = rows;
	this->cols = cols;
	this->leftMargin = left;
	this->topMargin = top;
	this->blockSize = blockSize;

	for (int i = 0; i < rows; i++) {
		std::vector<int> mapRow;
		for (int j = 0; j < cols; j++) {
			mapRow.push_back(0);
		}
		map.push_back(mapRow);
	}
}

void Tetris::init()
{
	//mciSendString("play res/bg.mp3 repeat", 0, 0, 0);
	delay = SPEED_NORMAL[0];

	// 配置随机种子
	srand(time(NULL)); // 随机产生

	// 创建游戏窗口
	initgraph(938, 896);

	// 加载背景图片

	loadimage(&imgBg, "res/bg2.png");
	loadimage(&imgWin, "res/win.png");
	loadimage(&imgOver, "res/over.png");

	// 初始化游戏区中的数据
	/*char data[20][10];*/
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			map[i][j] = 0; // 没有东西
		}
	}
	// 方块能走的区域

	score = 0;
	level = 1;
	lineCount = 0;

	//highestScore = 0;
	// 初始化最高分
	std::ifstream file(RECORDER_FILE);
	if (!file.is_open()) {
		std::cout << RECORDER_FILE << "打开失败" << std::endl;
		highestScore = 0;
	}
	else {
		file >> highestScore;
	}
	file.close(); // 关闭文件

	gameOver = false;
}

void Tetris::play()
{
	init();

	// 方块渲染
	nextBlock = new Block; // 类似于 temp 作用 先开辟一个 Block
	curBlock = nextBlock; // 第一个开辟的 Block 赋值给 cur
	nextBlock = new Block; // 预告 Block 再开一个新的

	int timer = 0;
	while (1) {
		checkOver();

		// 接受用户输入
		keyEvent();

		timer += getDelay();
		if (timer > delay) {
			timer = 0;
			drop();
			// 渲染游戏画面
			update = true;
		}

		if (update) {
			update = false;
			updateWindow();

			clearLine();
		}

		if (gameOver) {

			saveScore();

			// 更新游戏结束界面
			displayOver();

			system("pause");

			init(); // 重新开始
		}
	}
}

void Tetris::keyEvent() // 实现左右移动
{
	unsigned char ch; // 0 ... 255

	bool rotateFlag = false;

	int dx = 0;

	if (_kbhit()) {
		ch = _getch();

		// a w s d
		// 如果按下方向键，会自动返回两个字符
		// 上 先后返回 244 72
		// 下 先后返回 224 80
		// 左 先后返回 244 75
		// 右 先后返回 244 77

		if (ch == 224) {
			ch = _getch();
			switch(ch) {
			case 72:
				rotateFlag = true;
				break;
			case 80:
				delay = SPEED_QUICK;
				break;
			case 75:
				dx = -1;
				break;
			case 77:
				dx = 1;
				break;
			default:
				break;
			}
		}
	}

	if (rotateFlag) {
		// 旋转处理
		rotate();
		update = true; // 直接更新画面 因为操作
	}

	if (dx != 0) {
		// 实现左右移动
		moveLeftRight(dx);
		update = true; // 直接更新画面
	}
	
}

void Tetris::updateWindow()
{
	putimage(0, 0, &imgBg);

	//测试方块
	/*Block block;
	block.draw(leftMargin, topMargin);*/
		
	BeginBatchDraw(); // 一次性画完防止闪烁

	IMAGE** imgs = Block::getImages(); // 获取二维数组

	for (int i = 0; i < rows; i ++) {
		for (int j = 0; j < cols; j++) {
			if (map[i][j] == 0) continue;

			int x = j * blockSize + leftMargin;
			int y = i * blockSize + topMargin;
			putimage(x, y, imgs[map[i][j] - 1]); // 画
			// 看来 map[i][j] 存储的是 imgs 的类型
		}
	}

	curBlock->draw(leftMargin, topMargin);
	nextBlock->draw(689, 150);

	drawScore(); // 绘制当前分数

	EndBatchDraw();
}

// 第一次调用返回 0
// 返回距离上一次调用间隔时间
int Tetris::getDelay()
{
	static unsigned long long lastTime = 0;
	unsigned long long currentTime = GetTickCount();

	if (lastTime == 0) {
		lastTime = currentTime;
		return 0;
	}
	else {
		int ret = currentTime - lastTime;
		lastTime = currentTime;
		return ret;
	}
}

void Tetris::drop()
{
	backBlock = *curBlock; // * 解引用
	curBlock->drop();

	if (!curBlock->BlockInMap(map)) {
		// 固化方块
		backBlock.solidify(map);
		delete curBlock;
		curBlock = nextBlock;
		nextBlock = new Block;
	}

	delay = SPEED_NORMAL[level - 1];
}

void Tetris::clearLine()
{
	int lines = 0;
	int k = rows - 1; // 存储数据行数
	for (int i = rows - 1; i > 0; i--) {
		// 检查第 i 行是否满了
		int count = 0;
		for (int j = 0; j < cols; j++) {
			if (map[i][j]) {
				count++;
			}
			map[k][j] = map[i][j]; // 一边扫描一边存储 
		}

		if (count < cols) { // 不是满行
			k--; // 继续往上判断
		}
		else {
			lines++;
		}
	}

	if (lines > 0) {
		// 计算得分
		int addScore[4] = { 10, 30, 60, 80 };
		score += addScore[lines - 1];

		mciSendString("play res/xiaochu1.mp3", 0, 0, 0);
		update = true;

		level = (score + 99) / 100;
		if (level > 5) {
			gameOver = true;
		}
		lineCount += lines;
	}
}

void Tetris::moveLeftRight(int offset)
{
	backBlock = *curBlock;
	curBlock->moveLeftRight(offset);
	
	// 考虑边界
	if (!curBlock->BlockInMap(map)) {
		*curBlock = backBlock;
	}
}

void Tetris::rotate()
{
	if (curBlock->getBlockType() == 7) return; // 该形状旋不旋转都一样

	backBlock = *curBlock;
	curBlock->rotate();

	if (!curBlock->BlockInMap(map)) {
		*curBlock = backBlock;
	}
}

void Tetris::drawScore()
{
	char scoreText[32];
	sprintf_s(scoreText, sizeof(scoreText), "%d", score);

	setcolor(RGB(180, 180, 180));

	LOGFONT f;
	gettextstyle(&f); // 获取
	f.lfHeight = 60;
	f.lfWidth = 30;
	f.lfQuality = ANTIALIASED_QUALITY; // 设置字体为锯齿状效果
	strcpy_s(f.lfFaceName, sizeof(f.lfFaceName), _T("Segoe UI Black"));
	
	settextstyle(&f); // 启用

	setbkmode(TRANSPARENT); // 设置字体背景为透明效果

	outtextxy(670, 727, scoreText);

	sprintf_s(scoreText, sizeof(scoreText), "%d", lineCount);
	gettextstyle(&f);
	int xPos = 244 - f.lfWidth * strlen(scoreText); // 动态计算边界
	outtextxy(xPos - 20, 817, scoreText);

	// 绘制当前是第几关
	sprintf_s(scoreText, sizeof(scoreText), "%d", level);
	outtextxy(224 - 30, 727, scoreText);

	// 绘制最高分
	sprintf_s(scoreText, sizeof(scoreText), "%d", highestScore);
	outtextxy(670, 817, scoreText);
}

void Tetris::checkOver()
{
	gameOver = (curBlock->BlockInMap(map) == false); 
}

void Tetris::saveScore()
{
	if (score > highestScore) {
		highestScore = score;

		std::ofstream file(RECORDER_FILE);
		file << highestScore;
		file.close();
	}
}

void Tetris::displayOver()
{
	//mciSendString("stop res/bg.mp3", 0, 0, 0);

	if (level <= 5) {
		putimage(262, 361, &imgOver);
		//mciSendString("play res/over.mp3", 0, 0, 0);
	}
	else if (level >= 5) {
		putimage(262, 361, &imgWin);
		//mciSendString("play res/win.mp3", 0, 0, 0);
	}
}
