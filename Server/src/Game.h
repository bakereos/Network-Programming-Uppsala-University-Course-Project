#pragma once
#include "Server.h"
#include <vector>
#include <list>
#include "Entity.h"
#include <chrono>

#define STARTING_HEALTH 3
enum GameState
{
	Pause,
	Ended,
	Started,
	InProgress
};

class Game
{
	Server server;
	DataBuffer SendBuffer, ReceiveBuffer;

	std::vector<Entity*> entities;
	std::list<Player> players;
	std::list<Bomb> bombs;

	double spawnBombsCooldown = 5, lastSpawnedTime;
	int numOfBombsPerSpawn = 5;
	double frameStartTime;

	GameState gameState = Started;
	double pauseStartTime, pauseTime = 3;

public:
	void Init();
	void Update();
	void Draw();

	void Clear();

private:
	void NetUpdate();

	void DestroyAllBombs();

	void CollisionDetection();

	void UpdateGameState();
	void ClearPlayerHealth();

	void CreatePlayerEntity(int id);
	Player* FindPlayer(int id);

	Entity* CreateBombEntity(int id);
	void SpawnBombs();

	void RemoveDeadEntity();
};

