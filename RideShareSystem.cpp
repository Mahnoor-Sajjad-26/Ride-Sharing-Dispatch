#include "RideShareSystem.h"
#include <iostream>
using namespace std;

RideShareSystem::RideShareSystem()
    : driverCount(0), driverCapacity(10),
      riderCount(0), riderCapacity(10),
      tripCount(0), tripCapacity(20) {
    drivers = new Driver[driverCapacity];
    riders = new Rider[riderCapacity];
    trips = new Trip[tripCapacity];
}

RideShareSystem::~RideShareSystem() {
    delete[] drivers;
    delete[] riders;
    delete[] trips;
}

void RideShareSystem::resizeDrivers() {
    int newCapacity = driverCapacity * 2;
    Driver* newDrivers = new Driver[newCapacity];
    for (int i = 0; i < driverCount; i++) {
        newDrivers[i] = drivers[i];
    }
    delete[] drivers;
    drivers = newDrivers;
    driverCapacity = newCapacity;
}

void RideShareSystem::resizeRiders() {
    int newCapacity = riderCapacity * 2;
    Rider* newRiders = new Rider[newCapacity];
    for (int i = 0; i < riderCount; i++) {
        newRiders[i] = riders[i];
    }
    delete[] riders;
    riders = newRiders;
    riderCapacity = newCapacity;
}

void RideShareSystem::resizeTrips() {
    int newCapacity = tripCapacity * 2;
    Trip* newTrips = new Trip[newCapacity];
    for (int i = 0; i < tripCount; i++) {
        newTrips[i] = trips[i];
    }
    delete[] trips;
    trips = newTrips;
    tripCapacity = newCapacity;
}

Trip* RideShareSystem::findTrip(int tripId) {
    for (int i = 0; i < tripCount; i++) {
        if (trips[i].getId() == tripId) {
            return &trips[i];
        }
    }
    return nullptr;
}

Driver* RideShareSystem::findDriver(int driverId) {
    for (int i = 0; i < driverCount; i++) {
        if (drivers[i].getId() == driverId) {
            return &drivers[i];
        }
    }
    return nullptr;
}

// City setup
int RideShareSystem::addLocation(const string& name, int zoneId) {
    return city.addLocation(name, zoneId);
}

void RideShareSystem::addRoad(int from, int to, int distance) {
    city.addRoad(from, to, distance);
}

// Driver and Rider management
int RideShareSystem::addDriver(const string& name, int locationId, int zoneId) {
    if (driverCount >= driverCapacity) {
        resizeDrivers();
    }
    int id = driverCount;
    drivers[driverCount] = Driver(id, name, locationId, zoneId);
    driverCount++;
    return id;
}

int RideShareSystem::addRider(const string& name) {
    if (riderCount >= riderCapacity) {
        resizeRiders();
    }
    int id = riderCount;
    riders[riderCount] = Rider(id, name);
    riderCount++;
    return id;
}

// Trip operations
int RideShareSystem::requestTrip(int riderId, int pickupId, int dropoffId) {
    if (riderId < 0 || riderId >= riderCount) {
        cout << "Invalid rider ID." << endl;
        return -1;
    }

    if (tripCount >= tripCapacity) {
        resizeTrips();
    }

    int tripId = tripCount;
    trips[tripCount] = Trip(tripId, riderId, pickupId, dropoffId);

    // Calculate and set trip distance
    int distance = city.getDistance(pickupId, dropoffId);
    if (distance >= 0) {
        trips[tripCount].setDistance(distance);
    }

    // Record operation for rollback
    Operation op(OP_TRIP_CREATED, tripId, -1);
    rollbackMgr.recordOperation(op);

    tripCount++;
    cout << "Trip " << tripId << " requested successfully." << endl;
    return tripId;
}

bool RideShareSystem::assignTrip(int tripId) {
    Trip* trip = findTrip(tripId);
    if (!trip) {
        cout << "Trip not found." << endl;
        return false;
    }

    if (trip->getState() != TRIP_REQUESTED) {
        cout << "Trip cannot be assigned from current state." << endl;
        return false;
    }

    int pickupLocationId = trip->getPickupLocationId();
    int pickupZoneId = city.getLocationZone(pickupLocationId);

    // Find nearest available driver
    int driverIndex = dispatcher.findNearestDriver(city, drivers, driverCount,
                                                    pickupLocationId, pickupZoneId);

    if (driverIndex < 0) {
        cout << "No available drivers found." << endl;
        return false;
    }

    // Record operation for rollback BEFORE making changes
    Operation op(OP_TRIP_ASSIGNED, tripId, drivers[driverIndex].getId());
    op.previousTripState = trip->getState();
    op.previousDriverStatus = drivers[driverIndex].getStatus();
    op.previousDriverLocation = drivers[driverIndex].getCurrentLocationId();
    rollbackMgr.recordOperation(op);

    // Assign driver
    trip->assignDriver(drivers[driverIndex].getId());
    drivers[driverIndex].setStatus(DRIVER_BUSY);

    cout << "Trip " << tripId << " assigned to Driver " << drivers[driverIndex].getId()
         << " (" << drivers[driverIndex].getName() << ")." << endl;
    return true;
}

bool RideShareSystem::startTrip(int tripId) {
    Trip* trip = findTrip(tripId);
    if (!trip) {
        cout << "Trip not found." << endl;
        return false;
    }

    if (!trip->canTransitionTo(TRIP_ONGOING)) {
        cout << "Trip cannot be started from current state (" << trip->getStateString() << ")." << endl;
        return false;
    }

    Driver* driver = findDriver(trip->getDriverId());

    // Record operation for rollback
    Operation op(OP_TRIP_STARTED, tripId, trip->getDriverId());
    op.previousTripState = trip->getState();
    if (driver) {
        op.previousDriverLocation = driver->getCurrentLocationId();
    }
    rollbackMgr.recordOperation(op);

    // Start the trip
    trip->transitionTo(TRIP_ONGOING);

    // Move driver to pickup location
    if (driver) {
        driver->setCurrentLocationId(trip->getPickupLocationId());
    }

    cout << "Trip " << tripId << " is now ongoing." << endl;
    return true;
}

bool RideShareSystem::completeTrip(int tripId) {
    Trip* trip = findTrip(tripId);
    if (!trip) {
        cout << "Trip not found." << endl;
        return false;
    }

    if (!trip->canTransitionTo(TRIP_COMPLETED)) {
        cout << "Trip cannot be completed from current state (" << trip->getStateString() << ")." << endl;
        return false;
    }

    Driver* driver = findDriver(trip->getDriverId());

    // Record operation for rollback
    Operation op(OP_TRIP_COMPLETED, tripId, trip->getDriverId());
    op.previousTripState = trip->getState();
    op.tripDistance = trip->getDistance();
    if (driver) {
        op.previousDriverStatus = driver->getStatus();
        op.previousDriverLocation = driver->getCurrentLocationId();
        op.previousDriverTripsCompleted = driver->getTotalTripsCompleted();
        op.previousDriverDistanceCovered = driver->getTotalDistanceCovered();
    }
    rollbackMgr.recordOperation(op);

    // Complete the trip
    trip->transitionTo(TRIP_COMPLETED);

    // Update driver stats and location
    if (driver) {
        driver->setCurrentLocationId(trip->getDropoffLocationId());
        driver->setZoneId(city.getLocationZone(trip->getDropoffLocationId()));
        driver->completeTrip(trip->getDistance());
        driver->setStatus(DRIVER_AVAILABLE);
    }

    cout << "Trip " << tripId << " completed. Distance: " << trip->getDistance() << endl;
    return true;
}

bool RideShareSystem::cancelTrip(int tripId) {
    Trip* trip = findTrip(tripId);
    if (!trip) {
        cout << "Trip not found." << endl;
        return false;
    }

    if (!trip->canTransitionTo(TRIP_CANCELLED)) {
        cout << "Trip cannot be cancelled from current state (" << trip->getStateString() << ")." << endl;
        return false;
    }

    Driver* driver = findDriver(trip->getDriverId());

    // Record operation for rollback
    Operation op(OP_TRIP_CANCELLED, tripId, trip->getDriverId());
    op.previousTripState = trip->getState();
    if (driver) {
        op.previousDriverStatus = driver->getStatus();
        op.previousDriverLocation = driver->getCurrentLocationId();
    }
    rollbackMgr.recordOperation(op);

    // Cancel the trip
    trip->transitionTo(TRIP_CANCELLED);

    // Restore driver availability if assigned
    if (driver) {
        driver->setStatus(DRIVER_AVAILABLE);
    }

    cout << "Trip " << tripId << " cancelled." << endl;
    return true;
}

bool RideShareSystem::rollback(int k) {
    if (k <= 0) {
        cout << "Invalid rollback count." << endl;
        return false;
    }

    int rolledBack = 0;
    for (int i = 0; i < k && rollbackMgr.canRollback(); i++) {
        Operation op = rollbackMgr.popOperation();
        Trip* trip = findTrip(op.tripId);
        Driver* driver = findDriver(op.driverId);

        switch (op.type) {
            case OP_TRIP_CREATED:
                // Remove the trip (mark as invalid)
                if (trip) {
                    // We can't truly delete from array, so we decrement count
                    // This works because we add sequentially
                    if (op.tripId == tripCount - 1) {
                        tripCount--;
                    }
                }
                cout << "Rolled back: Trip " << op.tripId << " creation." << endl;
                break;

            case OP_TRIP_ASSIGNED:
                if (trip) {
                    trip->setState(op.previousTripState);
                    trip->setDriverId(-1);
                }
                if (driver) {
                    driver->setStatus(op.previousDriverStatus);
                }
                cout << "Rolled back: Trip " << op.tripId << " assignment." << endl;
                break;

            case OP_TRIP_STARTED:
                if (trip) {
                    trip->setState(op.previousTripState);
                }
                if (driver) {
                    driver->setCurrentLocationId(op.previousDriverLocation);
                }
                cout << "Rolled back: Trip " << op.tripId << " start." << endl;
                break;

            case OP_TRIP_COMPLETED:
                if (trip) {
                    trip->setState(op.previousTripState);
                }
                if (driver) {
                    driver->setStatus(op.previousDriverStatus);
                    driver->setCurrentLocationId(op.previousDriverLocation);
                    driver->setTotalTripsCompleted(op.previousDriverTripsCompleted);
                    driver->setTotalDistanceCovered(op.previousDriverDistanceCovered);
                }
                cout << "Rolled back: Trip " << op.tripId << " completion." << endl;
                break;

            case OP_TRIP_CANCELLED:
                if (trip) {
                    trip->setState(op.previousTripState);
                }
                if (driver) {
                    driver->setStatus(op.previousDriverStatus);
                }
                cout << "Rolled back: Trip " << op.tripId << " cancellation." << endl;
                break;
        }
        rolledBack++;
    }

    cout << "Successfully rolled back " << rolledBack << " operation(s)." << endl;
    return rolledBack > 0;
}

// Analytics
double RideShareSystem::getAverageTripDistance() const {
    int completedCount = 0;
    int totalDistance = 0;

    for (int i = 0; i < tripCount; i++) {
        if (trips[i].getState() == TRIP_COMPLETED) {
            completedCount++;
            totalDistance += trips[i].getDistance();
        }
    }

    if (completedCount == 0) return 0.0;
    return (double)totalDistance / completedCount;
}

double RideShareSystem::getDriverUtilization(int driverId) const {
    const Driver* driver = nullptr;
    for (int i = 0; i < driverCount; i++) {
        if (drivers[i].getId() == driverId) {
            driver = &drivers[i];
            break;
        }
    }

    if (!driver) return 0.0;

    int totalCompleted = getCompletedTripCount();
    if (totalCompleted == 0) return 0.0;

    return (double)driver->getTotalTripsCompleted() / totalCompleted * 100.0;
}

int RideShareSystem::getCompletedTripCount() const {
    int count = 0;
    for (int i = 0; i < tripCount; i++) {
        if (trips[i].getState() == TRIP_COMPLETED) {
            count++;
        }
    }
    return count;
}

int RideShareSystem::getCancelledTripCount() const {
    int count = 0;
    for (int i = 0; i < tripCount; i++) {
        if (trips[i].getState() == TRIP_CANCELLED) {
            count++;
        }
    }
    return count;
}

int RideShareSystem::getTotalTripCount() const {
    return tripCount;
}

// Getters
City& RideShareSystem::getCity() {
    return city;
}

int RideShareSystem::getDriverCount() const {
    return driverCount;
}

int RideShareSystem::getRiderCount() const {
    return riderCount;
}

// Display methods
void RideShareSystem::displayCity() const {
    city.displayCity();
}

void RideShareSystem::displayDrivers() const {
    cout << "\n=== Drivers ===" << endl;
    if (driverCount == 0) {
        cout << "No drivers registered." << endl;
        return;
    }
    for (int i = 0; i < driverCount; i++) {
        drivers[i].display();
    }
}

void RideShareSystem::displayRiders() const {
    cout << "\n=== Riders ===" << endl;
    if (riderCount == 0) {
        cout << "No riders registered." << endl;
        return;
    }
    for (int i = 0; i < riderCount; i++) {
        riders[i].display();
    }
}

void RideShareSystem::displayTrips() const {
    cout << "\n=== Trips ===" << endl;
    if (tripCount == 0) {
        cout << "No trips recorded." << endl;
        return;
    }
    for (int i = 0; i < tripCount; i++) {
        trips[i].display();
    }
}

void RideShareSystem::displayAnalytics() const {
    cout << "\n=== Analytics ===" << endl;
    cout << "Total Trips: " << tripCount << endl;
    cout << "Completed Trips: " << getCompletedTripCount() << endl;
    cout << "Cancelled Trips: " << getCancelledTripCount() << endl;
    cout << "Average Trip Distance: " << getAverageTripDistance() << endl;

    cout << "\nDriver Utilization:" << endl;
    for (int i = 0; i < driverCount; i++) {
        cout << "  Driver " << i << " (" << drivers[i].getName() << "): "
             << getDriverUtilization(i) << "%" << endl;
    }
}

void RideShareSystem::displayOperationHistory() const {
    rollbackMgr.displayHistory();
}

// For testing
Driver* RideShareSystem::getDriver(int index) {
    if (index < 0 || index >= driverCount) return nullptr;
    return &drivers[index];
}

Trip* RideShareSystem::getTrip(int index) {
    if (index < 0 || index >= tripCount) return nullptr;
    return &trips[index];
}

int RideShareSystem::getDistance(int from, int to) {
    return city.getDistance(from, to);
}
