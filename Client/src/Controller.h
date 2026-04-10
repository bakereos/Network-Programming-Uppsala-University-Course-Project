#pragma once
#include "raylibOverlay.h"

class Controller
{
	bool enabled = true;
	raylibOverlay::Vector2 deltaInput;
	bool isBoosting = false;

public:
	bool PlayerInput();
	raylibOverlay::Vector2 GetInput();
	void SetEnabled(bool enabled_);
	bool GetIsBoosting();
};

