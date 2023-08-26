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

	// �����������
	srand(time(NULL)); // �������

	// ������Ϸ����
	initgraph(938, 896);

	// ���ر���ͼƬ

	loadimage(&imgBg, "res/bg2.png");
	loadimage(&imgWin, "res/win.png");
	loadimage(&imgOver, "res/over.png");

	// ��ʼ����Ϸ���е�����
	/*char data[20][10];*/
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			map[i][j] = 0; // û�ж���
		}
	}
	// �������ߵ�����

	score = 0;
	level = 1;
	lineCount = 0;

	//highestScore = 0;
	// ��ʼ����߷�
	std::ifstream file(RECORDER_FILE);
	if (!file.is_open()) {
		std::cout << RECORDER_FILE << "��ʧ��" << std::endl;
		highestScore = 0;
	}
	else {
		file >> highestScore;
	}
	file.close(); // �ر��ļ�

	gameOver = false;
}

void Tetris::play()
{
	init();

	// ������Ⱦ
	nextBlock = new Block; // ������ temp ���� �ȿ���һ�� Block
	curBlock = nextBlock; // ��һ�����ٵ� Block ��ֵ�� cur
	nextBlock = new Block; // Ԥ�� Block �ٿ�һ���µ�

	int timer = 0;
	while (1) {
		checkOver();

		// �����û�����
		keyEvent();

		timer += getDelay();
		if (timer > delay) {
			timer = 0;
			drop();
			// ��Ⱦ��Ϸ����
			update = true;
		}

		if (update) {
			update = false;
			updateWindow();

			clearLine();
		}

		if (gameOver) {

			saveScore();

			// ������Ϸ��������
			displayOver();

			system("pause");

			init(); // ���¿�ʼ
		}
	}
}

void Tetris::keyEvent() // ʵ�������ƶ�
{
	unsigned char ch; // 0 ... 255

	bool rotateFlag = false;

	int dx = 0;

	if (_kbhit()) {
		ch = _getch();

		// a w s d
		// ������·���������Զ����������ַ�
		// �� �Ⱥ󷵻� 244 72
		// �� �Ⱥ󷵻� 224 80
		// �� �Ⱥ󷵻� 244 75
		// �� �Ⱥ󷵻� 244 77

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
		// ��ת����
		rotate();
		update = true; // ֱ�Ӹ��»��� ��Ϊ����
	}

	if (dx != 0) {
		// ʵ�������ƶ�
		moveLeftRight(dx);
		update = true; // ֱ�Ӹ��»���
	}
	
}

void Tetris::updateWindow()
{
	putimage(0, 0, &imgBg);

	//���Է���
	/*Block block;
	block.draw(leftMargin, topMargin);*/
		
	BeginBatchDraw(); // һ���Ի����ֹ��˸

	IMAGE** imgs = Block::getImages(); // ��ȡ��ά����

	for (int i = 0; i < rows; i ++) {
		for (int j = 0; j < cols; j++) {
			if (map[i][j] == 0) continue;

			int x = j * blockSize + leftMargin;
			int y = i * blockSize + topMargin;
			putimage(x, y, imgs[map[i][j] - 1]); // ��
			// ���� map[i][j] �洢���� imgs ������
		}
	}

	curBlock->draw(leftMargin, topMargin);
	nextBlock->draw(689, 150);

	drawScore(); // ���Ƶ�ǰ����

	EndBatchDraw();
}

// ��һ�ε��÷��� 0
// ���ؾ�����һ�ε��ü��ʱ��
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
	backBlock = *curBlock; // * ������
	curBlock->drop();

	if (!curBlock->BlockInMap(map)) {
		// �̻�����
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
	int k = rows - 1; // �洢��������
	for (int i = rows - 1; i > 0; i--) {
		// ���� i ���Ƿ�����
		int count = 0;
		for (int j = 0; j < cols; j++) {
			if (map[i][j]) {
				count++;
			}
			map[k][j] = map[i][j]; // һ��ɨ��һ�ߴ洢 
		}

		if (count < cols) { // ��������
			k--; // ���������ж�
		}
		else {
			lines++;
		}
	}

	if (lines > 0) {
		// ����÷�
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
	
	// ���Ǳ߽�
	if (!curBlock->BlockInMap(map)) {
		*curBlock = backBlock;
	}
}

void Tetris::rotate()
{
	if (curBlock->getBlockType() == 7) return; // ����״������ת��һ��

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
	gettextstyle(&f); // ��ȡ
	f.lfHeight = 60;
	f.lfWidth = 30;
	f.lfQuality = ANTIALIASED_QUALITY; // ��������Ϊ���״Ч��
	strcpy_s(f.lfFaceName, sizeof(f.lfFaceName), _T("Segoe UI Black"));
	
	settextstyle(&f); // ����

	setbkmode(TRANSPARENT); // �������屳��Ϊ͸��Ч��

	outtextxy(670, 727, scoreText);

	sprintf_s(scoreText, sizeof(scoreText), "%d", lineCount);
	gettextstyle(&f);
	int xPos = 244 - f.lfWidth * strlen(scoreText); // ��̬����߽�
	outtextxy(xPos - 20, 817, scoreText);

	// ���Ƶ�ǰ�ǵڼ���
	sprintf_s(scoreText, sizeof(scoreText), "%d", level);
	outtextxy(224 - 30, 727, scoreText);

	// ������߷�
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
