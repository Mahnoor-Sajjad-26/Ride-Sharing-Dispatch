#ifndef DISPATCH_ENGINE_H
#define DISPATCH_ENGINE_H

#include "City.h"
#include "Driver.h"
#include "Trip.h"

class DispatchEngine {
private:
    // Cross-zone penalty multiplier (50% extra)
    static const int CROSS_ZONE_PENALTY = 50;

public:
    DispatchEngine();

    // Find nearest available driver to pickup location
    // Returns driver index or -1 if none available
    // pickupZoneId is used for cross-zone penalty calculation
    int findNearestDriver(City& city, Driver* drivers, int driverCount,
                          int pickupLocationId, int pickupZoneId);

    // Calculate effective distance (with cross-zone penalty)
    int calculateEffectiveDistance(int actualDistance, int driverZone, int pickupZone);
};

#endif
