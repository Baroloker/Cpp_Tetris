#pragma once
#include <graphics.h>
#include <iostream>
#include <vector>

struct Point {
	int row;
	int col;
};

class Block
{
public:
	// 规定 Block 状态、行为
	Block();
	void drop();
	void moveLeftRight(int offset);
	void retate();
	void draw(int leftMargin, int topMargin);
	static IMAGE** getImages();
	Block& operator=(const Block& other);
	void rotate();

	bool BlockInMap(const std::vector<std::vector<int>>& map);

	void solidify(std::vector<std::vector<int>>& map);
	int getBlockType();

private:
	int blockType; // 方块类型
	Point smallBlocks[4];
	IMAGE* img;

	static IMAGE* imgs[7]; // 7 中颜色的方块 的指针
	static int size;
};

