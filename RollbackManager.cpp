#include "RollbackManager.h"
#include <iostream>
using namespace std;

// Lookup array for operation type names
const string OP_TYPE_NAMES[] = {
    "CREATED",    // 0 = OP_TRIP_CREATED
    "ASSIGNED",   // 1 = OP_TRIP_ASSIGNED
    "STARTED",    // 2 = OP_TRIP_STARTED
    "COMPLETED",  // 3 = OP_TRIP_COMPLETED
    "CANCELLED"   // 4 = OP_TRIP_CANCELLED
};

Operation::Operation()
    : type(OP_TRIP_CREATED), tripId(-1), driverId(-1),
      previousTripState(TRIP_REQUESTED), previousDriverStatus(DRIVER_AVAILABLE),
      previousDriverLocation(-1), previousDriverTripsCompleted(0),
      previousDriverDistanceCovered(0), tripDistance(0) {}

Operation::Operation(int type, int tripId, int driverId)
    : type(type), tripId(tripId), driverId(driverId),
      previousTripState(TRIP_REQUESTED), previousDriverStatus(DRIVER_AVAILABLE),
      previousDriverLocation(-1), previousDriverTripsCompleted(0),
      previousDriverDistanceCovered(0), tripDistance(0) {}

RollbackManager::RollbackManager(int initialCapacity)
    : top(-1), capacity(initialCapacity) {
    operationStack = new Operation[capacity];
}

RollbackManager::~RollbackManager() {
    delete[] operationStack;
}

void RollbackManager::resize() {
    int newCapacity = capacity * 2;
    Operation* newStack = new Operation[newCapacity];

    for (int i = 0; i <= top; i++) {
        newStack[i] = operationStack[i];
    }

    delete[] operationStack;
    operationStack = newStack;
    capacity = newCapacity;
}

void RollbackManager::recordOperation(const Operation& op) {
    if (top + 1 >= capacity) {
        resize();
    }
    operationStack[++top] = op;
}

Operation RollbackManager::popOperation() {
    if (top < 0) {
        return Operation(); // Return empty operation
    }
    return operationStack[top--];
}

Operation RollbackManager::peekOperation() const {
    if (top < 0) {
        return Operation();
    }
    return operationStack[top];
}

bool RollbackManager::canRollback() const {
    return top >= 0;
}

int RollbackManager::getOperationCount() const {
    return top + 1;
}

void RollbackManager::clear() {
    top = -1;
}

void RollbackManager::displayHistory() const {
    cout << "\n=== Operation History (Stack) ===" << endl;
    if (top < 0) {
        cout << "No operations recorded." << endl;
        return;
    }

    for (int i = top; i >= 0; i--) {
        cout << "[" << (top - i + 1) << "] Trip " << operationStack[i].tripId << " ";

        // Use lookup array instead of switch
        if (operationStack[i].type >= 0 && operationStack[i].type < OP_TYPE_COUNT) {
            cout << OP_TYPE_NAMES[operationStack[i].type];
        } else {
            cout << "UNKNOWN";
        }

        // Add driver info for assignment
        if (operationStack[i].type == OP_TRIP_ASSIGNED) {
            cout << " to Driver " << operationStack[i].driverId;
        }

        cout << endl;
    }
}
