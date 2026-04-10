#include <cmath> 
#include "Game.h"
#include <algorithm>


void Game::Init()
{
	client.Init();
        startTime = (float)raylibOverlay::GetTime();
       
}

void Game::Update()
{
   
    int C = 67;
    int X = 88;
	if (raylibOverlay::IsKeyPressed(C))
	{
        if (client.connection.GetConnectionState() == Connected)
        {
            return;
        }
		client.InitConnection();
        
        controller.SetEnabled(true);
        gameEnded = false;
	}

    if (raylibOverlay::IsKeyPressed(X))
    {
        client.InitDisconnection();
    }


	for(int i = 0; i < 2; i++)
	{
		client.Receive(ReceiveBuffer);
	}
                                                                                                                                                        
	NetUpdate();
    RemoveDeadEntity();
	CheckInput();

	
		if (client.Transmit(SendBuffer))
		{
			SendBuffer.ClearBuffer();
		}
	
}

void Game::Draw()
{
	for (auto& entity : entities)
	{
		entity.Draw();
	}
    int Z = 90;
	raylibOverlay::DrawFPS();
        if (raylibOverlay::IsKeyDown(Z)) {
                client.DrawConnectionInfo({100, 20});
        }
    if (gameEnded)
    {
        
        DrawGameEndMenu();
    }
    else
    {
        
        float time = 20 - (float)raylibOverlay::SecondsPassedSinceTime(startTime);
        std::string text = "Time left: " + std::to_string(time);
        raylibOverlay::DrawTextV(text.c_str(), {30, 40},
                                 15, {0, 200, 0, 255});
    }
}

void Game::Clear()
{
	client.Clear();
}

bool ArePositionsEqual(const raylibOverlay::Vector2& pos1, const raylibOverlay::Vector2& pos2, float tolerance = 0.01f) {
    return std::fabs(pos1.x - pos2.x) < tolerance && std::fabs(pos1.y - pos2.y) < tolerance;
}

void Game::NetUpdate() {
        std::vector<double>::iterator doubleDataIter =
            ReceiveBuffer.doubleData.begin();
        std::vector<raylibOverlay::Vector2>::iterator vector2DataIter =
            ReceiveBuffer.vector2Data.begin();
        std::vector<raylibOverlay::Color>::iterator colorDataIter =
            ReceiveBuffer.colorData.begin();
        std::vector<bool>::iterator boolDataIter =
            ReceiveBuffer.boolData.begin();

        for (auto header : ReceiveBuffer.headers) {
                switch (header.command) {
                        case CreatePlayer:
                          CreateEntity(header.id, *vector2DataIter,
                                       *colorDataIter,PLAYER_SIZE,*boolDataIter);
                          vector2DataIter++;
                          colorDataIter++;
                          boolDataIter++;
                          break;
                        case CreateBomb:
                          CreateEntity(header.id, *vector2DataIter,
                                       {0,0,0,255},BOMB_SIZE,false);
                          vector2DataIter++;
                          colorDataIter++;
                          break;

                        case UpdatePosition: {
                            Entity* entityToUpdate = FindEntity(header.id);
                            if (entityToUpdate != nullptr) {
                                if (entityToUpdate->isLocalPlayer) {
                                 
                                    entityToUpdate->CorrectPrediction(*vector2DataIter);
                                }
                                else {
                              
                                    auto& positionHistory = entityToUpdate->GetPositionHistory();
                                    double currentTime = *doubleDataIter;

                                    if (!positionHistory.empty()) {
                                        const PointAtTime& lastRecorded = positionHistory.front();

                                       
                                        if (lastRecorded.time != currentTime ||
                                            !ArePositionsEqual(lastRecorded.position, *vector2DataIter)) {
                                            entityToUpdate->AddPosition(*vector2DataIter, currentTime);
                                        }
                                    }
                                    else {
                                        entityToUpdate->AddPosition(*vector2DataIter, currentTime);
                                    }
                                }
                            }
                            break;
                        }


                        
                        case DestroyBomb:
                        {
                            Entity* entityToKill = FindEntity(header.id);
                            if (entityToKill != nullptr) entityToKill->SetIsDead(true);
                            break;
                        }
                        case DestroyPlayer:
                        {
                            Entity* entityToKill = FindEntity(header.id);
                            if(entityToKill != nullptr) entityToKill->SetIsDead(true);
                            break;
                        }
                        case GameEnded:
                            controller.SetEnabled(false);
                            gameEnded = true;
                            startTime = (float)raylibOverlay::GetTime();
                            lost = *boolDataIter;
                            boolDataIter++;
                            break;
                        case GameStarted:
                            controller.SetEnabled(true);
                            gameEnded = false;
                            break;
                        case DestroyAllEntity:
                            RemoveAllEntity();
                            break;
                }
        }

        ReceiveBuffer.ClearBuffer();
        for (auto& entity : entities) {
            if (entity.GetPositionHistory().size() > 3 && !entity.isLocalPlayer) {
                entity.SetPosition(InterpolateEntityPosition(entity));
            }
        }
}

void Game::CheckInput()
{
	if (controller.PlayerInput())
	{
        PlayerInputData inputData;
            inputData.timestamp = client.timeSyncer.GetNow();
        inputData.input = controller.GetInput();
        inputData.isBoosting = controller.GetIsBoosting();

        ApplyLocalInputPrediction(inputData);
     
		SendBuffer.headers.push_back(DataHeader(MovementInput, -1));
		SendBuffer.vector2Data.push_back(controller.GetInput());
        SendBuffer.boolData.push_back(controller.GetIsBoosting());
	}
}

void Game::DrawGameEndMenu()
{
    if (lost)
    {
        raylibOverlay::DrawTextV("YOU DIED", { 200, 125 }, 70, { 255, 0, 0, 255 });
       
    }
    else
    {
        raylibOverlay::DrawTextV("YOU SURVIVED", { 200, 125 }, 70, { 0, 255, 0, 255 });
       
    }
}

void Game::ApplyLocalInputPrediction(const PlayerInputData& inputData) {
       
        for (auto& entity : entities) {
             if (entity.isLocalPlayer)
            {
               entity.PredictInput(inputData);
               break;
            }
                
        }
}

void Game::CreateEntity(int id, raylibOverlay::Vector2 position, raylibOverlay::Color color,int radius,bool isLocalPlayer)
{
	Entity newEntity;
    
				newEntity.Init(id, position, color, isLocalPlayer,radius);

	entities.push_back(newEntity);
}

Entity* Game::FindEntity(int id)
{
	for (auto& entity : entities)
	{
		if (entity.GetID() == id)
		{
			return &entity;
		}
	}

	return nullptr;
}

void Game::RemoveDeadEntity() {
        auto entityIterator = entities.begin();
        while (true) {
                if (entityIterator == entities.end()) break;

                if ((*entityIterator).GetIsDead()) {
                        entityIterator = entities.erase(entityIterator);
                } else {
                        entityIterator++;
                }
        }
}

void Game::RemoveAllEntity()
{
    entities.clear();
    SendBuffer.ClearBuffer();
    ReceiveBuffer.ClearBuffer();
}

raylibOverlay::Vector2 Game::InterpolateEntityPosition(Entity& entity) {
    const float INTERPOLATION_BUFFER_DELAY_SECONDS = 0.1f; 
    double renderTime = client.timeSyncer.GetNow() - INTERPOLATION_BUFFER_DELAY_SECONDS;
    const std::deque<PointAtTime>& positionHistory = entity.GetPositionHistory(); 

  
    
    if (positionHistory.size() < 2) {
        if (!positionHistory.empty()) return positionHistory.front().position;
        return entity.position; 
    }

   

    
    if (renderTime >= positionHistory.front().time) {
        return positionHistory.front().position;
    }

    
    if (renderTime <= positionHistory.back().time) {
        return positionHistory.back().position;
    }

   
    for (size_t i = 0; i < positionHistory.size() - 1; ++i) {
        const PointAtTime& p_newer = positionHistory[i];     
        const PointAtTime& p_older = positionHistory[i + 1];  

      
        if (renderTime <= p_newer.time && renderTime >= p_older.time) {
            float alpha = 0.0f;
            double timeDiff = p_newer.time - p_older.time;
            if (timeDiff > 0.00001) { 
                alpha = (float)((renderTime - p_older.time) / timeDiff);
            }
            
            alpha = std::clamp(alpha,0.0f,1.0f);
            return Lerp(p_older, p_newer, alpha); 
        }
    }

  
    return positionHistory.back().position; 
}


raylibOverlay::Vector2 Game::Lerp(const PointAtTime& start, const PointAtTime& end, float alpha) {
    float clampedAlpha = std::clamp(alpha, 0.0f, 1.0f);
    return { start.position.x + clampedAlpha * (end.position.x - start.position.x),
             start.position.y + clampedAlpha * (end.position.y - start.position.y) };

}




