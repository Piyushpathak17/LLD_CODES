#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
using namespace std;

// ============================================================
// ENUMS
// ============================================================

enum VehicleType
{
    BIKE,
    CAR,
    TRUCK
};

enum SpotType
{
    BIKE_SPOT,
    CAR_SPOT,
    TRUCK_SPOT
};

enum TicketStatus
{
    ACTIVE,
    CLOSED
};

// ============================================================
// VEHICLE
// ============================================================

class Vehicle
{
    string number;
    VehicleType type;

public:
    Vehicle(string number, VehicleType type)
    {
        this->number = number;
        this->type = type;
    }

    string getNumber()
    {
        return number;
    }

    VehicleType getType()
    {
        return type;
    }
};

// ============================================================
// PARKING SPOT
// ============================================================

class ParkingSpot
{
    int id;
    SpotType type;

    // nullptr means the spot is free
    Vehicle *vehicle;

public:
    ParkingSpot(int id, SpotType type)
    {
        this->id = id;
        this->type = type;
        this->vehicle = nullptr;
    }

    int getId()
    {
        return id;
    }

    SpotType getType()
    {
        return type;
    }

    Vehicle *getVehicle()
    {
        return vehicle;
    }

    bool isAvailable()
    {
        return vehicle == nullptr;
    }

    bool canPark(Vehicle *v)
    {

        if (!isAvailable())
            return false;

        if (v->getType() == BIKE &&
            type == BIKE_SPOT)
            return true;

        if (v->getType() == CAR &&
            type == CAR_SPOT)
            return true;

        if (v->getType() == TRUCK &&
            type == TRUCK_SPOT)
            return true;

        return false;
    }

    bool parkVehicle(Vehicle *v)
    {

        if (!canPark(v))
            return false;

        vehicle = v;

        return true;
    }

    void removeVehicle()
    {
        vehicle = nullptr;
    }
};

// ============================================================
// PARKING FLOOR
// ============================================================

class ParkingFloor
{
    int floorNumber;

    vector<ParkingSpot *> spots;

public:
    ParkingFloor(int floorNumber)
    {
        this->floorNumber = floorNumber;
    }

    int getFloorNumber()
    {
        return floorNumber;
    }

    void addSpot(ParkingSpot *spot)
    {
        spots.push_back(spot);
    }

    vector<ParkingSpot *> &getSpots()
    {
        return spots;
    }

    void displayAvailability()
    {

        cout << "\nFloor " << floorNumber << "\n";

        for (auto spot : spots)
        {

            cout << "Spot "
                 << spot->getId()
                 << " -> ";

            if (spot->isAvailable())
                cout << "FREE";
            else
                cout << "OCCUPIED";

            cout << "\n";
        }
    }
};

// ============================================================
// TICKET
// ============================================================

class Ticket
{
    int id;

    Vehicle *vehicle;
    ParkingSpot *spot;
    ParkingFloor *floor;

    long long entryTime;

    TicketStatus status;

public:
    Ticket(
        int id,
        Vehicle *vehicle,
        ParkingSpot *spot,
        ParkingFloor *floor,
        long long entryTime)
    {
        this->id = id;
        this->vehicle = vehicle;
        this->spot = spot;
        this->floor = floor;
        this->entryTime = entryTime;

        this->status = ACTIVE;
    }

    int getId()
    {
        return id;
    }

    Vehicle *getVehicle()
    {
        return vehicle;
    }

    ParkingSpot *getSpot()
    {
        return spot;
    }

    ParkingFloor *getFloor()
    {
        return floor;
    }

    long long getEntryTime()
    {
        return entryTime;
    }

    TicketStatus getStatus()
    {
        return status;
    }

    void close()
    {
        status = CLOSED;
    }
};

// ============================================================
// PARKING SPOT STRATEGY
// ============================================================

class ParkingSpotStrategy
{
public:
    virtual pair<ParkingFloor *, ParkingSpot *>
    findSpot(
        vector<ParkingFloor *> &floors,
        Vehicle *vehicle) = 0;

    virtual ~ParkingSpotStrategy() {}
};

// ============================================================
// FIRST AVAILABLE STRATEGY
// ============================================================

class FirstAvailableSpotStrategy
    : public ParkingSpotStrategy
{

public:
    pair<ParkingFloor *, ParkingSpot *>
    findSpot(
        vector<ParkingFloor *> &floors,
        Vehicle *vehicle) override
    {

        for (auto floor : floors)
        {

            for (auto spot : floor->getSpots())
            {

                if (spot->canPark(vehicle))
                {

                    return {
                        floor,
                        spot};
                }
            }
        }

        return {nullptr, nullptr};
    }
};

// ============================================================
// LOWEST FLOOR STRATEGY
// ============================================================

class LowestFloorSpotStrategy
    : public ParkingSpotStrategy
{

public:
    pair<ParkingFloor *, ParkingSpot *>
    findSpot(
        vector<ParkingFloor *> &floors,
        Vehicle *vehicle) override
    {

        ParkingFloor *selectedFloor = nullptr;
        ParkingSpot *selectedSpot = nullptr;

        int bestFloor = INT_MAX;

        for (auto floor : floors)
        {

            for (auto spot : floor->getSpots())
            {

                if (!spot->canPark(vehicle))
                    continue;

                if (floor->getFloorNumber() < bestFloor)
                {

                    bestFloor =
                        floor->getFloorNumber();

                    selectedFloor = floor;
                    selectedSpot = spot;
                }
            }
        }

        return {
            selectedFloor,
            selectedSpot};
    }
};

// ============================================================
// PRICING STRATEGY
// ============================================================

class PricingStrategy
{
public:
    virtual double calculateFee(
        Ticket *ticket,
        long long exitTime) = 0;

    virtual ~PricingStrategy() {}
};

// ============================================================
// HOURLY PRICING
// ============================================================

class HourlyPricingStrategy
    : public PricingStrategy
{

public:
    double calculateFee(
        Ticket *ticket,
        long long exitTime) override
    {

        long long duration =
            exitTime - ticket->getEntryTime();

        int hours =
            max(
                1LL,
                (duration + 3599) / 3600);

        int rate = 0;

        VehicleType type =
            ticket->getVehicle()->getType();

        if (type == BIKE)
            rate = 10;

        else if (type == CAR)
            rate = 20;

        else if (type == TRUCK)
            rate = 30;

        return hours * rate;
    }
};

// ============================================================
// FLAT PRICING
// ============================================================

class FlatPricingStrategy
    : public PricingStrategy
{

public:
    double calculateFee(
        Ticket *ticket,
        long long exitTime) override
    {

        VehicleType type =
            ticket->getVehicle()->getType();

        if (type == BIKE)
            return 50;

        if (type == CAR)
            return 100;

        return 200;
    }
};

// ============================================================
// PAYMENT STRATEGY
// ============================================================

class PaymentStrategy
{
public:
    virtual bool pay(double amount) = 0;

    virtual ~PaymentStrategy() {}
};

// ============================================================
// UPI
// ============================================================

class UPIPayment
    : public PaymentStrategy
{

public:
    bool pay(double amount) override
    {

        cout << "Paid Rs."
             << amount
             << " using UPI\n";

        return true;
    }
};

// ============================================================
// CARD
// ============================================================

class CardPayment
    : public PaymentStrategy
{

public:
    bool pay(double amount) override
    {

        cout << "Paid Rs."
             << amount
             << " using Card\n";

        return true;
    }
};

// ============================================================
// CASH
// ============================================================

class CashPayment
    : public PaymentStrategy
{

public:
    bool pay(double amount) override
    {

        cout << "Paid Rs."
             << amount
             << " using Cash\n";

        return true;
    }
};

// ============================================================
// PARKING LOT
// ============================================================

class ParkingLot
{

    vector<ParkingFloor *> floors;
    vector<Ticket *> tickets;

    ParkingSpotStrategy *spotStrategy;
    PricingStrategy *pricingStrategy;

    int nextTicketId;

public:
    ParkingLot(
        ParkingSpotStrategy *spotStrategy,
        PricingStrategy *pricingStrategy)
    {
        this->spotStrategy =
            spotStrategy;

        this->pricingStrategy =
            pricingStrategy;

        nextTicketId = 1;
    }

    // --------------------------------------------------------
    // ADD FLOOR
    // --------------------------------------------------------

    void addFloor(
        ParkingFloor *floor)
    {
        floors.push_back(floor);
    }

    // --------------------------------------------------------
    // CHANGE SPOT STRATEGY
    // --------------------------------------------------------

    void setSpotStrategy(
        ParkingSpotStrategy *strategy)
    {
        spotStrategy = strategy;
    }

    // --------------------------------------------------------
    // CHANGE PRICING STRATEGY
    // --------------------------------------------------------

    void setPricingStrategy(
        PricingStrategy *strategy)
    {
        pricingStrategy = strategy;
    }

    // --------------------------------------------------------
    // PARK VEHICLE
    // --------------------------------------------------------

    Ticket *parkVehicle(
        Vehicle *vehicle)
    {

        pair<ParkingFloor *, ParkingSpot *> result =
            spotStrategy->findSpot(
                floors,
                vehicle);

        ParkingFloor *floor =
            result.first;

        ParkingSpot *spot =
            result.second;

        if (spot == nullptr)
        {

            cout << "No compatible parking spot available\n";

            return nullptr;
        }

        bool parked =
            spot->parkVehicle(vehicle);

        if (!parked)
        {

            cout << "Unable to park vehicle\n";

            return nullptr;
        }

        long long entryTime =
            time(nullptr);

        Ticket *ticket =
            new Ticket(
                nextTicketId++,
                vehicle,
                spot,
                floor,
                entryTime);

        tickets.push_back(ticket);

        cout << "\nVehicle "
             << vehicle->getNumber()
             << " parked successfully\n";

        cout << "Ticket ID: "
             << ticket->getId()
             << "\n";

        cout << "Floor: "
             << floor->getFloorNumber()
             << "\n";

        cout << "Spot: "
             << spot->getId()
             << "\n";

        return ticket;
    }

    // --------------------------------------------------------
    // UNPARK VEHICLE
    // --------------------------------------------------------

    bool unparkVehicle(
        Ticket *ticket,
        PaymentStrategy *paymentStrategy)
    {

        if (ticket == nullptr)
        {

            cout << "Invalid ticket\n";

            return false;
        }

        if (ticket->getStatus() == CLOSED)
        {

            cout << "Ticket already closed\n";

            return false;
        }

        long long exitTime =
            time(nullptr);

        double fee =
            pricingStrategy
                ->calculateFee(
                    ticket,
                    exitTime);

        cout << "\nParking fee = Rs."
             << fee
             << "\n";

        bool success =
            paymentStrategy->pay(fee);

        if (!success)
        {

            cout << "Payment failed\n";

            return false;
        }

        ticket
            ->getSpot()
            ->removeVehicle();

        ticket->close();

        cout << "Vehicle "
             << ticket
                    ->getVehicle()
                    ->getNumber()
             << " removed successfully\n";

        cout << "Ticket closed\n";

        return true;
    }

    // --------------------------------------------------------
    // DISPLAY AVAILABILITY
    // --------------------------------------------------------

    void displayAvailability()
    {

        cout << "\n===== PARKING STATUS =====\n";

        for (auto floor : floors)
        {
            floor->displayAvailability();
        }
    }

    // --------------------------------------------------------
    // GET TICKET
    // --------------------------------------------------------

    Ticket *getTicketById(
        int ticketId)
    {

        for (auto ticket : tickets)
        {

            if (ticket->getId() == ticketId)
            {

                return ticket;
            }
        }

        return nullptr;
    }
};

// ============================================================
// MAIN
// ============================================================

int main()
{

    // --------------------------------------------------------
    // FLOORS
    // --------------------------------------------------------

    ParkingFloor *floor1 =
        new ParkingFloor(1);

    ParkingFloor *floor2 =
        new ParkingFloor(2);

    // --------------------------------------------------------
    // FLOOR 1 SPOTS
    // --------------------------------------------------------

    floor1->addSpot(
        new ParkingSpot(
            101,
            BIKE_SPOT));

    floor1->addSpot(
        new ParkingSpot(
            102,
            CAR_SPOT));

    floor1->addSpot(
        new ParkingSpot(
            103,
            CAR_SPOT));

    floor1->addSpot(
        new ParkingSpot(
            104,
            TRUCK_SPOT));

    // --------------------------------------------------------
    // FLOOR 2 SPOTS
    // --------------------------------------------------------

    floor2->addSpot(
        new ParkingSpot(
            201,
            BIKE_SPOT));

    floor2->addSpot(
        new ParkingSpot(
            202,
            CAR_SPOT));

    floor2->addSpot(
        new ParkingSpot(
            203,
            TRUCK_SPOT));

    // --------------------------------------------------------
    // STRATEGIES
    // --------------------------------------------------------

    ParkingSpotStrategy *spotStrategy =
        new FirstAvailableSpotStrategy();

    PricingStrategy *pricingStrategy =
        new HourlyPricingStrategy();

    // --------------------------------------------------------
    // PARKING LOT
    // --------------------------------------------------------

    ParkingLot parkingLot(
        spotStrategy,
        pricingStrategy);

    parkingLot.addFloor(floor1);
    parkingLot.addFloor(floor2);

    // --------------------------------------------------------
    // VEHICLES
    // --------------------------------------------------------

    Vehicle *bike =
        new Vehicle(
            "UP70-BIKE-01",
            BIKE);

    Vehicle *car1 =
        new Vehicle(
            "UP70-CAR-01",
            CAR);

    Vehicle *car2 =
        new Vehicle(
            "UP70-CAR-02",
            CAR);

    Vehicle *truck =
        new Vehicle(
            "UP70-TRUCK-01",
            TRUCK);

    // --------------------------------------------------------
    // INITIAL STATUS
    // --------------------------------------------------------

    parkingLot.displayAvailability();

    // --------------------------------------------------------
    // PARK
    // --------------------------------------------------------

    Ticket *bikeTicket =
        parkingLot.parkVehicle(
            bike);

    Ticket *carTicket1 =
        parkingLot.parkVehicle(
            car1);

    Ticket *carTicket2 =
        parkingLot.parkVehicle(
            car2);

    Ticket *truckTicket =
        parkingLot.parkVehicle(
            truck);

    // --------------------------------------------------------
    // STATUS
    // --------------------------------------------------------

    parkingLot.displayAvailability();

    // --------------------------------------------------------
    // EXIT USING UPI
    // --------------------------------------------------------

    PaymentStrategy *upi =
        new UPIPayment();

    parkingLot.unparkVehicle(
        carTicket1,
        upi);

    // --------------------------------------------------------
    // STATUS AGAIN
    // --------------------------------------------------------

    parkingLot.displayAvailability();

    // --------------------------------------------------------
    // CHANGE SPOT STRATEGY
    // --------------------------------------------------------

    ParkingSpotStrategy *lowestFloor =
        new LowestFloorSpotStrategy();

    parkingLot.setSpotStrategy(
        lowestFloor);

    // --------------------------------------------------------
    // CHANGE PRICING STRATEGY
    // --------------------------------------------------------

    PricingStrategy *flatPricing =
        new FlatPricingStrategy();

    parkingLot.setPricingStrategy(
        flatPricing);

    Vehicle *car3 =
        new Vehicle(
            "UP70-CAR-03",
            CAR);

    Ticket *carTicket3 =
        parkingLot.parkVehicle(
            car3);

    PaymentStrategy *card =
        new CardPayment();

    parkingLot.unparkVehicle(
        carTicket3,
        card);

    return 0;
}