#include "chess.h"

int main() {

	InitWindow(CELL_SIZE * CELL_COUNT, CELL_SIZE * CELL_COUNT, "Chess");
	SetTargetFPS(60);
	Chess game;

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(BLACK);
			
		game.update();
		
		EndDrawing();
	}

	CloseWindow();
	return 0;
}