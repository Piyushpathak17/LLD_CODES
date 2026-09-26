#include <iostream>
#include<vector>
#include <set>

using namespace std;

// ============================================================
// ENUMS
// ============================================================

enum VehicleType {
    CAR,
    SUV,
    BIKE
};

enum BookingStatus {
    CONFIRMED,
    CANCELLED,
    COMPLETED
};

// ============================================================
// CUSTOMER
// ============================================================

class Customer {
    int id;
    string name;
    string phone;
    string licenseNumber;

public:

    Customer(
        int id,
        string name,
        string phone,
        string licenseNumber
    ) {
        this->id = id;
        this->name = name;
        this->phone = phone;
        this->licenseNumber = licenseNumber;
    }

    int getId() {
        return id;
    }

    string getName() {
        return name;
    }

    string getPhone() {
        return phone;
    }

    string getLicenseNumber() {
        return licenseNumber;
    }
};

// ============================================================
// VEHICLE
// ============================================================

class Vehicle {
    int id;
    string registrationNumber;
    VehicleType type;
    double pricePerDay;

public:

    Vehicle(
        int id,
        string registrationNumber,
        VehicleType type,
        double pricePerDay
    ) {
        this->id = id;
        this->registrationNumber =
            registrationNumber;

        this->type = type;
        this->pricePerDay = pricePerDay;
    }

    int getId() {
        return id;
    }

    string getRegistrationNumber() {
        return registrationNumber;
    }

    VehicleType getType() {
        return type;
    }

    double getPricePerDay() {
        return pricePerDay;
    }
};

// ============================================================
// BRANCH
// ============================================================

class Branch {

    int id;
    string name;
    string location;

    vector<Vehicle*> vehicles;

public:

    Branch(
        int id,
        string name,
        string location
    ) {
        this->id = id;
        this->name = name;
        this->location = location;
    }

    int getId() {
        return id;
    }

    string getName() {
        return name;
    }

    string getLocation() {
        return location;
    }

    void addVehicle(
        Vehicle* vehicle
    ) {
        vehicles.push_back(vehicle);
    }

    vector<Vehicle*>& getVehicles() {
        return vehicles;
    }

    Vehicle* getVehicleById(
        int vehicleId
    ) {

        for (auto vehicle : vehicles) {

            if (
                vehicle->getId()
                == vehicleId
            ) {
                return vehicle;
            }
        }

        return nullptr;
    }
};

// ============================================================
// BOOKING
// ============================================================

class Booking {

    int id;

    Customer* customer;
    Vehicle* vehicle;
    Branch* branch;

    int startDate;
    int endDate;

    BookingStatus status;

public:

    Booking(
        int id,
        Customer* customer,
        Vehicle* vehicle,
        Branch* branch,
        int startDate,
        int endDate
    ) {
        this->id = id;

        this->customer = customer;
        this->vehicle = vehicle;
        this->branch = branch;

        this->startDate = startDate;
        this->endDate = endDate;

        this->status = CONFIRMED;
    }

    int getId() {
        return id;
    }

    Customer* getCustomer() {
        return customer;
    }

    Vehicle* getVehicle() {
        return vehicle;
    }

    Branch* getBranch() {
        return branch;
    }

    int getStartDate() {
        return startDate;
    }

    int getEndDate() {
        return endDate;
    }

    BookingStatus getStatus() {
        return status;
    }

    void cancel() {
        status = CANCELLED;
    }

    void complete() {
        status = COMPLETED;
    }

    // --------------------------------------------------------
    // Check whether this booking overlaps with new interval
    // --------------------------------------------------------

    bool overlaps(
        int newStart,
        int newEnd
    ) {

        if (status == CANCELLED)
            return false;

        return newStart <= endDate &&
               newEnd >= startDate;
    }
};

// ============================================================
// PAYMENT STRATEGY
// ============================================================

class PaymentStrategy {

public:

    virtual bool pay(
        double amount
    ) = 0;

    virtual ~PaymentStrategy() {}
};

// ============================================================
// CARD PAYMENT
// ============================================================

class CardPayment
    : public PaymentStrategy {

public:

    bool pay(
        double amount
    ) override {

        cout << "Paid Rs."
             << amount
             << " using Card\n";

        return true;
    }
};

// ============================================================
// UPI PAYMENT
// ============================================================

class UPIPayment
    : public PaymentStrategy {

public:

    bool pay(
        double amount
    ) override {

        cout << "Paid Rs."
             << amount
             << " using UPI\n";

        return true;
    }
};

// ============================================================
// CASH PAYMENT
// ============================================================

class CashPayment
    : public PaymentStrategy {

public:

    bool pay(
        double amount
    ) override {

        cout << "Paid Rs."
             << amount
             << " using Cash\n";

        return true;
    }
};

// ============================================================
// PRICING STRATEGY
// ============================================================

class PricingStrategy {

public:

    virtual double calculatePrice(
        Vehicle* vehicle,
        int startDate,
        int endDate
    ) = 0;

    virtual ~PricingStrategy() {}
};

// ============================================================
// DAILY PRICING
// ============================================================

class DailyPricingStrategy
    : public PricingStrategy {

public:

    double calculatePrice(
        Vehicle* vehicle,
        int startDate,
        int endDate
    ) override {

        int days =
            endDate - startDate;

        /*
            We consider startDate and endDate
            as rental boundaries.

            Example:
            1 -> 3 = 2 days
        */

        return days *
               vehicle->getPricePerDay();
    }
};

// ============================================================
// WEEKEND PRICING
// ============================================================

class WeekendPricingStrategy
    : public PricingStrategy {

public:

    double calculatePrice(
        Vehicle* vehicle,
        int startDate,
        int endDate
    ) override {

        int days =
            endDate - startDate;

        double price =
            days *
            vehicle->getPricePerDay();

        /*
            Simple example:
            20% additional charge.
        */

        return price * 1.20;
    }
};

// ============================================================
// CAR RENTAL SYSTEM
// ============================================================

class CarRentalSystem {

    vector<Branch*> branches;
    vector<Customer*> customers;
    vector<Booking*> bookings;

    PricingStrategy* pricingStrategy;

    int nextBookingId;

public:

    CarRentalSystem(
        PricingStrategy* pricingStrategy
    ) {

        this->pricingStrategy =
            pricingStrategy;

        nextBookingId = 1;
    }

    // --------------------------------------------------------
    // ADD BRANCH
    // --------------------------------------------------------

    void addBranch(
        Branch* branch
    ) {

        branches.push_back(
            branch
        );
    }

    // --------------------------------------------------------
    // ADD CUSTOMER
    // --------------------------------------------------------

    void addCustomer(
        Customer* customer
    ) {

        customers.push_back(
            customer
        );
    }

    // --------------------------------------------------------
    // SET PRICING STRATEGY
    // --------------------------------------------------------

    void setPricingStrategy(
        PricingStrategy* strategy
    ) {

        pricingStrategy =
            strategy;
    }

    // --------------------------------------------------------
    // SEARCH VEHICLE
    // --------------------------------------------------------

    vector<Vehicle*> searchVehicles(
        VehicleType type,
        int startDate,
        int endDate
    ) {

        vector<Vehicle*> result;

        for (auto branch : branches) {

            for (
                auto vehicle :
                branch->getVehicles()
            ) {

                if (
                    vehicle->getType()
                    != type
                ) {
                    continue;
                }

                if (
                    isVehicleAvailable(
                        vehicle,
                        startDate,
                        endDate
                    )
                ) {

                    result.push_back(
                        vehicle
                    );
                }
            }
        }

        return result;
    }

    // --------------------------------------------------------
    // CHECK VEHICLE AVAILABILITY
    // --------------------------------------------------------

    bool isVehicleAvailable(
        Vehicle* vehicle,
        int startDate,
        int endDate
    ) {

        for (auto booking : bookings) {

            if (
                booking->getVehicle()
                != vehicle
            ) {
                continue;
            }

            if (
                booking->overlaps(
                    startDate,
                    endDate
                )
            ) {

                return false;
            }
        }

        return true;
    }

    // --------------------------------------------------------
    // FIND BRANCH OF VEHICLE
    // --------------------------------------------------------

    Branch* findVehicleBranch(
        Vehicle* vehicle
    ) {

        for (auto branch : branches) {

            for (
                auto current :
                branch->getVehicles()
            ) {

                if (current == vehicle)
                    return branch;
            }
        }

        return nullptr;
    }

    // --------------------------------------------------------
    // CREATE BOOKING
    // --------------------------------------------------------

    Booking* createBooking(
        Customer* customer,
        Vehicle* vehicle,
        int startDate,
        int endDate
    ) {

        if (customer == nullptr) {

            cout << "Invalid customer\n";
            return nullptr;
        }

        if (vehicle == nullptr) {

            cout << "Invalid vehicle\n";
            return nullptr;
        }

        if (
            startDate >= endDate
        ) {

            cout << "Invalid dates\n";
            return nullptr;
        }

        // ----------------------------------------------------
        // Check availability
        // ----------------------------------------------------

        if (
            !isVehicleAvailable(
                vehicle,
                startDate,
                endDate
            )
        ) {

            cout << "Vehicle is not available\n";

            return nullptr;
        }

        // ----------------------------------------------------
        // Find branch
        // ----------------------------------------------------

        Branch* branch =
            findVehicleBranch(
                vehicle
            );

        if (branch == nullptr) {

            cout << "Vehicle does not belong to system\n";

            return nullptr;
        }

        // ----------------------------------------------------
        // Create booking
        // ----------------------------------------------------

        Booking* booking =
            new Booking(
                nextBookingId++,
                customer,
                vehicle,
                branch,
                startDate,
                endDate
            );

        bookings.push_back(
            booking
        );

        cout << "\nBooking created successfully\n";

        cout << "Booking ID: "
             << booking->getId()
             << "\n";

        cout << "Vehicle: "
             << vehicle
                    ->getRegistrationNumber()
             << "\n";

        cout << "Branch: "
             << branch->getName()
             << "\n";

        return booking;
    }

    // --------------------------------------------------------
    // CALCULATE PRICE
    // --------------------------------------------------------

    double calculatePrice(
        Booking* booking
    ) {

        if (booking == nullptr)
            return 0;

        return pricingStrategy
            ->calculatePrice(
                booking->getVehicle(),
                booking->getStartDate(),
                booking->getEndDate()
            );
    }

    // --------------------------------------------------------
    // COMPLETE RENTAL
    // --------------------------------------------------------

    bool completeRental(
        Booking* booking,
        PaymentStrategy* paymentStrategy
    ) {

        if (booking == nullptr) {

            cout << "Invalid booking\n";
            return false;
        }

        if (
            booking->getStatus()
            != CONFIRMED
        ) {

            cout << "Booking is not active\n";
            return false;
        }

        double amount =
            calculatePrice(
                booking
            );

        cout << "\nRental amount: Rs."
             << amount
             << "\n";

        if (
            !paymentStrategy->pay(
                amount
            )
        ) {

            cout << "Payment failed\n";
            return false;
        }

        booking->complete();

        cout << "Rental completed successfully\n";

        return true;
    }

    // --------------------------------------------------------
    // CANCEL BOOKING
    // --------------------------------------------------------

    bool cancelBooking(
        Booking* booking
    ) {

        if (booking == nullptr)
            return false;

        if (
            booking->getStatus()
            != CONFIRMED
        ) {

            cout << "Booking cannot be cancelled\n";
            return false;
        }

        booking->cancel();

        cout << "Booking "
             << booking->getId()
             << " cancelled\n";

        return true;
    }

    // --------------------------------------------------------
    // DISPLAY AVAILABLE VEHICLES
    // --------------------------------------------------------

    void displayAvailableVehicles(
        VehicleType type,
        int startDate,
        int endDate
    ) {

        vector<Vehicle*> vehicles =
            searchVehicles(
                type,
                startDate,
                endDate
            );

        cout << "\n===== AVAILABLE VEHICLES =====\n";

        if (vehicles.empty()) {

            cout << "No vehicles available\n";
            return;
        }

        for (auto vehicle : vehicles) {

            cout << "Vehicle ID: "
                 << vehicle->getId()
                 << " | Number: "
                 << vehicle
                        ->getRegistrationNumber()
                 << " | Price/day: Rs."
                 << vehicle->getPricePerDay()
                 << "\n";
        }
    }
};

// ============================================================
// MAIN
// ============================================================

int main() {

    // ========================================================
    // CREATE VEHICLES
    // ========================================================

    Vehicle* car1 =
        new Vehicle(
            1,
            "UP70-AB-1234",
            CAR,
            2000
        );

    Vehicle* car2 =
        new Vehicle(
            2,
            "UP70-CD-5678",
            CAR,
            2500
        );

    Vehicle* suv1 =
        new Vehicle(
            3,
            "UP70-SUV-1111",
            SUV,
            4000
        );

    Vehicle* bike1 =
        new Vehicle(
            4,
            "UP70-BIKE-2222",
            BIKE,
            800
        );

    // ========================================================
    // CREATE BRANCHES
    // ========================================================

    Branch* branch1 =
        new Branch(
            1,
            "Civil Lines",
            "Prayagraj"
        );

    Branch* branch2 =
        new Branch(
            2,
            "Airport Branch",
            "Prayagraj"
        );

    // Add vehicles directly to branches.
    // No VehicleInventory class.

    branch1->addVehicle(car1);
    branch1->addVehicle(car2);

    branch2->addVehicle(suv1);
    branch2->addVehicle(bike1);

    // ========================================================
    // CREATE CUSTOMER
    // ========================================================

    Customer* customer =
        new Customer(
            101,
            "Rahul",
            "9876543210",
            "DL123456"
        );

    // ========================================================
    // PRICING STRATEGY
    // ========================================================

    PricingStrategy* pricing =
        new DailyPricingStrategy();

    // ========================================================
    // CAR RENTAL SYSTEM
    // ========================================================

    CarRentalSystem rentalSystem(
        pricing
    );

    rentalSystem.addBranch(
        branch1
    );

    rentalSystem.addBranch(
        branch2
    );

    rentalSystem.addCustomer(
        customer
    );

    // ========================================================
    // SEARCH
    // ========================================================

    /*
        Customer wants a CAR from day 1 to day 5.

        Rental duration = 5 - 1 = 4 days.
    */

    rentalSystem.displayAvailableVehicles(
        CAR,
        1,
        5
    );

    // ========================================================
    // CREATE BOOKING
    // ========================================================

    Booking* booking1 =
        rentalSystem.createBooking(
            customer,
            car1,
            1,
            5
        );

    // ========================================================
    // TRY TO BOOK SAME CAR OVERLAPPING DATES
    // ========================================================

    Booking* booking2 =
        rentalSystem.createBooking(
            customer,
            car1,
            4,
            8
        );

    /*
        This should fail because:

        Existing:
        1 -------- 5

        New:
            4 -------- 8

        They overlap.
    */

    // ========================================================
    // SEARCH AGAIN
    // ========================================================

    rentalSystem.displayAvailableVehicles(
        CAR,
        4,
        8
    );

    // car1 should not appear because it is booked.
    // car2 can still appear.

    // ========================================================
    // CALCULATE PRICE
    // ========================================================

    if (booking1 != nullptr) {

        double price =
            rentalSystem.calculatePrice(
                booking1
            );

        cout << "\nBooking price: Rs."
             << price
             << "\n";
    }

    // ========================================================
    // PAYMENT
    // ========================================================

    PaymentStrategy* upi =
        new UPIPayment();

    // ========================================================
    // COMPLETE RENTAL
    // ========================================================

    if (booking1 != nullptr) {

        rentalSystem.completeRental(
            booking1,
            upi
        );
    }

    // ========================================================
    // CHANGE PRICING STRATEGY
    // ========================================================

    PricingStrategy* weekendPricing =
        new WeekendPricingStrategy();

    rentalSystem.setPricingStrategy(
        weekendPricing
    );

    // ========================================================
    // CREATE ANOTHER BOOKING
    // ========================================================

    Booking* booking3 =
        rentalSystem.createBooking(
            customer,
            car2,
            10,
            13
        );

    if (booking3 != nullptr) {

        double price =
            rentalSystem.calculatePrice(
                booking3
            );

        cout << "\nWeekend pricing: Rs."
             << price
             << "\n";

        PaymentStrategy* card =
            new CardPayment();

        rentalSystem.completeRental(
            booking3,
            card
        );
    }

    return 0;
}