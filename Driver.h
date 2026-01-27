#ifndef DRIVER_H
#define DRIVER_H

#include <string>
using namespace std;

enum DriverStatus {
    DRIVER_AVAILABLE,
    DRIVER_BUSY,
    DRIVER_OFFLINE
};

class Driver {
private:
    int id;
    string name;
    int currentLocationId;
    int zoneId;
    DriverStatus status;
    int totalTripsCompleted;
    int totalDistanceCovered;

public:
    Driver();
    Driver(int id, const string& name, int locationId, int zoneId);

    // Getters
    int getId() const;
    string getName() const;
    int getCurrentLocationId() const;
    int getZoneId() const;
    DriverStatus getStatus() const;
    int getTotalTripsCompleted() const;
    int getTotalDistanceCovered() const;

    // Setters
    void setCurrentLocationId(int locationId);
    void setZoneId(int zone);
    void setStatus(DriverStatus newStatus);

    // Operations
    void completeTrip(int distance);
    void resetStats();

    // For rollback
    void setTotalTripsCompleted(int count);
    void setTotalDistanceCovered(int distance);

    // Display
    void display() const;
    string getStatusString() const;
};

#endif
