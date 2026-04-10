#pragma once
  constexpr double SERVER_STEP_MS = 100.0f;

  class TimeSyncer {
  public:
      TimeSyncer();

      void OnServerUpdate(double serverTime); // Called when receiving a server update
      double GetNow() const;                 // Returns the current synchronized time

  private:
      static double GetTime();        // High-resolution local time
      double expectedTime;
      double integrator;
      double totalDrift;

  };
