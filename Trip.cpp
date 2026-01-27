#include "Trip.h"
#include <iostream>
using namespace std;

// Lookup array for state names
const string TRIP_STATE_NAMES[] = {
    "Requested",   // 0 = TRIP_REQUESTED
    "Assigned",    // 1 = TRIP_ASSIGNED
    "Ongoing",     // 2 = TRIP_ONGOING
    "Completed",   // 3 = TRIP_COMPLETED
    "Cancelled"    // 4 = TRIP_CANCELLED
};

Trip::Trip()
    : id(-1), riderId(-1), driverId(-1), pickupLocationId(-1),
      dropoffLocationId(-1), state(TRIP_REQUESTED), distance(0) {}

Trip::Trip(int id, int riderId, int pickupId, int dropoffId)
    : id(id), riderId(riderId), driverId(-1), pickupLocationId(pickupId),
      dropoffLocationId(dropoffId), state(TRIP_REQUESTED), distance(0) {}

int Trip::getId() const {
    return id;
}

int Trip::getRiderId() const {
    return riderId;
}

int Trip::getDriverId() const {
    return driverId;
}

int Trip::getPickupLocationId() const {
    return pickupLocationId;
}

int Trip::getDropoffLocationId() const {
    return dropoffLocationId;
}

int Trip::getState() const {
    return state;
}

int Trip::getDistance() const {
    return distance;
}

void Trip::setDriverId(int id) {
    driverId = id;
}

void Trip::setState(int newState) {
    state = newState;
}

void Trip::setDistance(int dist) {
    distance = dist;
}

bool Trip::canTransitionTo(int newState) const {
    // State machine validation using if-else (no switch on enum)
    // Valid transitions:
    // REQUESTED -> ASSIGNED, CANCELLED
    // ASSIGNED -> ONGOING, CANCELLED
    // ONGOING -> COMPLETED
    // COMPLETED -> (terminal)
    // CANCELLED -> (terminal)

    if (state == TRIP_REQUESTED) {
        return (newState == TRIP_ASSIGNED || newState == TRIP_CANCELLED);
    }
    else if (state == TRIP_ASSIGNED) {
        return (newState == TRIP_ONGOING || newState == TRIP_CANCELLED);
    }
    else if (state == TRIP_ONGOING) {
        return (newState == TRIP_COMPLETED);
    }
    else if (state == TRIP_COMPLETED || state == TRIP_CANCELLED) {
        return false; // Terminal states
    }

    return false;
}

bool Trip::transitionTo(int newState) {
    if (canTransitionTo(newState)) {
        state = newState;
        return true;
    }
    return false;
}

bool Trip::assignDriver(int driver) {
    if (state == TRIP_REQUESTED) {
        driverId = driver;
        return transitionTo(TRIP_ASSIGNED);
    }
    return false;
}

string Trip::getStateString() const {
    // Use lookup array instead of switch
    if (state >= 0 && state < TRIP_STATE_COUNT) {
        return TRIP_STATE_NAMES[state];
    }
    return "Unknown";
}

bool Trip::isActive() const {
    return (state == TRIP_REQUESTED || state == TRIP_ASSIGNED || state == TRIP_ONGOING);
}

bool Trip::isTerminal() const {
    return (state == TRIP_COMPLETED || state == TRIP_CANCELLED);
}

void Trip::display() const {
    cout << "Trip " << id
         << " | Rider: " << riderId
         << " | Driver: " << (driverId >= 0 ? to_string(driverId) : "None")
         << " | Pickup: " << pickupLocationId
         << " | Dropoff: " << dropoffLocationId
         << " | State: " << getStateString()
         << " | Distance: " << distance << endl;
}
