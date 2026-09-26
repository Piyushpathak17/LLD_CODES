#include <iostream>
#include <vector>
#include <map>
#include <string>

using namespace std;

enum AttendanceStatus {
    PRESENT,
    ABSENT,
    LATE
};

// ============================================================
// STUDENT
// ============================================================
class Student {
    int id;
    string name;

public:
    Student(int id, string name) {
        this->id = id;
        this->name = name;
    }

    int getId() { return id; }
    string getName() { return name; }
};

// ============================================================
// COURSE
// ============================================================
class Course {
    int id;
    string name;
    vector<Student*> students;

public:
    Course(int id, string name) {
        this->id = id;
        this->name = name;
    }

    int getId() { return id; }
    string getName() { return name; }

    void enrollStudent(Student* student) {
        students.push_back(student);
    }

    vector<Student*>& getStudents() { return students; }

    bool isStudentEnrolled(int studentId) {
        for (auto student : students) {
            if (student->getId() == studentId) return true;
        }
        return false;
    }
};

// ============================================================
// ATTENDANCE RECORD
// ============================================================
class AttendanceRecord {
    Student* student;
    AttendanceStatus status;

public:
    AttendanceRecord(Student* student, AttendanceStatus status) {
        this->student = student;
        this->status = status;
    }

    Student* getStudent() { return student; }
    AttendanceStatus getStatus() { return status; }
    void setStatus(AttendanceStatus status) { this->status = status; }
};

// ============================================================
// CLASS SESSION
// ============================================================
class ClassSession {
    int id;
    Course* course;
    string date;
    vector<AttendanceRecord*> records;

public:
    ClassSession(int id, Course* course, string date) {
        this->id = id;
        this->course = course;
        this->date = date;
    }

    int getId() { return id; }
    Course* getCourse() { return course; }
    string getDate() { return date; }

    void addRecord(AttendanceRecord* record) { records.push_back(record); }
    vector<AttendanceRecord*>& getRecords() { return records; }

    AttendanceRecord* getRecordForStudent(int studentId) {
        for (auto record : records) {
            if (record->getStudent()->getId() == studentId) {
                return record;
            }
        }
        return nullptr;
    }
};

// ============================================================
// OBSERVER PATTERN (NOTIFICATIONS)
// ============================================================
class LowAttendanceObserver {
public:
    virtual void onLowAttendance(Student* student, Course* course, double percentage) = 0;
    virtual ~LowAttendanceObserver() {}
};

class EmailNotifier : public LowAttendanceObserver {
public:
    void onLowAttendance(Student* student, Course* course, double percentage) override {
        cout << "[EMAIL ALERT] Parent of " << student->getName() 
             << ": Attendance in " << course->getName() 
             << " is critically low at " << percentage << "%!\n";
    }
};

class SMSNotifier : public LowAttendanceObserver {
public:
    void onLowAttendance(Student* student, Course* course, double percentage) override {
        cout << "[SMS ALERT] Hi " << student->getName() 
             << ", your attendance in " << course->getName() 
             << " has dropped to " << percentage << "% (Below 75%).\n";
    }
};

// ============================================================
// STRATEGY PATTERN (ATTENDANCE MARKING)
// ============================================================
class AttendanceMarkingStrategy {
public:
    virtual bool markAttendance(ClassSession* session, Student* student, AttendanceStatus status) = 0;
    virtual ~AttendanceMarkingStrategy() {}
};

class ManualAttendanceStrategy : public AttendanceMarkingStrategy {
public:
    bool markAttendance(ClassSession* session, Student* student, AttendanceStatus status) override {
        AttendanceRecord* existing = session->getRecordForStudent(student->getId());
        if (existing != nullptr) {
            existing->setStatus(status);
            return true;
        }
        session->addRecord(new AttendanceRecord(student, status));
        return true;
    }
};

class QRAttendanceStrategy : public AttendanceMarkingStrategy {
public:
    bool markAttendance(ClassSession* session, Student* student, AttendanceStatus status) override {
        cout << "[System] QR validated successfully for " << student->getName() << "\n";
        AttendanceRecord* existing = session->getRecordForStudent(student->getId());
        if (existing != nullptr) {
            existing->setStatus(status);
            return true;
        }
        session->addRecord(new AttendanceRecord(student, status));
        return true;
    }
};

// ============================================================
// CORE SYSTEM (FACADE / CONTROLLER)
// ============================================================
class AttendanceSystem {
    vector<Student*> students;
    vector<Course*> courses;
    vector<ClassSession*> sessions;
    
    AttendanceMarkingStrategy* markingStrategy;
    vector<LowAttendanceObserver*> observers;

    // Caching maps for O(1) percentage calculation
    map<int, int> courseTotalSessions; // courseId -> total sessions
    map<int, map<int, int>> studentAttendedSessions; // courseId -> (studentId -> attended sessions)

    int nextSessionId = 1;

public:
    AttendanceSystem(AttendanceMarkingStrategy* strategy) {
        this->markingStrategy = strategy;
    }

    void setMarkingStrategy(AttendanceMarkingStrategy* strategy) {
        markingStrategy = strategy;
    }

    void addObserver(LowAttendanceObserver* observer) {
        observers.push_back(observer);
    }

    void addStudent(Student* student) { students.push_back(student); }
    void addCourse(Course* course) { courses.push_back(course); }
    void enrollStudent(Course* course, Student* student) { course->enrollStudent(student); }

    ClassSession* createSession(Course* course, string date) {
        ClassSession* session = new ClassSession(nextSessionId++, course, date);
        sessions.push_back(session);
        
        // Update cache: Course has one more session
        courseTotalSessions[course->getId()]++;

        cout << "\n--- Session created for " << course->getName() << " on " << date << " ---\n";
        return session;
    }

    bool markAttendance(ClassSession* session, Student* student, AttendanceStatus status) {
        if (!session->getCourse()->isStudentEnrolled(student->getId())) {
            cout << "Error: Student not enrolled in course\n";
            return false;
        }

        // Capture previous state before applying new mark
        AttendanceRecord* existingRecord = session->getRecordForStudent(student->getId());
        bool wasPresentOrLate = (existingRecord != nullptr && 
                                (existingRecord->getStatus() == PRESENT || existingRecord->getStatus() == LATE));

        // Apply strategy
        bool success = markingStrategy->markAttendance(session, student, status);

        if (success) {
            bool isNowPresentOrLate = (status == PRESENT || status == LATE);
            int cId = session->getCourse()->getId();
            int sId = student->getId();

            // Update cache O(1)
            if (!wasPresentOrLate && isNowPresentOrLate) {
                studentAttendedSessions[cId][sId]++;
            } else if (wasPresentOrLate && !isNowPresentOrLate) {
                studentAttendedSessions[cId][sId]--;
            }

            // Trigger Observers if below 75%
            double currentPercentage = getAttendancePercentage(session->getCourse(), student);
            if (currentPercentage < 75.0) {
                for (auto obs : observers) {
                    obs->onLowAttendance(student, session->getCourse(), currentPercentage);
                }
            }
        }
        return success;
    }

    double getAttendancePercentage(Course* course, Student* student) {
        int total = courseTotalSessions[course->getId()];
        if (total == 0) return 100.0; // Assume 100% if no classes have happened yet

        int attended = studentAttendedSessions[course->getId()][student->getId()];
        return (attended * 100.0) / total;
    }

    void showSessionAttendance(ClassSession* session) {
        cout << "\nAttendance Log for " << session->getCourse()->getName() << " (" << session->getDate() << ")\n";
        for (auto record : session->getRecords()) {
            cout << "- " << record->getStudent()->getName() << " : ";
            if (record->getStatus() == PRESENT) cout << "PRESENT\n";
            else if (record->getStatus() == ABSENT) cout << "ABSENT\n";
            else cout << "LATE\n";
        }
    }
};

// ============================================================
// MAIN EXECUTION
// ============================================================
int main() {
    // 1. Create Core Data
    Student* s1 = new Student(1, "Anirudh");
    Student* s2 = new Student(2, "Rahul");
    Student* s3 = new Student(3, "Aman");

    Course* cpp = new Course(101, "C++ Programming");

    // 2. Setup System with Manual Strategy
    AttendanceMarkingStrategy* manual = new ManualAttendanceStrategy();
    AttendanceSystem system(manual);

    system.addStudent(s1);
    system.addStudent(s2);
    system.addStudent(s3);
    system.addCourse(cpp);
    
    system.enrollStudent(cpp, s1);
    system.enrollStudent(cpp, s2);
    system.enrollStudent(cpp, s3);

    // 3. Attach Notification Observers
    LowAttendanceObserver* emailAlert = new EmailNotifier();
    LowAttendanceObserver* smsAlert = new SMSNotifier();
    system.addObserver(emailAlert);
    system.addObserver(smsAlert);

    // 4. Simulate Classes
    ClassSession* session1 = system.createSession(cpp, "13-09-2026");
    system.markAttendance(session1, s1, PRESENT);
    system.markAttendance(session1, s2, PRESENT); 
    system.markAttendance(session1, s3, LATE);
    
    system.showSessionAttendance(session1);

    ClassSession* session2 = system.createSession(cpp, "14-09-2026");
    system.markAttendance(session2, s1, PRESENT);
    system.markAttendance(session2, s2, ABSENT); // Rahul misses class
    system.markAttendance(session2, s3, PRESENT);

    ClassSession* session3 = system.createSession(cpp, "15-09-2026");
    system.markAttendance(session3, s1, PRESENT);
    system.markAttendance(session3, s2, ABSENT); // Rahul misses again (Drops to 33.3%, triggers alerts!)
    system.markAttendance(session3, s3, ABSENT); 

    // 5. Change marking strategy dynamically for the next class
    AttendanceMarkingStrategy* qr = new QRAttendanceStrategy();
    system.setMarkingStrategy(qr);

    ClassSession* session4 = system.createSession(cpp, "16-09-2026");
    system.markAttendance(session4, s1, PRESENT);

    // Print final percentages using the O(1) cache
    cout << "\n--- Final Attendance Percentages ---\n";
    cout << s1->getName() << " = " << system.getAttendancePercentage(cpp, s1) << "%\n";
    cout << s2->getName() << " = " << system.getAttendancePercentage(cpp, s2) << "%\n";
    cout << s3->getName() << " = " << system.getAttendancePercentage(cpp, s3) << "%\n";

    return 0;
}