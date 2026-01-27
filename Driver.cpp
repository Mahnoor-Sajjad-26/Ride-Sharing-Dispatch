#include "Driver.h"
#include <iostream>
using namespace std;

// Lookup array for status names
const string DRIVER_STATUS_NAMES[] = {
    "Available",  // 0 = DRIVER_AVAILABLE
    "Busy",       // 1 = DRIVER_BUSY
    "Offline"     // 2 = DRIVER_OFFLINE
};

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

int Driver::getStatus() const {
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

void Driver::setStatus(int newStatus) {
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
    // Use lookup array instead of switch
    if (status >= 0 && status < DRIVER_STATUS_COUNT) {
        return DRIVER_STATUS_NAMES[status];
    }
    return "Unknown";
}

void Driver::display() const {
    cout << "Driver " << id << ": " << name
         << " | Location: " << currentLocationId
         << " | Zone: " << zoneId
         << " | Status: " << getStatusString()
         << " | Trips: " << totalTripsCompleted
         << " | Distance: " << totalDistanceCovered << endl;
}
