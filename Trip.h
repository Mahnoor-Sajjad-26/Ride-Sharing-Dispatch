#ifndef TRIP_H
#define TRIP_H

#include <string>
using namespace std;

// Trip State Constants (instead of enum)
const int TRIP_REQUESTED = 0;
const int TRIP_ASSIGNED = 1;
const int TRIP_ONGOING = 2;
const int TRIP_COMPLETED = 3;
const int TRIP_CANCELLED = 4;
const int TRIP_STATE_COUNT = 5;

// Lookup array for state names (declared in Trip.cpp)
extern const string TRIP_STATE_NAMES[];

class Trip {
private:
    int id;
    int riderId;
    int driverId;
    int pickupLocationId;
    int dropoffLocationId;
    int state;  // Using int instead of enum
    int distance;

public:
    Trip();
    Trip(int id, int riderId, int pickupId, int dropoffId);

    // Getters
    int getId() const;
    int getRiderId() const;
    int getDriverId() const;
    int getPickupLocationId() const;
    int getDropoffLocationId() const;
    int getState() const;  // Returns int instead of enum
    int getDistance() const;

    // Setters (for rollback)
    void setDriverId(int id);
    void setState(int newState);  // Takes int instead of enum
    void setDistance(int dist);

    // State machine operations
    bool canTransitionTo(int newState) const;  // Takes int instead of enum
    bool transitionTo(int newState);

    // Assign driver
    bool assignDriver(int driverId);

    // Display
    void display() const;
    string getStateString() const;

    // Check states
    bool isActive() const;
    bool isTerminal() const;
};

#endif
