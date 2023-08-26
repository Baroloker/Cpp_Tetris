#pragma once

#include <vector>
#include <graphics.h>
#include "Block.h"


class Tetris
{
public:
	// ���캯��
	Tetris(int rows, int cols, int left, int top, int blockSize);
	void init();
	void play();

private:
	void keyEvent();
	void updateWindow();
	int getDelay(); // ������һ��ʱ��
	void drop();
	void clearLine();
	void moveLeftRight(int offset);
	void rotate(); // ��ת
	void drawScore();
	void checkOver(); // �����Ϸ�Ƿ����
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
	IMAGE imgBg; // ����ͼƬ
	IMAGE imgOver; // ���ȳ�ʼ��
				   // ��������ȸ���������������Ԫ�ظ�ֵ
				   // ��ֵΪ�����ֵ
	IMAGE imgWin;

	Block* curBlock;
	Block* nextBlock; // Ԥ�淽��
	Block backBlock; // ��ǰ����������еı���

	int score;
	int level;
	int lineCount; // Ŀǰ�Ѿ�����������
	int highestScore; // ��߷�
	bool gameOver;
};

