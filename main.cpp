#include <iostream>
#include "Tetris.h"
#include "Block.h"

int main(void) {
	Tetris game(20, 10, 263, 133, 36);
	game.play();
	return 0;
}