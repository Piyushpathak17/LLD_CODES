#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <set>
using namespace std;

// ============================================================
// ENUMS
// ============================================================

enum SeatType {
    REGULAR,
    PREMIUM,
    RECLINER
};

enum SeatStatus {
    AVAILABLE,
    LOCKED,
    BOOKED
};

enum BookingStatus {
    CREATED,
    PAYMENT_PENDING,
    CONFIRMED,
    CANCELLED,
    FAILED
};

// ============================================================
// USER
// ============================================================

class User {
    int id;
    string name;

public:
    User(int id, string name) {
        this->id = id;
        this->name = name;
    }

    int getId() {
        return id;
    }

    string getName() {
        return name;
    }
};

// ============================================================
// MOVIE
// ============================================================

class Movie {
    int id;
    string title;
    int durationMinutes;
    string language;

public:
    Movie(
        int id,
        string title,
        int durationMinutes,
        string language
    ) {
        this->id = id;
        this->title = title;
        this->durationMinutes = durationMinutes;
        this->language = language;
    }

    int getId() {
        return id;
    }

    string getTitle() {
        return title;
    }

    int getDurationMinutes() {
        return durationMinutes;
    }

    string getLanguage() {
        return language;
    }
};

// ============================================================
// SEAT
// Physical seat only
// No booked/available state here
// ============================================================

class Seat {
    int id;
    string row;
    int number;
    SeatType type;

public:
    Seat(
        int id,
        string row,
        int number,
        SeatType type
    ) {
        this->id = id;
        this->row = row;
        this->number = number;
        this->type = type;
    }

    int getId() {
        return id;
    }

    string getRow() {
        return row;
    }

    int getNumber() {
        return number;
    }

    SeatType getType() {
        return type;
    }

    string getSeatName() {
        return row + to_string(number);
    }
};

// ============================================================
// SCREEN
// ============================================================

class Screen {
    int id;
    string name;

    vector<Seat*> seats;

public:
    Screen(int id, string name) {
        this->id = id;
        this->name = name;
    }

    int getId() {
        return id;
    }

    string getName() {
        return name;
    }

    void addSeat(Seat* seat) {
        seats.push_back(seat);
    }

    vector<Seat*>& getSeats() {
        return seats;
    }
};

// ============================================================
// THEATRE
// ============================================================

class Theatre {
    int id;
    string name;
    string city;

    vector<Screen*> screens;

public:
    Theatre(
        int id,
        string name,
        string city
    ) {
        this->id = id;
        this->name = name;
        this->city = city;
    }

    int getId() {
        return id;
    }

    string getName() {
        return name;
    }

    string getCity() {
        return city;
    }

    void addScreen(Screen* screen) {
        screens.push_back(screen);
    }

    vector<Screen*>& getScreens() {
        return screens;
    }
};

// ============================================================
// SHOW SEAT
// Seat state for one specific show
// ============================================================

class ShowSeat {
    Seat* seat;
    SeatStatus status;
    double price;

    User* lockedBy;
    long long lockTime;
    int lockDurationSeconds;

public:
    ShowSeat(
        Seat* seat,
        double price
    ) {
        this->seat = seat;
        this->price = price;

        this->status = AVAILABLE;

        this->lockedBy = nullptr;
        this->lockTime = 0;

        this->lockDurationSeconds = 300;
    }

    Seat* getSeat() {
        return seat;
    }

    double getPrice() {
        return price;
    }

    SeatStatus getStatus() {
        /*
            If lock expired,
            automatically release it.
        */

        if (status == LOCKED &&
            isLockExpired()) {

            releaseLock();
        }

        return status;
    }

    bool isAvailable() {
        return getStatus() == AVAILABLE;
    }

    bool lockSeat(User* user) {

        if (!isAvailable())
            return false;

        status = LOCKED;

        lockedBy = user;
        lockTime = time(nullptr);

        return true;
    }

    bool isLockedBy(User* user) {

        if (getStatus() != LOCKED)
            return false;

        return lockedBy != nullptr &&
               lockedBy->getId() == user->getId();
    }

    void bookSeat(User* user) {

        if (!isLockedBy(user))
            return;

        status = BOOKED;

        lockedBy = nullptr;
        lockTime = 0;
    }

    void releaseLock() {

        if (status == LOCKED) {
            status = AVAILABLE;

            lockedBy = nullptr;
            lockTime = 0;
        }
    }

    void makeAvailable() {

        status = AVAILABLE;

        lockedBy = nullptr;
        lockTime = 0;
    }

private:

    bool isLockExpired() {

        if (status != LOCKED)
            return false;

        long long now =
            time(nullptr);

        return
            now - lockTime
            >= lockDurationSeconds;
    }
};

// ============================================================
// SHOW
// ============================================================

class Show {
    int id;

    Movie* movie;
    Theatre* theatre;
    Screen* screen;

    long long startTime;

    vector<ShowSeat*> showSeats;

public:
    Show(
        int id,
        Movie* movie,
        Theatre* theatre,
        Screen* screen,
        long long startTime
    ) {
        this->id = id;
        this->movie = movie;
        this->theatre = theatre;
        this->screen = screen;
        this->startTime = startTime;

        initializeShowSeats();
    }

    int getId() {
        return id;
    }

    Movie* getMovie() {
        return movie;
    }

    Theatre* getTheatre() {
        return theatre;
    }

    Screen* getScreen() {
        return screen;
    }

    long long getStartTime() {
        return startTime;
    }

    vector<ShowSeat*>& getShowSeats() {
        return showSeats;
    }

    ShowSeat* getShowSeatById(
        int seatId
    ) {

        for (auto showSeat : showSeats) {

            if (showSeat
                    ->getSeat()
                    ->getId()
                == seatId) {

                return showSeat;
            }
        }

        return nullptr;
    }

    vector<ShowSeat*> getAvailableSeats() {

        vector<ShowSeat*> result;

        for (auto seat : showSeats) {

            if (seat->isAvailable()) {
                result.push_back(seat);
            }
        }

        return result;
    }

private:

    void initializeShowSeats() {

        for (auto seat :
             screen->getSeats()) {

            double price = 0;

            if (seat->getType() == REGULAR)
                price = 200;

            else if (seat->getType() == PREMIUM)
                price = 300;

            else if (seat->getType() == RECLINER)
                price = 500;

            showSeats.push_back(
                new ShowSeat(
                    seat,
                    price
                )
            );
        }
    }
};

// ============================================================
// BOOKING
// ============================================================

class Booking {
    int id;

    User* user;
    Show* show;

    vector<ShowSeat*> seats;

    double amount;

    BookingStatus status;

public:
    Booking(
        int id,
        User* user,
        Show* show,
        vector<ShowSeat*> seats,
        double amount
    ) {
        this->id = id;
        this->user = user;
        this->show = show;
        this->seats = seats;
        this->amount = amount;

        this->status = PAYMENT_PENDING;
    }

    int getId() {
        return id;
    }

    User* getUser() {
        return user;
    }

    Show* getShow() {
        return show;
    }

    vector<ShowSeat*>& getSeats() {
        return seats;
    }

    double getAmount() {
        return amount;
    }

    BookingStatus getStatus() {
        return status;
    }

    void confirm() {
        status = CONFIRMED;
    }

    void cancel() {
        status = CANCELLED;
    }

    void fail() {
        status = FAILED;
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
// UPI PAYMENT
// ============================================================

class UPIPayment
    : public PaymentStrategy {

public:

    bool pay(double amount) override {

        cout << "Paid Rs."
             << amount
             << " using UPI\n";

        return true;
    }
};

// ============================================================
// CARD PAYMENT
// ============================================================

class CardPayment
    : public PaymentStrategy {

public:

    bool pay(double amount) override {

        cout << "Paid Rs."
             << amount
             << " using Card\n";

        return true;
    }
};

// ============================================================
// BOOK MY SHOW SYSTEM
// ============================================================

class BookMyShowSystem {

    vector<Movie*> movies;
    vector<Theatre*> theatres;
    vector<Show*> shows;
    vector<Booking*> bookings;

    int nextBookingId;

public:

    BookMyShowSystem() {
        nextBookingId = 1;
    }
    // --------------------------------------------------------
    // Add entities
    // --------------------------------------------------------

    void addMovie(Movie* movie) {
        movies.push_back(movie);
    }

    void addTheatre(Theatre* theatre) {
        theatres.push_back(theatre);
    }

    void addShow(Show* show) {
        shows.push_back(show);
    }

    // --------------------------------------------------------
    // Search movies by city
    // --------------------------------------------------------

    vector<Movie*> searchMovies(
        string city
    ) {

        set<int> movieIds;
        vector<Movie*> result;

        for (auto show : shows) {

            if (show->getTheatre()->getCity()
                != city) {

                continue;
            }

            int movieId =
                show->getMovie()->getId();

            if (movieIds.count(movieId))
                continue;

            movieIds.insert(movieId);

            result.push_back(
                show->getMovie()
            );
        }

        return result;
    }

    // --------------------------------------------------------
    // Get shows for movie + city
    // --------------------------------------------------------

    vector<Show*> getShows(
        Movie* movie,
        string city
    ) {

        vector<Show*> result;

        for (auto show : shows) {

            if (show->getMovie()->getId()
                == movie->getId()
                &&
                show->getTheatre()->getCity()
                == city) {

                result.push_back(show);
            }
        }

        return result;
    }

    // --------------------------------------------------------
    // Display available seats
    // --------------------------------------------------------

    void showAvailableSeats(
        Show* show
    ) {

        cout << "\nAvailable seats:\n";

        for (auto showSeat :
             show->getAvailableSeats()) {

            cout << showSeat
                        ->getSeat()
                        ->getSeatName()
                 << " - Rs."
                 << showSeat->getPrice()
                 << "\n";
        }
    }

    // --------------------------------------------------------
    // Create booking
    // --------------------------------------------------------

    Booking* createBooking(
        User* user,
        Show* show,
        vector<int> seatIds
    ) {

        vector<ShowSeat*> selectedSeats;

        double totalAmount = 0;

        /*
            Step 1:
            Validate + lock seats
        */

        for (int seatId : seatIds) {

            ShowSeat* showSeat =
                show->getShowSeatById(
                    seatId
                );

            if (showSeat == nullptr) {

                cout << "Invalid seat\n";

                releaseSeats(
                    selectedSeats,
                    user
                );

                return nullptr;
            }

            bool locked =
                showSeat->lockSeat(user);

            if (!locked) {

                cout << "Seat "
                     << showSeat
                            ->getSeat()
                            ->getSeatName()
                     << " unavailable\n";

                releaseSeats(
                    selectedSeats,
                    user
                );

                return nullptr;
            }

            selectedSeats.push_back(
                showSeat
            );

            totalAmount +=
                showSeat->getPrice();
        }

        /*
            Step 2:
            Create booking
        */

        Booking* booking =
            new Booking(
                nextBookingId++,
                user,
                show,
                selectedSeats,
                totalAmount
            );

        bookings.push_back(booking);

        cout << "\nBooking created\n";

        cout << "Booking ID: "
             << booking->getId()
             << "\n";

        cout << "Amount: Rs."
             << totalAmount
             << "\n";

        return booking;
    }

    // --------------------------------------------------------
    // Make payment
    // --------------------------------------------------------

    bool makePayment(
        Booking* booking,
        PaymentStrategy* paymentStrategy
    ) {

        if (booking == nullptr)
            return false;

        if (booking->getStatus()
            != PAYMENT_PENDING) {

            cout << "Booking is not awaiting payment\n";

            return false;
        }

        /*
            Make sure locks still belong
            to this user.
        */

        for (auto seat :
             booking->getSeats()) {

            if (!seat->isLockedBy(
                    booking->getUser()
                )) {

                cout << "Seat lock expired\n";

                booking->fail();

                releaseSeats(
                    booking->getSeats(),
                    booking->getUser()
                );

                return false;
            }
        }

        bool success =
            paymentStrategy->pay(
                booking->getAmount()
            );

        if (!success) {

            booking->fail();

            releaseSeats(
                booking->getSeats(),
                booking->getUser()
            );

            return false;
        }

        /*
            Payment successful:
            LOCKED -> BOOKED
        */

        for (auto seat :
             booking->getSeats()) {

            seat->bookSeat(
                booking->getUser()
            );
        }

        booking->confirm();

        cout << "Booking confirmed\n";

        return true;
    }

    // --------------------------------------------------------
    // Cancel confirmed booking
    // --------------------------------------------------------

    bool cancelBooking(
        Booking* booking
    ) {

        if (booking == nullptr)
            return false;

        if (booking->getStatus()
            != CONFIRMED) {

            cout << "Only confirmed booking can be cancelled\n";

            return false;
        }

        for (auto seat :
             booking->getSeats()) {

            seat->makeAvailable();
        }

        booking->cancel();

        cout << "Booking cancelled\n";

        return true;
    }

    // --------------------------------------------------------
    // Display booking
    // --------------------------------------------------------

    void displayBooking(
        Booking* booking
    ) {

        if (booking == nullptr)
            return;

        cout << "\n===== BOOKING =====\n";

        cout << "Booking ID: "
             << booking->getId()
             << "\n";

        cout << "User: "
             << booking->getUser()->getName()
             << "\n";

        cout << "Movie: "
             << booking
                    ->getShow()
                    ->getMovie()
                    ->getTitle()
             << "\n";

        cout << "Theatre: "
             << booking
                    ->getShow()
                    ->getTheatre()
                    ->getName()
             << "\n";

        cout << "Seats: ";

        for (auto seat :
             booking->getSeats()) {

            cout << seat
                        ->getSeat()
                        ->getSeatName()
                 << " ";
        }

        cout << "\nAmount: Rs."
             << booking->getAmount()
             << "\n";

        cout << "Status: ";

        if (booking->getStatus()
            == PAYMENT_PENDING)
            cout << "PAYMENT_PENDING";

        else if (booking->getStatus()
                 == CONFIRMED)
            cout << "CONFIRMED";

        else if (booking->getStatus()
                 == CANCELLED)
            cout << "CANCELLED";

        else
            cout << "FAILED";

        cout << "\n";
    }

private:

    void releaseSeats(
        vector<ShowSeat*>& seats,
        User* user
    ) {

        for (auto seat : seats) {

            if (seat->isLockedBy(user)) {
                seat->releaseLock();
            }
        }
    }
};

// ============================================================
// MAIN
// ============================================================

int main() {

    // --------------------------------------------------------
    // User
    // --------------------------------------------------------

    User* user =
        new User(
            1,
            "Anirudh"
        );

    // --------------------------------------------------------
    // Movie
    // --------------------------------------------------------

    Movie* movie =
        new Movie(
            101,
            "Interstellar",
            169,
            "English"
        );

    // --------------------------------------------------------
    // Theatre
    // --------------------------------------------------------

    Theatre* theatre =
        new Theatre(
            1,
            "PVR",
            "Prayagraj"
        );

    // --------------------------------------------------------
    // Screen
    // --------------------------------------------------------

    Screen* screen =
        new Screen(
            1,
            "Screen 1"
        );

    screen->addSeat(
        new Seat(
            1,
            "A",
            1,
            REGULAR
        )
    );

    screen->addSeat(
        new Seat(
            2,
            "A",
            2,
            REGULAR
        )
    );

    screen->addSeat(
        new Seat(
            3,
            "B",
            1,
            PREMIUM
        )
    );

    screen->addSeat(
        new Seat(
            4,
            "B",
            2,
            PREMIUM
        )
    );

    screen->addSeat(
        new Seat(
            5,
            "C",
            1,
            RECLINER
        )
    );

    theatre->addScreen(screen);

    // --------------------------------------------------------
    // Show
    // --------------------------------------------------------

    long long showTime =
        time(nullptr) + 3600;

    Show* show =
        new Show(
            1,
            movie,
            theatre,
            screen,
            showTime
        );

    // --------------------------------------------------------
    // System
    // --------------------------------------------------------

    BookMyShowSystem system;

    system.addMovie(movie);
    system.addTheatre(theatre);
    system.addShow(show);

    // --------------------------------------------------------
    // Show available seats
    // --------------------------------------------------------

    system.showAvailableSeats(
        show
    );

    // --------------------------------------------------------
    // User selects A1 + A2
    // --------------------------------------------------------

    vector<int> selectedSeatIds = {
        1,
        2
    };

    Booking* booking =
        system.createBooking(
            user,
            show,
            selectedSeatIds
        );

    // --------------------------------------------------------
    // Payment
    // --------------------------------------------------------

    PaymentStrategy* upi =
        new UPIPayment();

    system.makePayment(
        booking,
        upi
    );

    // --------------------------------------------------------
    // Show booking
    // --------------------------------------------------------

    system.displayBooking(
        booking
    );

    // --------------------------------------------------------
    // Availability after booking
    // --------------------------------------------------------

    system.showAvailableSeats(
        show
    );

    // --------------------------------------------------------
    // Cancel booking
    // --------------------------------------------------------

    system.cancelBooking(
        booking
    );

    // --------------------------------------------------------
    // Availability after cancellation
    // --------------------------------------------------------

    system.showAvailableSeats(
        show
    );

    return 0;
}