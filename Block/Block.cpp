#include "Block.h"
#include <stdlib.h>

IMAGE* Block::imgs[7] = { NULL, };
int Block::size = 36; // 一块的大小 像素 36 * 36；

Block::Block()
{
	if (imgs[0] == NULL) {
		IMAGE imgTmp;
		loadimage(&imgTmp, "res/tiles.png");

		SetWorkingImage(&imgTmp);
		for (int i = 0; i < 7; i++) {
			imgs[i] = new IMAGE;
			getimage(imgs[i], i * size, 0, size, size); // 内存 切割点x 切割点y 宽度 高度
		}
		SetWorkingImage(); // 恢复工作区
	}

	int blocks[7][4] = {
		1,3,5,7, // I
		2,4,5,7, // Z1
		2,5,4,6, // Z2
		3,5,4,7, // T
		2,3,5,7, // L
		3,5,7,6, // J
		2,3,4,5, // 0
	};

	// 随机生成一种
	blockType = rand() % 7 + 1; // 1 + 0 ... 6 => 1 ... 7

	// 初始化 smallBlocks
	for (int i = 0; i < 4; i++) {
		int value = blocks[blockType - 1][i];
		smallBlocks[i].row = value / 2;
		smallBlocks[i].col = value % 2;
	}

	img = imgs[blockType - 1]; // img 指针指向 某个方块类型
							   // 便于传参绘制方块
}

void Block::drop()
{
	for (int i = 0; i < 4; i++) {
		smallBlocks[i].row++;
	}
}

void Block::moveLeftRight(int offset)
{
	for (int i = 0; i < 4; i++) {
		smallBlocks[i].col += offset;
	}
}

void Block::retate()
{
}

void Block::draw(int leftMargin, int topMargin)
{
	for (int i = 0; i < 4; i++) {
		int x = leftMargin + smallBlocks[i].col * size;
		int y = topMargin + smallBlocks[i].row * size;
		putimage(x, y, img);
	}
}

IMAGE** Block::getImages()
{
	return  imgs;
}

Block& Block::operator=(const Block& other)
{
	if (this == &other) return *this;

	this->blockType = other.blockType;

	for (int i = 0; i < 4; i++) {
		this->smallBlocks[i] = other.smallBlocks[i];
	}

	return *this;
}

void Block::rotate()
{
	Point p = smallBlocks[1]; // 旋转重心
	
	for (int i = 0; i < 4; i++) {
		Point temp = smallBlocks[i];
		smallBlocks[i].col = p.col - temp.row + p.row;
		smallBlocks[i].row = p.row + temp.col - p.col;
	}
}

bool Block::BlockInMap(const std::vector<std::vector<int>>& map)
{
	int rows = map.size();
	int cols = map[0].size();

	for (int i = 0; i < 4; i++) {
		if (smallBlocks[i].col < 0 || smallBlocks[i].col >= cols ||
			smallBlocks[i].row < 0 || smallBlocks[i].row >= rows ||
			map[smallBlocks[i].row][smallBlocks[i].col]) { // 不会覆盖亦有方块
			return false;
		}
	}
	return true;
}

void Block::solidify(std::vector<std::vector<int>>& map)
{
	for (int i = 0; i < 4; i++) {
		// 设置标记 固化 对应的位置
		map[smallBlocks[i].row][smallBlocks[i].col] = blockType;
	}
}

int Block::getBlockType()
{
	return blockType;
}
