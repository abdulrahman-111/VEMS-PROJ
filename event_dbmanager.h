#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include "sqlite3.h"
#include <vector>
#include <string>
#include <iostream>

#include "users.dbmanger.h"

// ==============================================
// Forward Declarations
// ==============================================

class user;  // Forward declaration for user class

// ==============================================
// Database Connection
// ==============================================

extern sqlite3* db;           // Database connection handle
extern bool opendb();         // Opens database connection, returns true if successful

// ==============================================
// Data Structures
// ==============================================

// Struct containing all event information
struct event_db {
    int event_id;                  // Unique event identifier
    int organizer_id;              // Event organizer ID
    int capacity;                  // Max attendees
    int type;                      // 1: Webinar, 2: Workshop, 3: Conference
    int current_attendees;         // Current attendees
    int price;                     // Event price

    std::string name;              // Event name
    std::string description;       // Event description
    std::string datetime;          // Event date and time
    std::string platform;          // Platform or venue

    // Webinar-specific fields
    bool questions_and_answers_enabled;
    std::string webinar_topic;

    // Workshop-specific fields
    std::string breakout_rooms;
    std::string materials_required;

    // Conference-specific fields
    std::string session_info;
    std::string tutor;
    std::string sessions_track;
    event_db():current_attendees(0),price(0){}
};

// Struct for dynamic event filtering
struct event_criteria {
    int event_id;                  // Search by specific ID
    int max_capacity;              // Max capacity filter
    int min_capacity;              // Min capacity filter
    std::string name;              // Name filter
    int type;              // Type filter
    int organizer;         // Organizer filter
    std::string datetime;          // Exact datetime
    std::string start_date;        // From date
    std::string end_date;          // To date
    std::string platform;          // Platform filter

    // Default constructor
    event_criteria()
        : event_id(0), max_capacity(0), min_capacity(0),
          name(""), type(0), organizer(0), datetime(""),
          start_date(""), end_date(""), platform("") {}
};

// ==============================================
// Core Event Operations
// ==============================================

extern bool add_event(event_db& event);
extern bool check_event_available(std::string name, std::string start_date, int organizer);
extern int return_id(event_db event);
extern std::string return_name_for_id(int event_id);
extern bool increment_capacity(event_db event);

// ==============================================
// Event Search & Retrieval
// ==============================================

extern std::vector<std::string> getEventTypes();
extern std::vector<std::string> getPlatforms();

extern std::vector<event_db> get_all_events();
extern std::vector<event_db> get_all_registered_events_(int user_id);
extern std::vector<event_db> get_all_created_events_(int user_id);

extern std::vector<event_db> dynamic_search(event_criteria &criteria);
extern event_criteria get_filter();
extern void dynamic_helper(event_criteria& criteria, std::string& where, std::vector<std::string>& parameters);

// ==============================================
// Event Modification
// ==============================================

extern bool cancel_event_by_name(int event);
extern bool update_event_date(int event_id, std::string datetime);

#endif // DATABASE_MANAGER_H
