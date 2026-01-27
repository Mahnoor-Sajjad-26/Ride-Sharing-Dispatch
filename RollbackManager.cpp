#include "RollbackManager.h"
#include <iostream>
using namespace std;

Operation::Operation()
    : type(OP_TRIP_CREATED), tripId(-1), driverId(-1),
      previousTripState(TRIP_REQUESTED), previousDriverStatus(DRIVER_AVAILABLE),
      previousDriverLocation(-1), previousDriverTripsCompleted(0),
      previousDriverDistanceCovered(0), tripDistance(0) {}

Operation::Operation(OperationType type, int tripId, int driverId)
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
        cout << "[" << (top - i + 1) << "] ";
        switch (operationStack[i].type) {
            case OP_TRIP_CREATED:
                cout << "Trip " << operationStack[i].tripId << " CREATED";
                break;
            case OP_TRIP_ASSIGNED:
                cout << "Trip " << operationStack[i].tripId << " ASSIGNED to Driver " << operationStack[i].driverId;
                break;
            case OP_TRIP_STARTED:
                cout << "Trip " << operationStack[i].tripId << " STARTED";
                break;
            case OP_TRIP_COMPLETED:
                cout << "Trip " << operationStack[i].tripId << " COMPLETED";
                break;
            case OP_TRIP_CANCELLED:
                cout << "Trip " << operationStack[i].tripId << " CANCELLED";
                break;
        }
        cout << endl;
    }
}
