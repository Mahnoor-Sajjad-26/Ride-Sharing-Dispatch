#ifndef ROLLBACK_MANAGER_H
#define ROLLBACK_MANAGER_H

#include "Trip.h"
#include "Driver.h"

enum OperationType {
    OP_TRIP_CREATED,
    OP_TRIP_ASSIGNED,
    OP_TRIP_STARTED,
    OP_TRIP_COMPLETED,
    OP_TRIP_CANCELLED
};

// Stores all information needed to rollback an operation
struct Operation {
    OperationType type;
    int tripId;
    int driverId;

    // Previous state for rollback
    TripState previousTripState;
    DriverStatus previousDriverStatus;
    int previousDriverLocation;
    int previousDriverTripsCompleted;
    int previousDriverDistanceCovered;
    int tripDistance;

    Operation();
    Operation(OperationType type, int tripId, int driverId);
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
