#pragma once

#define RANDOM_COLOR { unsigned char(rand() % 256), unsigned char(rand() % 256), unsigned char(rand() % 256), 255 }

namespace raylibOverlay
{
	struct Color
	{
		unsigned char r = 0, g = 0, b = 0, a = 0;

		Color();
		Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	};

	struct Vector2
	{
		float x = 0, y = 0;
	};

	//Initializer
	void InitWindow(int width, int height, const char* name);

	//FPS
	void SetFPS(int fps);
	void DrawFPS();

	//Checks
	bool WindowShouldClose();

	//Draw
	void BeginDrawing();
	void ClearBackground(Color color);
	void DrawCircle(Vector2 position, int radius, Color color);
	void DrawTextV(const char* text, raylibOverlay::Vector2 position, int fontSize, raylibOverlay::Color color);
	void EndDrawing();

	//Input
	bool IsKeyPressed(int key);
	bool IsKeyDown(int key);

	//Time
	double GetTime();
	double SecondsPassedSinceTime(double time);

	/// <summary>
	/// Get system clock time in milliseconds
	/// </summary>
	/// <returns>Clock time in milliseconds</returns>
	int GetClockTime();

	//Collision detection
	bool CheckCollisionCircles(Vector2 center1, float radius1, Vector2 center2, float radius2);

	//Clear
	void CloseWindow();
}