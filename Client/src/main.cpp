#include "Game.h"
#include "raylibOverlay.h"

int main(void)
{
	const int windowWidth = 800;
	const int windowHeight = 450;
	raylibOverlay::InitWindow(windowWidth, windowHeight, "Client");
	raylibOverlay::SetFPS(60);

	Game game;
	game.Init();

	while (!raylibOverlay::WindowShouldClose())
	{
		// Update

		game.Update();

		//Draw
		raylibOverlay::BeginDrawing();
		raylibOverlay::ClearBackground(raylibOverlay::Color(245, 245, 245, 255));

		game.Draw();

		raylibOverlay::EndDrawing();
	}

	game.Clear();
	raylibOverlay::CloseWindow();
}