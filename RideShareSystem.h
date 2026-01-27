#ifndef RIDE_SHARE_SYSTEM_H
#define RIDE_SHARE_SYSTEM_H

#include "City.h"
#include "Driver.h"
#include "Rider.h"
#include "Trip.h"
#include "DispatchEngine.h"
#include "RollbackManager.h"

class RideShareSystem {
private:
    City city;
    Driver* drivers;
    int driverCount;
    int driverCapacity;

    Rider* riders;
    int riderCount;
    int riderCapacity;

    Trip* trips;
    int tripCount;
    int tripCapacity;

    DispatchEngine dispatcher;
    RollbackManager rollbackMgr;

    // Helper methods
    void resizeDrivers();
    void resizeRiders();
    void resizeTrips();

    Trip* findTrip(int tripId);
    Driver* findDriver(int driverId);

public:
    RideShareSystem();
    ~RideShareSystem();

    // City setup
    int addLocation(const string& name, int zoneId);
    void addRoad(int from, int to, int distance);

    // Driver and Rider management
    int addDriver(const string& name, int locationId, int zoneId);
    int addRider(const string& name);

    // Trip operations (core functionality)
    int requestTrip(int riderId, int pickupId, int dropoffId);
    bool assignTrip(int tripId);
    bool startTrip(int tripId);
    bool completeTrip(int tripId);
    bool cancelTrip(int tripId);

    // Rollback functionality
    bool rollback(int k = 1);

    // Analytics
    double getAverageTripDistance() const;
    double getDriverUtilization(int driverId) const;
    int getCompletedTripCount() const;
    int getCancelledTripCount() const;
    int getTotalTripCount() const;

    // Getters for display
    City& getCity();
    int getDriverCount() const;
    int getRiderCount() const;

    // Display methods
    void displayCity() const;
    void displayDrivers() const;
    void displayRiders() const;
    void displayTrips() const;
    void displayAnalytics() const;
    void displayOperationHistory() const;

    // For testing
    Driver* getDriver(int index);
    Trip* getTrip(int index);
    int getDistance(int from, int to);
};

#endif
