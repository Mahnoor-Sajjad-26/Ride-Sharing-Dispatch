#include "DispatchEngine.h"
#include <climits>
#include <iostream>
using namespace std;

DispatchEngine::DispatchEngine() {}

int DispatchEngine::calculateEffectiveDistance(int actualDistance, int driverZone, int pickupZone) {
    if (driverZone != pickupZone) {
        // Apply cross-zone penalty (50% extra)
        return actualDistance + (actualDistance * CROSS_ZONE_PENALTY / 100);
    }
    return actualDistance;
}

int DispatchEngine::findNearestDriver(City& city, Driver* drivers, int driverCount,
                                       int pickupLocationId, int pickupZoneId) {
    int nearestDriverIndex = -1;
    int minEffectiveDistance = INT_MAX;

    for (int i = 0; i < driverCount; i++) {
        // Only consider available drivers
        if (drivers[i].getStatus() != DRIVER_AVAILABLE) {
            continue;
        }

        // Get actual distance from driver to pickup
        int actualDistance = city.getDistance(drivers[i].getCurrentLocationId(), pickupLocationId);

        if (actualDistance < 0) {
            // No path exists
            continue;
        }

        // Calculate effective distance with cross-zone penalty
        int effectiveDistance = calculateEffectiveDistance(
            actualDistance,
            drivers[i].getZoneId(),
            pickupZoneId
        );

        // Update nearest driver if this one is closer
        if (effectiveDistance < minEffectiveDistance) {
            minEffectiveDistance = effectiveDistance;
            nearestDriverIndex = i;
        }
    }

    return nearestDriverIndex;
}
