#pragma once
#include "raylibOverlay.h"

#define PLAYER_SIZE 50
#define BOMB_SIZE 25
#define BOOST_SPEED 15
#define NORMAL_SPEED 5
#define INITIAL_HEALTH 4

class Entity
{
protected:
	float speed = 5; 
	int id;
	raylibOverlay::Vector2 position;
	raylibOverlay::Color color;
	bool isDead = false;

	/// <summary>
	/// Type of entity
	/// 0 = player, 1 = bomb
	/// </summary>
	int type = -1;

public:
	virtual void Update() = 0;
	virtual void Draw() = 0;

	int GetID();

	/// <summary>
	/// Get type of entity
	/// 0 = player, 1 = bomb
	/// </summary>
	int GetType();
	void SetIsDead(bool isDead_);
	bool GetIsDead();
	raylibOverlay::Vector2 GetPosition();
	raylibOverlay::Color GetColor();
};

class Player : public Entity
{
	int health = INITIAL_HEALTH;
public:
	void Init(int id_, raylibOverlay::Vector2 position_, raylibOverlay::Color color_);
	void UpdateInput(raylibOverlay::Vector2 input, bool isBoosting);
	void Update() override;
	void Draw() override;

	void RemoveHealth(int amount);
	int GetHealth();
	void ClearHealth();
};

class Bomb : public Entity
{
public:
	void Init(int id_, raylibOverlay::Vector2 position_, raylibOverlay::Color color_);
	void Update() override;
	void Draw() override;
};