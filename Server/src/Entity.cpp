#include "Entity.h"

void Player::Init(int id_, raylibOverlay::Vector2 position_, raylibOverlay::Color color_)
{
	type = 0;
	this->id = id_;
	this->position = position_;
	this->color = color_;
}

void Player::UpdateInput(raylibOverlay::Vector2 input, bool isBoosting)
{
	if (isBoosting)
	{
		speed = BOOST_SPEED;
	}
	else
	{
		speed = NORMAL_SPEED;
	}

	position.x += input.x * speed;
	position.y += input.y * speed;
}

void Player::Update()
{
}

void Player::Draw()
{
	raylibOverlay::DrawCircle(position, PLAYER_SIZE, color);
}


void Player::RemoveHealth(int amount)
{
	health -= amount;

	if (health < 0)
	{
		health = 0;
	}
}

int Player::GetHealth()
{
	return health;
}

void Player::ClearHealth()
{
	health = INITIAL_HEALTH;
}

int Entity::GetID()
{
	return id;
}

int Entity::GetType()
{
	return type;
}

void Entity::SetIsDead(bool isDead_)
{
	this->isDead = isDead_;
}

bool Entity::GetIsDead()
{
	return isDead;
}

raylibOverlay::Vector2 Entity::GetPosition()
{
	return position;
}

raylibOverlay::Color Entity::GetColor()
{
	return color;
}

void Bomb::Init(int id_, raylibOverlay::Vector2 position_, raylibOverlay::Color color_)
{
	type = 1;
	this->id = id_;
	this->position = position_;
	this->color = color_;
}

void Bomb::Update()
{
}

void Bomb::Draw()
{ raylibOverlay::DrawCircle(position, BOMB_SIZE, {0,0,0,255}); }
