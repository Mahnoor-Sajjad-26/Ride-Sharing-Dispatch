#ifndef TRIP_H
#define TRIP_H

#include <string>
using namespace std;

enum TripState {
    TRIP_REQUESTED,
    TRIP_ASSIGNED,
    TRIP_ONGOING,
    TRIP_COMPLETED,
    TRIP_CANCELLED
};

class Trip {
private:
    int id;
    int riderId;
    int driverId;
    int pickupLocationId;
    int dropoffLocationId;
    TripState state;
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
    TripState getState() const;
    int getDistance() const;

    // Setters (for rollback)
    void setDriverId(int id);
    void setState(TripState newState);
    void setDistance(int dist);

    // State machine operations
    bool canTransitionTo(TripState newState) const;
    bool transitionTo(TripState newState);

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
