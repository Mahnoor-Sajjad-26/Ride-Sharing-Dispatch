# Ride-Sharing Dispatch System - Design Document

## 1. Graph Representation and Routing Approach

### City Graph Structure

The city is represented as a **weighted undirected graph** using an **adjacency list** implementation with custom data structures (no STL containers for core logic).

```
City
├── locations[] (dynamic array of Location)
│   └── Location
│       ├── id: int
│       ├── name: string
│       ├── zoneId: int
│       └── adjacencyList: Edge* (linked list)
│           └── Edge
│               ├── destination: int
│               ├── weight: int (distance)
│               └── next: Edge*
```

### Why Adjacency List?

1. **Space Efficient**: O(V + E) vs O(V²) for adjacency matrix
2. **Dynamic**: Easy to add new locations and roads
3. **Sparse Graph**: City roads are typically sparse (each location connects to few others)

### Shortest Path Algorithm: Dijkstra's

**Implementation**: Array-based Dijkstra's algorithm without priority queue.

```cpp
int getDistance(int source, int destination) {
    1. Initialize dist[] = INF, visited[] = false
    2. dist[source] = 0
    3. For each vertex:
       a. Find unvisited vertex u with minimum dist[u]
       b. Mark u as visited
       c. For each neighbor v of u:
          if dist[u] + weight(u,v) < dist[v]:
             dist[v] = dist[u] + weight(u,v)
    4. Return dist[destination]
}
```

### Zone-Based Partitioning

- Each location has a `zoneId` attribute
- Drivers are assigned a home zone
- Cross-zone assignments incur a **50% distance penalty** to prefer local drivers

---

## 2. Trip State Machine Design

### State Diagram

```
                    ┌─────────────┐
                    │  REQUESTED  │
                    └──────┬──────┘
                           │
              ┌────────────┼────────────┐
              │            │            │
              ▼            │            │
        ┌──────────┐       │       ┌────┴─────┐
        │ ASSIGNED │       │       │ CANCELLED│
        └────┬─────┘       │       └──────────┘
             │             │            ▲
             ▼             │            │
        ┌──────────┐       │            │
        │ ONGOING  │───────┘────────────┘
        └────┬─────┘
             │
             ▼
        ┌──────────┐
        │COMPLETED │
        └──────────┘
```

### Valid Transitions

| Current State | Valid Next States |
|--------------|-------------------|
| REQUESTED | ASSIGNED, CANCELLED |
| ASSIGNED | ONGOING, CANCELLED |
| ONGOING | COMPLETED |
| COMPLETED | (terminal) |
| CANCELLED | (terminal) |

### Implementation

```cpp
bool Trip::canTransitionTo(TripState newState) {
    switch (state) {
        case TRIP_REQUESTED:
            return (newState == TRIP_ASSIGNED || newState == TRIP_CANCELLED);
        case TRIP_ASSIGNED:
            return (newState == TRIP_ONGOING || newState == TRIP_CANCELLED);
        case TRIP_ONGOING:
            return (newState == TRIP_COMPLETED);
        default:
            return false; // Terminal states
    }
}
```

**Enforcement**: All state changes go through `transitionTo()` which validates via `canTransitionTo()`.

---

## 3. Rollback Strategy

### Design Pattern: Command Pattern

Each operation is recorded as an `Operation` struct containing all data needed to reverse it.

### Operation Stack

```cpp
struct Operation {
    OperationType type;      // What happened
    int tripId;              // Which trip
    int driverId;            // Which driver

    // Snapshot of previous state
    TripState previousTripState;
    DriverStatus previousDriverStatus;
    int previousDriverLocation;
    int previousDriverTripsCompleted;
    int previousDriverDistanceCovered;
    int tripDistance;
};
```

### Rollback Process

```cpp
bool rollback(int k) {
    for (int i = 0; i < k; i++) {
        Operation op = stack.pop();

        switch (op.type) {
            case OP_TRIP_CREATED:
                // Remove trip from system
                tripCount--;
                break;

            case OP_TRIP_ASSIGNED:
                // Restore trip and driver to pre-assignment state
                trip->setState(op.previousTripState);
                trip->setDriverId(-1);
                driver->setStatus(op.previousDriverStatus);
                break;

            case OP_TRIP_COMPLETED:
                // Restore trip state and driver stats
                trip->setState(op.previousTripState);
                driver->setStatus(op.previousDriverStatus);
                driver->setLocation(op.previousDriverLocation);
                driver->setTripsCompleted(op.previousDriverTripsCompleted);
                driver->setDistanceCovered(op.previousDriverDistanceCovered);
                break;
            // ... similar for other operations
        }
    }
}
```

### Key Principle

**Record before change**: Always capture the current state BEFORE making modifications. This ensures perfect restoration.

---

## 4. Time and Space Complexity

### City Operations

| Operation | Time Complexity | Space Complexity |
|-----------|----------------|------------------|
| Add Location | O(1) amortized | O(1) |
| Add Road | O(1) | O(1) |
| Get Distance (Dijkstra) | O(V²) | O(V) |
| Get Shortest Path | O(V²) | O(V) |

**Note**: O(V²) Dijkstra is acceptable for small city graphs. For larger graphs, a min-heap implementation would reduce to O((V+E) log V).

### Driver/Trip Operations

| Operation | Time Complexity | Space Complexity |
|-----------|----------------|------------------|
| Find Nearest Driver | O(D × V²) | O(V) |
| Request Trip | O(V²) | O(1) |
| Assign Trip | O(D × V²) | O(1) |
| Start/Complete/Cancel Trip | O(1) | O(1) |
| Rollback (k operations) | O(k) | O(1) |

### Analytics

| Operation | Time Complexity | Space Complexity |
|-----------|----------------|------------------|
| Average Trip Distance | O(T) | O(1) |
| Driver Utilization | O(T) | O(1) |
| Completed/Cancelled Count | O(T) | O(1) |

### Space Summary

| Component | Space |
|-----------|-------|
| City Graph | O(V + E) |
| Drivers Array | O(D) |
| Riders Array | O(R) |
| Trips Array | O(T) |
| Operation Stack | O(O) |
| **Total** | **O(V + E + D + R + T + O)** |

Where:
- V = number of locations (vertices)
- E = number of roads (edges)
- D = number of drivers
- R = number of riders
- T = number of trips
- O = number of operations recorded

---

## 5. Data Structure Summary

| Component | Data Structure | Rationale |
|-----------|---------------|-----------|
| City locations | Dynamic array | Random access by ID, resizable |
| Adjacency list | Linked list | Efficient for sparse graphs |
| Drivers/Riders/Trips | Dynamic arrays | Sequential storage, fast access |
| Operation history | Stack (array-based) | LIFO for undo operations |
| State machine | Enum + validation | Type-safe transitions |

---

## 6. File Structure

```
project/
├── City.h / City.cpp           # Graph implementation
├── Driver.h / Driver.cpp       # Driver entity
├── Rider.h / Rider.cpp         # Rider entity
├── Trip.h / Trip.cpp           # Trip + state machine
├── DispatchEngine.h/.cpp       # Driver assignment logic
├── RollbackManager.h/.cpp      # Undo functionality
├── RideShareSystem.h/.cpp      # Main controller
├── main.cpp                    # Entry point + tests
└── design.md                   # This document
```

---

## 7. Testing Approach

10 test cases covering:

1. **Shortest Path Basic** - Direct path calculation
2. **Shortest Path Complex** - Multi-hop, cross-zone paths
3. **Driver Assignment Same Zone** - Prefers nearest driver
4. **Driver Assignment Cross Zone** - Applies penalty correctly
5. **Valid State Transition** - Full lifecycle
6. **Invalid State Transition** - Rejects illegal transitions
7. **Cancel and Reassign** - Driver becomes available
8. **Single Rollback** - Undo one operation
9. **Multiple Rollback** - Undo chain of operations
10. **Analytics After Rollback** - Stats update correctly

Each test verifies correctness and outputs PASSED/FAILED.
