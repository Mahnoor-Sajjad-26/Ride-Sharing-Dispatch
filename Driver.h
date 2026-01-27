#ifndef DRIVER_H
#define DRIVER_H

#include <string>
using namespace std;

// Driver Status Constants (instead of enum)
const int DRIVER_AVAILABLE = 0;
const int DRIVER_BUSY = 1;
const int DRIVER_OFFLINE = 2;
const int DRIVER_STATUS_COUNT = 3;

// Lookup array for status names (declared in Driver.cpp)
extern const string DRIVER_STATUS_NAMES[];

class Driver {
private:
    int id;
    string name;
    int currentLocationId;
    int zoneId;
    int status;  // Using int instead of enum
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
    int getStatus() const;  // Returns int instead of enum
    int getTotalTripsCompleted() const;
    int getTotalDistanceCovered() const;

    // Setters
    void setCurrentLocationId(int locationId);
    void setZoneId(int zone);
    void setStatus(int newStatus);  // Takes int instead of enum

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
