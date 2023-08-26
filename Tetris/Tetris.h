#pragma once

#include <vector>
#include <graphics.h>
#include "Block.h"


class Tetris
{
public:
	// 构造函数
	Tetris(int rows, int cols, int left, int top, int blockSize);
	void init();
	void play();

private:
	void keyEvent();
	void updateWindow();
	int getDelay(); // 返回上一次时间
	void drop();
	void clearLine();
	void moveLeftRight(int offset);
	void rotate(); // 旋转
	void drawScore();
	void checkOver(); // 检查游戏是否结束
	void saveScore();
	void displayOver();

private:
	int delay;
	bool update;

	std::vector<std::vector<int>> map;
	int rows;
	int cols;
	int leftMargin;
	int topMargin;
	int blockSize;
	IMAGE imgBg; // 背景图片
	IMAGE imgOver; // 得先初始化
				   // 否则可能先给声明序列中其他元素赋值
				   // 该值为随机数值
	IMAGE imgWin;

	Block* curBlock;
	Block* nextBlock; // 预告方块
	Block backBlock; // 当前方框降落过程中的备份

	int score;
	int level;
	int lineCount; // 目前已经消除多少行
	int highestScore; // 最高分
	bool gameOver;
};

