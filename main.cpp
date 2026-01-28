// Fix for Windows SDK byte conflict with C++17
#define _HAS_STD_BYTE 0

#include <iostream>
#include "RideShareSystem.h"
#include "WebServer.h"
using namespace std;

// Function prototypes
void displayMenu();
void setupSampleCity(RideShareSystem& system);
void runAllTests(RideShareSystem& system);

// Individual test functions
bool testShortestPathBasic(RideShareSystem& system);
bool testShortestPathComplex(RideShareSystem& system);
bool testDriverAssignmentSameZone(RideShareSystem& system);
bool testDriverAssignmentCrossZone(RideShareSystem& system);
bool testValidStateTransition(RideShareSystem& system);
bool testInvalidStateTransition(RideShareSystem& system);
bool testCancelAndReassign(RideShareSystem& system);
bool testSingleRollback(RideShareSystem& system);
bool testMultipleRollback(RideShareSystem& system);
bool testAnalyticsAfterRollback(RideShareSystem& system);

void displayMenu() {
    cout << "\n========================================" << endl;
    cout << "   RIDE-SHARING DISPATCH SYSTEM" << endl;
    cout << "========================================" << endl;
    cout << "1. Request a Trip" << endl;
    cout << "2. Start a Trip" << endl;
    cout << "3. Complete a Trip" << endl;
    cout << "4. Cancel a Trip" << endl;
    cout << "5. Rollback Operations" << endl;
    cout << "6. View All Trips" << endl;
    cout << "7. View Drivers" << endl;
    cout << "8. View City Map" << endl;
    cout << "9. View Analytics" << endl;
    cout << "10. View Operation History" << endl;
    cout << "11. Run All Tests" << endl;
    cout << "12. Start Web Server & GUI Dashboard" << endl;
    cout << "0. Exit" << endl;
    cout << "----------------------------------------" << endl;
    cout << "Enter choice: ";
}

void setupSampleCity(RideShareSystem& system) {
    // Zone 1 (Downtown): Locations 0-3 (A, B, C, D)
    // Zone 2 (Suburb): Locations 4-7 (E, F, G, H)

    // Add locations
    system.addLocation("A-Downtown", 1);  // 0
    system.addLocation("B-Downtown", 1);  // 1
    system.addLocation("C-Downtown", 1);  // 2
    system.addLocation("D-Downtown", 1);  // 3
    system.addLocation("E-Suburb", 2);    // 4
    system.addLocation("F-Suburb", 2);    // 5
    system.addLocation("G-Suburb", 2);    // 6
    system.addLocation("H-Suburb", 2);    // 7

    // Zone 1 roads
    system.addRoad(0, 1, 5);  // A-B: 5
    system.addRoad(0, 2, 3);  // A-C: 3
    system.addRoad(1, 3, 4);  // B-D: 4
    system.addRoad(2, 3, 2);  // C-D: 2

    // Zone 2 roads
    system.addRoad(4, 5, 8);  // E-F: 8
    system.addRoad(4, 6, 6);  // E-G: 6
    system.addRoad(6, 7, 3);  // G-H: 3

    // Cross-zone connection
    system.addRoad(3, 4, 10); // D-E: 10 (connects zones)

    // Add drivers
    system.addDriver("Mahnoor", 0, 1);   // Driver 0: at A, Zone 1
    system.addDriver("Bob", 3, 1);     // Driver 1: at D, Zone 1
    system.addDriver("Charlie", 6, 2); // Driver 2: at G, Zone 2

    // Add riders
    system.addRider("John");    // Rider 0
    system.addRider("Jane");    // Rider 1
    system.addRider("Mike");    // Rider 2

    cout << "Sample city with 8 locations, 3 drivers, and 3 riders loaded." << endl;
}

// ==================== TEST CASES ====================

bool testShortestPathBasic(RideShareSystem& system) {
    cout << "\n[TEST 1] Shortest Path - Basic" << endl;
    cout << "Testing path from A(0) to D(3)..." << endl;

    int distance = system.getDistance(0, 3);  // A to D
    // Expected: A->C->D = 3+2 = 5 (shorter than A->B->D = 5+4 = 9)

    cout << "Distance A to D: " << distance << endl;
    bool passed = (distance == 5);
    cout << "Expected: 5, Got: " << distance << " - " << (passed ? "PASSED" : "FAILED") << endl;
    return passed;
}

bool testShortestPathComplex(RideShareSystem& system) {
    cout << "\n[TEST 2] Shortest Path - Complex (Cross-zone)" << endl;
    cout << "Testing path from A(0) to H(7)..." << endl;

    int distance = system.getDistance(0, 7);  // A to H
    // Expected: A->C->D->E->G->H = 3+2+10+6+3 = 24

    cout << "Distance A to H: " << distance << endl;
    bool passed = (distance == 24);
    cout << "Expected: 24, Got: " << distance << " - " << (passed ? "PASSED" : "FAILED") << endl;
    return passed;
}

bool testDriverAssignmentSameZone(RideShareSystem& system) {
    cout << "\n[TEST 3] Driver Assignment - Same Zone" << endl;
    cout << "Requesting trip in Zone 1 (pickup at C)..." << endl;

    int tripId = system.requestTrip(0, 2, 3);  // Rider 0, C to D
    bool assigned = system.assignTrip(tripId);

    // Alice is at A (dist to C = 3), Bob is at D (dist to C = 2)
    // Bob should be assigned (closer)
    Trip* trip = system.getTrip(tripId);
    bool passed = assigned && trip && trip->getDriverId() == 1; // Bob

    cout << "Assigned driver: " << (trip ? trip->getDriverId() : -1) << endl;
    cout << "Expected: Driver 1 (Bob) - " << (passed ? "PASSED" : "FAILED") << endl;

    // Cancel for next tests
    system.cancelTrip(tripId);
    system.rollback(3); // Clean up

    return passed;
}

bool testDriverAssignmentCrossZone(RideShareSystem& system) {
    cout << "\n[TEST 4] Driver Assignment - Cross Zone Penalty" << endl;
    cout << "Requesting trip in Zone 2 (pickup at E)..." << endl;

    int tripId = system.requestTrip(1, 4, 7);  // Rider 1, E to H
    bool assigned = system.assignTrip(tripId);

    // Charlie at G (Zone 2): dist to E = 6, no penalty = 6
    // Bob at D (Zone 1): dist to E = 10, with 50% penalty = 15
    // Charlie should be assigned
    Trip* trip = system.getTrip(tripId);
    bool passed = assigned && trip && trip->getDriverId() == 2; // Charlie

    cout << "Assigned driver: " << (trip ? trip->getDriverId() : -1) << endl;
    cout << "Expected: Driver 2 (Charlie) - " << (passed ? "PASSED" : "FAILED") << endl;

    // Cancel for next tests
    system.cancelTrip(tripId);
    system.rollback(3);

    return passed;
}

bool testValidStateTransition(RideShareSystem& system) {
    cout << "\n[TEST 5] Valid State Transitions" << endl;
    cout << "Testing: REQUESTED -> ASSIGNED -> ONGOING -> COMPLETED" << endl;

    int tripId = system.requestTrip(0, 0, 3);  // A to D
    Trip* trip = system.getTrip(tripId);

    bool step1 = trip && trip->getState() == TRIP_REQUESTED;
    cout << "1. Created in REQUESTED state: " << (step1 ? "OK" : "FAIL") << endl;

    system.assignTrip(tripId);
    bool step2 = trip && trip->getState() == TRIP_ASSIGNED;
    cout << "2. Transitioned to ASSIGNED: " << (step2 ? "OK" : "FAIL") << endl;

    system.startTrip(tripId);
    bool step3 = trip && trip->getState() == TRIP_ONGOING;
    cout << "3. Transitioned to ONGOING: " << (step3 ? "OK" : "FAIL") << endl;

    system.completeTrip(tripId);
    bool step4 = trip && trip->getState() == TRIP_COMPLETED;
    cout << "4. Transitioned to COMPLETED: " << (step4 ? "OK" : "FAIL") << endl;

    bool passed = step1 && step2 && step3 && step4;
    cout << "Result: " << (passed ? "PASSED" : "FAILED") << endl;

    system.rollback(4); // Clean up

    return passed;
}

bool testInvalidStateTransition(RideShareSystem& system) {
    cout << "\n[TEST 6] Invalid State Transition" << endl;
    cout << "Testing: REQUESTED -> COMPLETED (should fail)" << endl;

    int tripId = system.requestTrip(0, 0, 3);
    Trip* trip = system.getTrip(tripId);

    bool canTransition = trip->canTransitionTo(TRIP_COMPLETED);
    cout << "Can transition REQUESTED -> COMPLETED: " << (canTransition ? "Yes" : "No") << endl;

    bool passed = !canTransition;  // Should NOT be able to transition
    cout << "Expected: No - " << (passed ? "PASSED" : "FAILED") << endl;

    system.rollback(1);

    return passed;
}

bool testCancelAndReassign(RideShareSystem& system) {
    cout << "\n[TEST 7] Cancel and Driver Reassignment" << endl;

    int tripId = system.requestTrip(0, 2, 3);  // C to D
    system.assignTrip(tripId);

    Trip* trip = system.getTrip(tripId);
    int assignedDriver = trip->getDriverId();
    Driver* driver = system.getDriver(assignedDriver);

    cout << "Driver " << assignedDriver << " status after assignment: "
         << driver->getStatusString() << endl;

    bool busyAfterAssign = driver->getStatus() == DRIVER_BUSY;

    system.cancelTrip(tripId);
    cout << "Driver " << assignedDriver << " status after cancellation: "
         << driver->getStatusString() << endl;

    bool availableAfterCancel = driver->getStatus() == DRIVER_AVAILABLE;

    bool passed = busyAfterAssign && availableAfterCancel;
    cout << "Result: " << (passed ? "PASSED" : "FAILED") << endl;

    system.rollback(3);

    return passed;
}

bool testSingleRollback(RideShareSystem& system) {
    cout << "\n[TEST 8] Single Rollback" << endl;

    int tripId = system.requestTrip(0, 0, 3);
    system.assignTrip(tripId);

    Trip* trip = system.getTrip(tripId);
    cout << "State before rollback: " << trip->getStateString() << endl;

    system.rollback(1);  // Undo assignment
    cout << "State after rollback: " << trip->getStateString() << endl;

    bool passed = trip->getState() == TRIP_REQUESTED && trip->getDriverId() == -1;
    cout << "Expected: REQUESTED with no driver - " << (passed ? "PASSED" : "FAILED") << endl;

    system.rollback(1); // Clean up creation

    return passed;
}

bool testMultipleRollback(RideShareSystem& system) {
    cout << "\n[TEST 9] Multiple Rollback (k=3)" << endl;

    int tripId = system.requestTrip(0, 0, 3);
    system.assignTrip(tripId);
    system.startTrip(tripId);

    Trip* trip = system.getTrip(tripId);
    cout << "State before rollback: " << trip->getStateString() << endl;

    system.rollback(3);  // Undo start, assign, and create

    // After rolling back creation, tripCount should decrease
    int currentTrips = system.getTotalTripCount();
    cout << "Trips after rollback: " << currentTrips << endl;

    bool passed = (currentTrips == 0);
    cout << "Expected: 0 trips - " << (passed ? "PASSED" : "FAILED") << endl;

    return passed;
}

bool testAnalyticsAfterRollback(RideShareSystem& system) {
    cout << "\n[TEST 10] Analytics After Rollback" << endl;

    // Complete a trip
    int tripId = system.requestTrip(0, 0, 3);  // A to D, distance 5
    system.assignTrip(tripId);
    system.startTrip(tripId);
    system.completeTrip(tripId);

    double avgBefore = system.getAverageTripDistance();
    int completedBefore = system.getCompletedTripCount();
    cout << "Before rollback - Avg distance: " << avgBefore
         << ", Completed: " << completedBefore << endl;

    // Rollback the completion
    system.rollback(1);

    double avgAfter = system.getAverageTripDistance();
    int completedAfter = system.getCompletedTripCount();
    cout << "After rollback - Avg distance: " << avgAfter
         << ", Completed: " << completedAfter << endl;

    bool passed = (completedAfter == completedBefore - 1);
    cout << "Expected: Completed count decreased by 1 - " << (passed ? "PASSED" : "FAILED") << endl;

    // Clean up
    system.rollback(3);

    return passed;
}

void runAllTests(RideShareSystem& system) {
    cout << "\n========================================" << endl;
    cout << "        RUNNING ALL TEST CASES" << endl;
    cout << "========================================" << endl;

    int passed = 0;
    int total = 10;

    if (testShortestPathBasic(system)) passed++;
    if (testShortestPathComplex(system)) passed++;
    if (testDriverAssignmentSameZone(system)) passed++;
    if (testDriverAssignmentCrossZone(system)) passed++;
    if (testValidStateTransition(system)) passed++;
    if (testInvalidStateTransition(system)) passed++;
    if (testCancelAndReassign(system)) passed++;
    if (testSingleRollback(system)) passed++;
    if (testMultipleRollback(system)) passed++;
    if (testAnalyticsAfterRollback(system)) passed++;

    cout << "\n========================================" << endl;
    cout << "TEST RESULTS: " << passed << "/" << total << " PASSED" << endl;
    cout << "========================================" << endl;
}

int main() {
    RideShareSystem system;
    setupSampleCity(system);

    int choice;
    int tripId, riderId, pickupId, dropoffId, k;

    do {
        displayMenu();
        cin >> choice;

        switch (choice) {
            case 1: // Request Trip
                cout << "Enter Rider ID: ";
                cin >> riderId;
                system.displayCity();
                cout << "Enter Pickup Location ID: ";
                cin >> pickupId;
                cout << "Enter Dropoff Location ID: ";
                cin >> dropoffId;
                tripId = system.requestTrip(riderId, pickupId, dropoffId);
                if (tripId >= 0) {
                    cout << "Auto-assigning driver..." << endl;
                    system.assignTrip(tripId);
                }
                break;

            case 2: // Start Trip
                system.displayTrips();
                cout << "Enter Trip ID to start: ";
                cin >> tripId;
                system.startTrip(tripId);
                break;

            case 3: // Complete Trip
                system.displayTrips();
                cout << "Enter Trip ID to complete: ";
                cin >> tripId;
                system.completeTrip(tripId);
                break;

            case 4: // Cancel Trip
                system.displayTrips();
                cout << "Enter Trip ID to cancel: ";
                cin >> tripId;
                system.cancelTrip(tripId);
                break;

            case 5: // Rollback
                system.displayOperationHistory();
                cout << "Enter number of operations to rollback: ";
                cin >> k;
                system.rollback(k);
                break;

            case 6: // View Trips
                system.displayTrips();
                break;

            case 7: // View Drivers
                system.displayDrivers();
                break;

            case 8: // View City
                system.displayCity();
                break;

            case 9: // View Analytics
                system.displayAnalytics();
                break;

            case 10: // View History
                system.displayOperationHistory();
                break;

            case 11: // Run Tests
                runAllTests(system);
                // Reset system after tests
                system = RideShareSystem();
                setupSampleCity(system);
                break;

            case 12: // Start Web Server
                {
                    cout << "\n========================================" << endl;
                    cout << "  LAUNCHING WEB SERVER" << endl;
                    cout << "========================================" << endl;
                    cout << "The web server will start and the system will be accessible via browser." << endl;
                    cout << "Once started, open your browser and go to:" << endl;
                    cout << "  http://localhost:8080/dashboard.html" << endl;
                    cout << "\nPress Ctrl+C to stop the server and return to menu." << endl;
                    cout << "========================================\n" << endl;

                    WebServer webServer(&system, 8080);
                    webServer.start();
                }
                break;

            case 0:
                cout << "Exiting system. Goodbye!" << endl;
                break;

            default:
                cout << "Invalid choice. Please try again." << endl;
        }

    } while (choice != 0);

    return 0;
}
