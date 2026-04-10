#include "raylibOverlay.h"
#include "raylib.h"
#include <string>
#include <chrono>

namespace raylibOverlay
{
	void InitWindow(int width, int height, const char* name)
	{
		RAYLIB_H::InitWindow(width, height, name);
	}

	void SetFPS(int fps)
	{
		RAYLIB_H::SetTargetFPS(fps);
	}

	void DrawFPS()
	{
		RAYLIB_H::DrawFPS(10, 10);
	}

	bool WindowShouldClose()
	{
		if (RAYLIB_H::WindowShouldClose())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	void BeginDrawing()
	{
		RAYLIB_H::BeginDrawing();
	}

	void ClearBackground(Color color)
	{
		RAYLIB_H::Color rColor;
		rColor.r = color.r;
		rColor.g = color.g;
		rColor.b = color.b;
		rColor.a = color.a;

		RAYLIB_H::ClearBackground(rColor);
	}

	void DrawCircle(Vector2 position, int radius, Color color)
	{
		RAYLIB_H::Color rColor;
		rColor.r = color.r;
		rColor.g = color.g;
		rColor.b = color.b;
		rColor.a = color.a;

		RAYLIB_H::DrawCircleV({ position.x, position.y }, (float)radius, rColor);
	}

	void DrawTextV(const char* text, raylibOverlay::Vector2 position, int fontSize, raylibOverlay::Color color)
	{
		RAYLIB_H::Color rColor;
		rColor.r = color.r;
		rColor.g = color.g;
		rColor.b = color.b;
		rColor.a = color.a;

		RAYLIB_H::DrawText(text, (int)position.x, (int)position.y, fontSize, rColor);
	}

	void EndDrawing()
	{
		RAYLIB_H::EndDrawing();
	}

	bool IsKeyPressed(int key)
	{
		if (RAYLIB_H::IsKeyPressed(key))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool IsKeyDown(int key)
	{
		if (RAYLIB_H::IsKeyDown(key))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	double GetTime()
	{
		return RAYLIB_H::GetTime();
	}

	double SecondsPassedSinceTime(double time)
	{
		return RAYLIB_H::GetTime() - time;
	}

	int GetClockTime()
	{
		using namespace std::chrono;

		// Get the current time point using high-resolution clock
        auto local = zoned_time{current_zone(), system_clock::now()};
		time_point currentTime =  local.get_sys_time();

		// Convert the time duration to milliseconds
		auto duration = duration_cast<milliseconds>(currentTime.time_since_epoch());

		// Extract the count (duration in milliseconds) from the duration
		return static_cast<int>(duration.count());
	}

	bool CheckCollisionCircles(Vector2 center1, float radius1, Vector2 center2, float radius2)
	{
		RAYLIB_H::Vector2 rCenter1 = { center1.x, center1.y };
		RAYLIB_H::Vector2 rCenter2 = { center2.x, center2.y };

		return RAYLIB_H::CheckCollisionCircles(rCenter1, radius1, rCenter2, radius2);
	}

	void CloseWindow()
	{
		RAYLIB_H::CloseWindow();
	}

	Color::Color()
	{
	}

	Color::Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}
}