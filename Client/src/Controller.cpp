#include "Controller.h"

bool Controller::PlayerInput()
{
	if (!enabled) return false;

	deltaInput = { 0, 0 };
	isBoosting = false;

	// A key
	if (raylibOverlay::IsKeyDown(65))
	{
		deltaInput.x--;
	}

	// S key
	if (raylibOverlay::IsKeyDown(68))
	{
		deltaInput.x++;
	}

	// W key
	if (raylibOverlay::IsKeyDown(87))
	{
		deltaInput.y--;
	}

	// S key
	if (raylibOverlay::IsKeyDown(83))
	{
		deltaInput.y++;
	}

	//boost
		if (raylibOverlay::IsKeyDown(32)) {
			isBoosting = true;
        }

	if (deltaInput.x != 0 || deltaInput.y != 0) return true;
	else return false;
}

raylibOverlay::Vector2 Controller::GetInput()
{
	return deltaInput;
}

void Controller::SetEnabled(bool enabled_)
{
	this->enabled = enabled_;
}

bool Controller::GetIsBoosting()
{
	return isBoosting;
}
