#ifndef ROLLBACK_MANAGER_H
#define ROLLBACK_MANAGER_H

#include "Trip.h"
#include "Driver.h"

// Operation Type Constants (instead of enum)
const int OP_TRIP_CREATED = 0;
const int OP_TRIP_ASSIGNED = 1;
const int OP_TRIP_STARTED = 2;
const int OP_TRIP_COMPLETED = 3;
const int OP_TRIP_CANCELLED = 4;
const int OP_TYPE_COUNT = 5;

// Lookup array for operation names (declared in RollbackManager.cpp)
extern const string OP_TYPE_NAMES[];

// Stores all information needed to rollback an operation
struct Operation {
    int type;  // Using int instead of enum
    int tripId;
    int driverId;

    // Previous state for rollback (using int instead of enum)
    int previousTripState;
    int previousDriverStatus;
    int previousDriverLocation;
    int previousDriverTripsCompleted;
    int previousDriverDistanceCovered;
    int tripDistance;

    Operation();
    Operation(int type, int tripId, int driverId);
};

class RollbackManager {
private:
    Operation* operationStack;
    int top;
    int capacity;

    void resize();

public:
    RollbackManager(int initialCapacity = 100);
    ~RollbackManager();

    // Stack operations
    void recordOperation(const Operation& op);
    Operation popOperation();
    Operation peekOperation() const;

    // Status
    bool canRollback() const;
    int getOperationCount() const;

    // Clear all operations
    void clear();

    // Display
    void displayHistory() const;
};

#endif
