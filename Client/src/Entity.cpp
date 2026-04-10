#include "Entity.h"
#include <cmath> // For std::sqrt if used by Distance

// Note: Client.h is included in Entity.h, which might lead to circular dependencies if Client.h also includes Entity.h.
// The `Client::inputMiscalculations++;` line in CorrectPrediction was problematic.

void Entity::Init(int id_, raylibOverlay::Vector2 position_, raylibOverlay::Color color_, int radius_)
{
    this->id = id_;
    this->position = position_;
    this->color = color_;
    this->radius = radius_;
    this->isLocalPlayer = false; // Default for this constructor
    this->isDead = false;
    this->positionHistory.clear();
}

void Entity::Init(int id_, raylibOverlay::Vector2 position_, raylibOverlay::Color color_, bool islocalPlayer_, int radius_) {
    this->id = id_;
    this->position = position_;
    this->color = color_;
    this->isLocalPlayer = islocalPlayer_;
    this->radius = radius_;
    this->isDead = false;
    this->positionHistory.clear();
}


void Entity::Draw() {
  
    raylibOverlay::DrawCircle(position, radius, color);
}

int Entity::GetID() const 
{
    return id;
}



void Entity::AddPosition(const raylibOverlay::Vector2& posit, double time)
{
    PointAtTime newPoint;
    newPoint.position = posit;
    newPoint.time = time;
    positionHistory.push_front(newPoint);
    if (positionHistory.size() > MAX_HISTORY_SIZE) {
        positionHistory.pop_back(); 
    }
}

void Entity::SetPosition(const raylibOverlay::Vector2& position_) {
    position = position_;
}

const std::deque<PointAtTime>& Entity::GetPositionHistory() const { 
    return positionHistory;
}

void Entity::PredictInput(const PlayerInputData& inputData) {
    float current_speed; 
    if (inputData.isBoosting)
    {
        current_speed = BOOST_SPEED;
    }
    else
    {
        current_speed = NORMAL_SPEED;
    }


    position.x += current_speed * inputData.input.x;
    position.y += current_speed * inputData.input.y;
}


float CalculateDistance(const raylibOverlay::Vector2& v1, const raylibOverlay::Vector2& v2)
{
    float dx = v2.x - v1.x;
    float dy = v2.y - v1.y;
    return std::sqrt(dx * dx + dy * dy);
}

void Entity::CorrectPrediction(raylibOverlay::Vector2 serverPosition) {
    float distance = CalculateDistance(this->position, serverPosition);
    if (distance > 0.1f) {
        SetPosition(serverPosition);
    }
}

void Entity::SetIsDead(bool isDead_) {
    isDead = isDead_;
}

bool Entity::GetIsDead() const {
    return isDead;
}

float Entity::GetSpeed() { 
    return NORMAL_SPEED;
}

