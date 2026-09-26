#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <set>
#include <queue>
#include <stack>
#include <cmath>
using namespace std;
enum Direction
{
    UP,
    DOWN,
    IDLE
};

enum ElevatorState
{
    MOVING,
    STOPPED
};

// ----------------------------------------------------
// Request
// ----------------------------------------------------

class Request
{
protected:
    int floor;

public:
    Request(int floor)
    {
        this->floor = floor;
    }

    int getFloor()
    {
        return floor;
    }

    virtual ~Request() {}
};

// ----------------------------------------------------
// External Request
// Example: floor 5 presses UP
// ----------------------------------------------------

class ExternalRequest : public Request
{
    Direction direction;

public:
    ExternalRequest(int floor, Direction direction)
        : Request(floor)
    {
        this->direction = direction;
    }

    Direction getDirection()
    {
        return direction;
    }
};

// ----------------------------------------------------
// Internal Request
// Example: user inside elevator presses floor 10
// ----------------------------------------------------

class InternalRequest : public Request
{

public:
    InternalRequest(int destinationFloor)
        : Request(destinationFloor)
    {
    }
};

// ----------------------------------------------------
// Elevator
// ----------------------------------------------------

class Elevator
{
    int id;
    int currentFloor;

    Direction currentDirection;
    ElevatorState currentState;

    // ascending order
    set<int> upRequests;

    // descending order
    set<int, greater<int>> downRequests;

public:
    Elevator(int id, int currentFloor = 0)
    {
        this->id = id;
        this->currentFloor = currentFloor;
        this->currentDirection = IDLE;
        this->currentState = STOPPED;
    }

    int getId()
    {
        return id;
    }

    int getCurrentFloor()
    {
        return currentFloor;
    }

    Direction getDirection()
    {
        return currentDirection;
    }

    ElevatorState getState()
    {
        return currentState;
    }

    int getPendingRequestsCount()
    {
        return upRequests.size() + downRequests.size();
    }

    // -------------------------------------------
    // Adding internal request
    // -------------------------------------------

    void addInternalRequest(InternalRequest *request)
    {

        int floor = request->getFloor();

        cout << "Internal request for floor "
             << floor
             << " added to elevator "
             << id
             << "\n";

        if (floor > currentFloor)
        {
            upRequests.insert(floor);
        }
        else if (floor < currentFloor)
        {
            downRequests.insert(floor);
        }
        else
        {
            cout << "Elevator already at floor "
                 << floor << "\n";
            openDoor();
            closeDoor();
        }
    }

    // -------------------------------------------
    // Adding external request
    // -------------------------------------------

    void addExternalRequest(ExternalRequest *request)
    {

        int floor = request->getFloor();

        cout << "External request assigned to Elevator "
             << id
             << " for floor "
             << floor
             << "\n";

        /*
            Important:

            If request floor is above us,
            we need to reach that floor first.

            If below us,
            we need to eventually go down there.
        */

        if (floor > currentFloor)
        {
            upRequests.insert(floor);
        }
        else if (floor < currentFloor)
        {
            downRequests.insert(floor);
        }
        else
        {
            // Request is from our current floor
            openDoor();
            closeDoor();
        }
    }

    // -------------------------------------------
    // Process all requests
    // -------------------------------------------

    void processRequests()
    {

        while (!upRequests.empty() ||
               !downRequests.empty())
        {

            /*
                Continue in current direction
                if possible.
            */

            if (currentDirection == UP)
            {

                if (!upRequests.empty())
                {
                    processUpRequests();
                }
                else
                {
                    currentDirection = DOWN;
                }
            }

            else if (currentDirection == DOWN)
            {

                if (!downRequests.empty())
                {
                    processDownRequests();
                }
                else
                {
                    currentDirection = UP;
                }
            }

            else
            {

                /*
                    Elevator is idle.

                    Decide which direction to start.
                */

                if (!upRequests.empty())
                {
                    currentDirection = UP;
                }
                else if (!downRequests.empty())
                {
                    currentDirection = DOWN;
                }
            }
        }

        currentDirection = IDLE;
        currentState = STOPPED;

        cout << "Elevator "
             << id
             << " is now IDLE at floor "
             << currentFloor
             << "\n";
    }

private:
    // -------------------------------------------
    // Process next upward stop
    // -------------------------------------------

    void processUpRequests()
    {

        if (upRequests.empty())
            return;

        int nextFloor = *upRequests.begin();

        upRequests.erase(upRequests.begin());

        moveToFloor(nextFloor);
    }

    // -------------------------------------------
    // Process next downward stop
    // -------------------------------------------

    void processDownRequests()
    {

        if (downRequests.empty())
            return;

        int nextFloor = *downRequests.begin();

        downRequests.erase(downRequests.begin());

        moveToFloor(nextFloor);
    }

    // -------------------------------------------
    // Elevator physically moves
    // -------------------------------------------

    void moveToFloor(int destination)
    {

        currentState = MOVING;

        if (destination > currentFloor)
        {

            currentDirection = UP;

            while (currentFloor < destination)
            {
                currentFloor++;

                cout << "Elevator "
                     << id
                     << " moving UP -> floor "
                     << currentFloor
                     << "\n";
            }
        }

        else if (destination < currentFloor)
        {

            currentDirection = DOWN;

            while (currentFloor > destination)
            {
                currentFloor--;

                cout << "Elevator "
                     << id
                     << " moving DOWN -> floor "
                     << currentFloor
                     << "\n";
            }
        }

        currentState = STOPPED;

        cout << "Elevator "
             << id
             << " reached floor "
             << destination
             << "\n";

        openDoor();
        closeDoor();
    }

    void openDoor()
    {
        cout << "Elevator "
             << id
             << " door opened at floor "
             << currentFloor
             << "\n";
    }

    void closeDoor()
    {
        cout << "Elevator "
             << id
             << " door closed\n";
    }
};

// ----------------------------------------------------
// Floor
// ----------------------------------------------------

class Floor
{
    int floorNumber;

public:
    Floor(int floorNumber)
    {
        this->floorNumber = floorNumber;
    }

    int getFloorNumber()
    {
        return floorNumber;
    }
};

// ----------------------------------------------------
// Elevator Controller
// ----------------------------------------------------

class ElevatorController
{

public:
    Elevator *selectElevator(
        vector<Elevator *> &elevators,
        ExternalRequest *request)
    {

        Elevator *bestElevator = nullptr;
        int minimumCost = INT_MAX;

        for (auto elevator : elevators)
        {

            bool eligible = false;

            /*
                Rule 1:
                Idle elevator can serve.
            */

            if (elevator->getDirection() == IDLE)
            {
                eligible = true;
            }

            /*
                Rule 2:
                Elevator moving UP can serve
                if request is UP and request floor
                is ahead of elevator.
            */

            else if (
                elevator->getDirection() == UP &&
                request->getDirection() == UP &&
                elevator->getCurrentFloor() <= request->getFloor())
            {
                eligible = true;
            }

            /*
                Rule 3:
                Same idea for DOWN.
            */

            else if (
                elevator->getDirection() == DOWN &&
                request->getDirection() == DOWN &&
                elevator->getCurrentFloor() >= request->getFloor())
            {
                eligible = true;
            }

            if (!eligible)
                continue;

            int cost =
                abs(
                    elevator->getCurrentFloor() - request->getFloor());

            if (cost < minimumCost)
            {
                minimumCost = cost;
                bestElevator = elevator;
            }
        }

        /*
            Fallback:
            if no elevator matched direction,
            choose nearest elevator.
        */

        if (bestElevator == nullptr)
        {

            for (auto elevator : elevators)
            {

                int cost =
                    abs(
                        elevator->getCurrentFloor() - request->getFloor());

                if (cost < minimumCost)
                {
                    minimumCost = cost;
                    bestElevator = elevator;
                }
            }
        }

        return bestElevator;
    }

    void handleExternalRequest(
        vector<Elevator *> &elevators,
        ExternalRequest *request)
    {

        Elevator *selected =
            selectElevator(
                elevators,
                request);

        if (selected == nullptr)
        {
            cout << "No elevator available\n";
            return;
        }

        cout << "\nController selected Elevator "
             << selected->getId()
             << "\n";

        selected->addExternalRequest(request);
    }
};

// ----------------------------------------------------
// Elevator System
// ----------------------------------------------------

class ElevatorSystem
{

    vector<Elevator *> elevators;

    ElevatorController *controller;

public:
    ElevatorSystem()
    {
        controller =
            new ElevatorController();
    }

    void addElevator(Elevator *elevator)
    {
        elevators.push_back(elevator);
    }

    /*
        Called when someone presses
        UP/DOWN outside elevator.
    */

    void requestElevator(
        int floor,
        Direction direction)
    {

        ExternalRequest *request =
            new ExternalRequest(
                floor,
                direction);

        controller->handleExternalRequest(
            elevators,
            request);
    }

    /*
        Called after passenger enters
        an already selected elevator.
    */

    void selectFloor(
        int elevatorId,
        int destinationFloor)
    {

        Elevator *elevator =
            getElevatorById(elevatorId);

        if (elevator == nullptr)
        {
            cout << "Invalid elevator\n";
            return;
        }

        InternalRequest *request =
            new InternalRequest(
                destinationFloor);

        elevator->addInternalRequest(request);
    }

    void runElevator(int elevatorId)
    {

        Elevator *elevator =
            getElevatorById(elevatorId);

        if (elevator != nullptr)
        {
            elevator->processRequests();
        }
    }

private:
    Elevator *getElevatorById(int id)
    {

        for (auto elevator : elevators)
        {

            if (elevator->getId() == id)
                return elevator;
        }

        return nullptr;
    }
};

// ----------------------------------------------------
// MAIN
// ----------------------------------------------------

int main()
{

    ElevatorSystem system;

    Elevator *e1 =
        new Elevator(1, 2);

    Elevator *e2 =
        new Elevator(2, 6);

    Elevator *e3 =
        new Elevator(3, 10);

    system.addElevator(e1);
    system.addElevator(e2);
    system.addElevator(e3);

    /*
        Someone on floor 5 presses UP.
    */

    cout << "\n===== External Request =====\n";

    system.requestElevator(
        5,
        UP);

    /*
        Assume Elevator 2 was selected.

        Passenger enters elevator 2
        and presses floor 9.
    */

    cout << "\n===== Internal Request =====\n";

    system.selectFloor(
        2,
        9);

    /*
        Process Elevator 2's requests.
    */

    cout << "\n===== Movement =====\n";

    system.runElevator(2);

    return 0;
}