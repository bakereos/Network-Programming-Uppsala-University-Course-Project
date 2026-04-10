#pragma once
#include "raylibOverlay.h"
#include <deque> // Changed from <vector> to <deque>
#include <cmath>
#include "Client.h"

#define BOOST_SPEED 15
#define NORMAL_SPEED 5
#define BOMB_SIZE 25
#define PLAYER_SIZE 50
#define MAX_POSITION_HISTORY 60 // Max number of position samples to keep (e.g., 3 seconds at 20pps)

struct PlayerInputData {
    raylibOverlay::Vector2 input{ 0, 0 };
    bool isBoosting = false;
    double timestamp = 0;
};
struct PointAtTime
{
    raylibOverlay::Vector2 position;
    double time;
};

class Entity
{
    int id;
    int radius = PLAYER_SIZE;
    static inline float speed = NORMAL_SPEED; 
    bool isDead = false;
    raylibOverlay::Color color;
    std::deque<PointAtTime> positionHistory;
    int MAX_HISTORY_SIZE = 100; 

public:
   

    raylibOverlay::Vector2 position;
    bool isLocalPlayer = false;
    void Init(int id_, raylibOverlay::Vector2 position_, raylibOverlay::Color color_, bool islocalPlayer_, int radius_);
    void Init(int id_, raylibOverlay::Vector2 position_, raylibOverlay::Color color_, int radius_);
    void Update();
    void Draw();
    void AddPosition(const raylibOverlay::Vector2& position_, double timestamp); 
    void SetPosition(const raylibOverlay::Vector2& position);
    const std::deque<PointAtTime>& GetPositionHistory() const; 
    int GetID() const; 
    void PredictInput(const PlayerInputData& inputData);
    void CorrectPrediction(raylibOverlay::Vector2 serverPosition);
    void SetIsDead(bool isDead);
    bool GetIsDead() const; 
    static float GetSpeed(); 
};

