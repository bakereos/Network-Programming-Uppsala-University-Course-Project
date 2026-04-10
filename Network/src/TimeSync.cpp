#include <chrono>
#include <algorithm>
#include "TimeSync.h"
// Constructor
TimeSyncer::TimeSyncer()
    : expectedTime(GetTime()), integrator(0.0f), totalDrift(0.0f) {}

// Called when receiving a server update
void TimeSyncer::OnServerUpdate(double serverTime) {
    double localTime = GetNow();
    double timeDifference = serverTime - localTime;

    // Exponential smoothing for drift correction
    integrator = integrator * 0.9f + timeDifference * 0.1f;

    // Apply a bounded adjustment to the total drift
    double adjustment = std::clamp(integrator * 0.05f, (double) - 0.5f, (double)0.5f);
    totalDrift += adjustment;

    // Update the expected time
    expectedTime = serverTime;
}

// Returns the current synchronized time
double TimeSyncer::GetNow() const {
    return GetTime() + totalDrift;
}

// Returns the high-resolution local time in milliseconds
double TimeSyncer::GetTime() {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(now.time_since_epoch());
    return duration.count();
}
