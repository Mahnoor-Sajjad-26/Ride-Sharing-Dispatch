#include "Trip.h"
#include <iostream>
using namespace std;

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

TripState Trip::getState() const {
    return state;
}

int Trip::getDistance() const {
    return distance;
}

void Trip::setDriverId(int id) {
    driverId = id;
}

void Trip::setState(TripState newState) {
    state = newState;
}

void Trip::setDistance(int dist) {
    distance = dist;
}

bool Trip::canTransitionTo(TripState newState) const {
    // State machine validation
    // Valid transitions:
    // REQUESTED -> ASSIGNED, CANCELLED
    // ASSIGNED -> ONGOING, CANCELLED
    // ONGOING -> COMPLETED
    // COMPLETED -> (terminal)
    // CANCELLED -> (terminal)

    switch (state) {
        case TRIP_REQUESTED:
            return (newState == TRIP_ASSIGNED || newState == TRIP_CANCELLED);

        case TRIP_ASSIGNED:
            return (newState == TRIP_ONGOING || newState == TRIP_CANCELLED);

        case TRIP_ONGOING:
            return (newState == TRIP_COMPLETED);

        case TRIP_COMPLETED:
        case TRIP_CANCELLED:
            return false; // Terminal states

        default:
            return false;
    }
}

bool Trip::transitionTo(TripState newState) {
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
    switch (state) {
        case TRIP_REQUESTED: return "Requested";
        case TRIP_ASSIGNED: return "Assigned";
        case TRIP_ONGOING: return "Ongoing";
        case TRIP_COMPLETED: return "Completed";
        case TRIP_CANCELLED: return "Cancelled";
        default: return "Unknown";
    }
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
