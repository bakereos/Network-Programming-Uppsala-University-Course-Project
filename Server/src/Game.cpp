#include "Game.h"
#include "IDGenerator.h"


void Game::Init()
{
	server.Init();
        
}

void Game::Update()
{
	frameStartTime = server.timeSyncer.GetNow();
	//if (raylibOverlay::IsKeyPressed(82))
	server.Receive(ReceiveBuffer);
	NetUpdate();
	SpawnBombs();
	CollisionDetection();
	UpdateGameState();

	RemoveDeadEntity();
	server.CheckTimeout(ReceiveBuffer);

	//if (raylibOverlay::IsKeyPressed(83))
	{
		if (server.Transmit(SendBuffer))
		{
			SendBuffer.ClearBuffer();
		}
	}
}

void Game::Draw()
{
	for (auto& entity : entities)
	{
		entity->Draw();
	}

	raylibOverlay::DrawFPS();

	if (raylibOverlay::IsKeyDown(90))
	{
		server.DrawConnectionInfo({ 100, 20 });
	}
	if (gameState == InProgress)
	{

  float time =
            20 - (float)raylibOverlay::SecondsPassedSinceTime(server.startTime);
        std::string text = "Time left: " + std::to_string(time);
        raylibOverlay::DrawTextV(text.c_str(), {30, 40}, 15, {0, 200, 0, 255});
	}
}

void Game::NetUpdate()
{
	std::vector<raylibOverlay::Vector2>::iterator vector2DataIter = ReceiveBuffer.vector2Data.begin();
	std::vector<raylibOverlay::Color>::iterator colorDataIter = ReceiveBuffer.colorData.begin();
	std::vector<bool>::iterator boolDataIter = ReceiveBuffer.boolData.begin();

	for (auto header : ReceiveBuffer.headers)
	{
		switch (header.command)
		{
		case CreatePlayer:
			CreatePlayerEntity(header.id);
			break;
		case MovementInput:
		{
			Player* playerToUpdate = FindPlayer(header.id);

			if (playerToUpdate != nullptr)
			{
				playerToUpdate->UpdateInput(*vector2DataIter, *boolDataIter);
				vector2DataIter++;
				boolDataIter++;

				SendBuffer.headers.push_back(DataHeader(UpdatePosition, playerToUpdate->GetID()));
				SendBuffer.doubleData.push_back(server.timeSyncer.GetNow());
				SendBuffer.vector2Data.push_back(playerToUpdate->GetPosition());

			}
			break;
		}
		case DestroyPlayer:
			Player* playerToDestroy = FindPlayer(header.id);

			if (playerToDestroy != nullptr)
			{
				playerToDestroy->SetIsDead(true);
				
				server.DestroyReplicatedPlayer(playerToDestroy);
			}

			break;
		}
	}

	ReceiveBuffer.ClearBuffer();
}

void Game::DestroyAllBombs()
{
	for (auto& bomb : bombs)
	{
		bomb.SetIsDead(true);
		SendBuffer.headers.push_back(DataHeader(DestroyBomb, bomb.GetID()));
	}
}
void Game::CollisionDetection()
{
	for (auto& player : players)
	{
		for (auto& bomb : bombs)
		{
			if (raylibOverlay::CheckCollisionCircles(player.GetPosition(), PLAYER_SIZE, bomb.GetPosition(), BOMB_SIZE))
			{
				player.RemoveHealth(1);
				bomb.SetIsDead(true);
				SendBuffer.headers.push_back(DataHeader(DestroyBomb, bomb.GetID()));
			}
		}
		
	}
}

void Game::UpdateGameState()
{
	switch (gameState)
	{
	case Started:
		server.SendStartGameEvent();
		if (!server.Empty())
		{

		gameState = InProgress;
		}
		break;
	case InProgress:
		for (auto& player : players)
		{
			
			if (player.GetHealth() <= 0)
			{
				server.SendEndGameEvent(&player);
				pauseStartTime = server.timeSyncer.GetNow();

				ClearPlayerHealth();

				gameState = Ended;
				break;
			}
                        float stopwatch =
                            (float)raylibOverlay::SecondsPassedSinceTime(server.startTime);
                        if (stopwatch >= 20){
							if (players.back().GetHealth() >=
								players.front().GetHealth())
							{
                                server.SendEndGameEvent(&players.front());
								
								}
							else
							{
                                server.SendEndGameEvent(&players.back());
							}
                                pauseStartTime = server.timeSyncer.GetNow();
                                ClearPlayerHealth();
                                gameState = Ended;
								
                                break;
                        }


		}
		break;
	case Ended:
		if (raylibOverlay::SecondsPassedSinceTime(pauseStartTime) > pauseTime )
		{
			gameState = Started;
		}
		break;
	}
	
	
}

void Game::ClearPlayerHealth()
{
	for (auto& player : players)
	{
		player.ClearHealth();
	}
}

void Game::CreatePlayerEntity(int id)
{
	Player newPlayer;
	raylibOverlay::Vector2 newPosition = { float(rand() % 800), float(rand() % 450) };
	raylibOverlay::Color newColor = RANDOM_COLOR;
	newPlayer.Init(id, newPosition, newColor);
	
	players.push_back(newPlayer);
	entities.push_back(&players.back());

	server.ReplicateNewPlayerToClients(newPlayer, entities);
}

Player* Game::FindPlayer(int id)
{
	for (auto& player : players)
	{
		if (player.GetID() == id)
		{
			return &player;
		}
	}

	return nullptr;
}

Entity* Game::CreateBombEntity(int id)
{
	Bomb newBomb;
	raylibOverlay::Vector2 newPosition = { float(rand() % 800), float(rand() % 450) };
	raylibOverlay::Color newColor = RANDOM_COLOR;
	newBomb.Init(id, newPosition, newColor);

	bombs.push_back(newBomb);
	entities.push_back(&bombs.back());

	return &bombs.back();
}

void Game::SpawnBombs()
{
	if (gameState == Ended||players.size() == 0) return;

	if (raylibOverlay::SecondsPassedSinceTime(lastSpawnedTime) > spawnBombsCooldown)
	{
		std::vector<Entity*> newEntities;

		for (int i = 0; i < numOfBombsPerSpawn; i++)
		{
			Entity* newEntity = CreateBombEntity(IDGenerator::GenerateID());

			newEntities.push_back(newEntity);
		}

		server.ReplicateNewEntitiesToClients(newEntities);
		lastSpawnedTime = raylibOverlay::GetTime();
	}
}

void Game::RemoveDeadEntity()
{
	auto entityIterator = entities.begin();
	while (true)
	{
		if (entityIterator == entities.end()) break;

		if ((*entityIterator)->GetIsDead())
		{
			entityIterator = entities.erase(entityIterator);
		}
		else
		{
			entityIterator++;
		}
	}

	bombs.remove_if([](Bomb& a) { return a.GetIsDead(); });
}

void Game::Clear()
{
	server.Clear();
}
