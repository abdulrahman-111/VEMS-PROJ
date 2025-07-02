#ifndef VIRTUALEVENT_H
#define VIRTUALEVENT_H

// ==================== INCLUDES ====================
#include <iostream>
#include <vector>
#include <string>
#include <ctime>

#include "event_dbmanager.h"          // Handles event database interactions
#include "users.dbmanger.h"           // Manages user database operations
class user;                           // Forward declaration for user class

using namespace std;

// ==================== ENUMERATIONS ====================
// Types of events supported by the system
enum eventtype { Workshop, Webinar, Conference };

// ==================== STRUCTURES ====================
// Represents an attendee’s core information
struct attendee {
    std::string name;
    std::string email;
    int password;
    std::string affiliation;
};

// ==================== VIRTUAL EVENT CLASS ====================
// Abstract base class representing a generic virtual event
class virtualevent {
    // ==================== MEMBER VARIABLES ====================
    std::string name;                      // Event name
    std::string description;               // Event description
    std::string organizer;                 // Organizer name
    std::tm datetime;                      // Event date and time
    std::string platform;                  // Platform (e.g., Zoom, Teams)
    unsigned int capacity;                 // Maximum number of attendees
    unsigned int event_id;                 // Unique event ID
    double overall_rating;                 // Calculated overall rating
    int org_id;                            // Organizer ID
    vector<user> attendees;                // List of registered attendees
    vector<user> waitingList;              // List of users on the waiting list
    vector<feedback> event_feedbacks;      // List of feedback for this event

public:
    // ==================== CONSTRUCTOR/DESTRUCTOR ====================
    virtualevent() {}

    // Parameterized constructor to initialize all main fields
    virtualevent(const std::string& n, unsigned int id,
                 unsigned int orgid,
                 const std::string& desc,
                 const std::tm& date,
                 const std::string& plat,
                 unsigned int cap,
                 const std::string& org);

    virtual ~virtualevent() {}

    // ==================== FACTORY METHOD ====================
    // Creates and returns a new event object based on the event type
    static virtualevent* build(eventtype eventtype,
                               const std::string& n,
                               unsigned int id,
                               unsigned orgid,
                               const std::string& desc,
                               const std::tm& date,
                               const std::string& plat,
                               unsigned int cap,
                               const std::string& org);

    // ==================== PURE VIRTUAL METHODS ====================
    // Implemented by derived event types
    virtual std::string geteventdetails() const ;
    virtual void sendReminder() const = 0;
    virtual void getter() = 0;
    virtual void convertetoobject(event_db e) = 0;
    virtual event_db convertetostruct() = 0;

    // ==================== EVENT MANAGEMENT ====================
    void set_id(int id) { event_id = id; }                        // Set event ID
    bool registerAttendee(user& a);                               // Register a user for the event
    bool unregisterAttendee(user& a);                             // Remove a user from the event
    void showAttendees() const;                                   // Display all registered attendees
    void show_waiting_list() const;                               // Display waiting list
    void setdate(const std::string datetime);                     // Set date from string
    void get_all_event_feedback();                                // Load all feedback for event

    // ==================== GETTERS ====================
    std::string getname() const { return name; }
    int get_event_id() const { return event_id; }
    std::string getdescription() const { return description; }
    std::string getplatform() const { return platform; }
    std::tm getdatetime() const { return datetime; }
    int getorganizer() const { return org_id; }
    int get_org_id() const { return org_id; }
    int getcapacity() const { return capacity; }
    unsigned int getattendeescount() const { return attendees.size(); }
    double get_overall_feedback() { return overall_rating; }

    // Notify attendees if the event is postponed
    void notify_attendees_postponing(string date) {
        for(auto a : attendees)
            std::cout << "Notification for " << a.getName()
                      << " - Event: " << name
                      << " - evnent postponed to  " << date << std::endl;
    }

    // Notify attendees if the event is canceled
    void notify_attendees_cancelation() {
        for(auto a : attendees)
            std::cout << "Notification for " << a.getName()
                      << " - Event: " << name
                      << " - event canceled  " << std::endl;
    }

    // ==================== TIME & DATE ====================
    std::string timetostring(tm date);                 // Convert tm to string
    std::tm converttime(std::string);                  // Convert string to tm

    // ==================== DATABASE LOAD ====================
    void load_attendees_fromDatabase();                // Load attendees from DB
    void load_waiting_list_fromDatabase();             // Load waiting list from DB
    void load_feedbacks_fromDatabase();                // Load feedback from DB
    void add_feedback(feedback f);                     // Add single feedback to list
    double calculateOverallRating();                   // Calculate average rating
};

#endif // VIRTUALEVENT_H