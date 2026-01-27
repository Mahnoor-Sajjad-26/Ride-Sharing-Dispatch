#include "Driver.h"
#include <iostream>
using namespace std;

Driver::Driver()
    : id(-1), name(""), currentLocationId(-1), zoneId(-1),
      status(DRIVER_OFFLINE), totalTripsCompleted(0), totalDistanceCovered(0) {}

Driver::Driver(int id, const string& name, int locationId, int zoneId)
    : id(id), name(name), currentLocationId(locationId), zoneId(zoneId),
      status(DRIVER_AVAILABLE), totalTripsCompleted(0), totalDistanceCovered(0) {}

int Driver::getId() const {
    return id;
}

string Driver::getName() const {
    return name;
}

int Driver::getCurrentLocationId() const {
    return currentLocationId;
}

int Driver::getZoneId() const {
    return zoneId;
}

DriverStatus Driver::getStatus() const {
    return status;
}

int Driver::getTotalTripsCompleted() const {
    return totalTripsCompleted;
}

int Driver::getTotalDistanceCovered() const {
    return totalDistanceCovered;
}

void Driver::setCurrentLocationId(int locationId) {
    currentLocationId = locationId;
}

void Driver::setZoneId(int zone) {
    zoneId = zone;
}

void Driver::setStatus(DriverStatus newStatus) {
    status = newStatus;
}

void Driver::completeTrip(int distance) {
    totalTripsCompleted++;
    totalDistanceCovered += distance;
}

void Driver::resetStats() {
    totalTripsCompleted = 0;
    totalDistanceCovered = 0;
}

void Driver::setTotalTripsCompleted(int count) {
    totalTripsCompleted = count;
}

void Driver::setTotalDistanceCovered(int distance) {
    totalDistanceCovered = distance;
}

string Driver::getStatusString() const {
    switch (status) {
        case DRIVER_AVAILABLE: return "Available";
        case DRIVER_BUSY: return "Busy";
        case DRIVER_OFFLINE: return "Offline";
        default: return "Unknown";
    }
}

void Driver::display() const {
    cout << "Driver " << id << ": " << name
         << " | Location: " << currentLocationId
         << " | Zone: " << zoneId
         << " | Status: " << getStatusString()
         << " | Trips: " << totalTripsCompleted
         << " | Distance: " << totalDistanceCovered << endl;
}
