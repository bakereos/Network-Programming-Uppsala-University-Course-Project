#pragma once
#include "Client.h"
#include "Entity.h"
#include "Controller.h"
#include "TimeSync.h"
#include "chrono"

class Game {
  Client client;
  DataBuffer SendBuffer, ReceiveBuffer;
  TimeSyncer timeSyncer;
  std::vector<Entity> entities;
  Controller controller;
  bool gameEnded;
  bool lost;

 public:
  float startTime;
  void Init();
  void Update();
  void Draw();

  void Clear();

 private:
  void NetUpdate();

  void CheckInput();

  void DrawGameEndMenu();

  raylibOverlay::Vector2 Lerp(const PointAtTime& start, const PointAtTime& end, float alpha);

  raylibOverlay::Vector2 InterpolateEntityPosition(Entity& entity);

  void CreateEntity(int id, raylibOverlay::Vector2 position, raylibOverlay::Color color, int radius,bool isLocalPlayer);

  void ApplyLocalInputPrediction(const PlayerInputData& inputData);

  Entity* FindEntity(int id);


  void RemoveDeadEntity();
  void RemoveAllEntity();
};