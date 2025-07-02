#ifndef USERS_DBMANGER_H
#define USERS_DBMANGER_H
#include "event_dbmanager.h"
#include "feedback.h"
#include "user.h"
struct event_db;
// User structure containing user information
struct user_db {
    int user_id;                     // Unique user if unique
    std::string email;               // email//unique
    std::string password;            // pass // not unique
    std::string username;            //username//unique
    std::string Name;                //user attendee of organizer
    std::string affiliation;         //
};


// ==============================================
// User Account Functions
// ==============================================

// Add new user to database after validation
// Returns true if successful, false if failed
extern bool add_user(user &user);

// Get user_id by username
// Returns user_id if found, -1 if not found
extern int return_user_id (const std::string& email);

// Delete user account
// Returns true if successful, false if failed
extern bool delete_user(user &user);

// Check if user exists with given credentials
// Returns true if authentication successful, false if failed
extern bool authenticate_user(user &user);

// Get complete user information by email
// Returns user  with data if found, empty struct if not found
extern user return_user_by_email(const std::string &email);
extern user return_user_by_id(int user_id);

// ==============================================
// Attendee Operations
// ==============================================

// Register user for an event
// Uses capacity increment function internally
// Returns true if successful, false if failed
extern bool registerAttendee_db(user &user, event_db &event);

// Unregister user from an event
// Returns true if successful, false if failed
extern bool unregisterAttendee_db(int user_id, int event_id);

// ==============================================
// Feedback Operations
// ==============================================

// Submit feedback for an event
// Returns true if successful, false if failed
extern bool submitFeedback( feedback& feedback);
// Get all feedback for specific event
// Returns vector of feedback structs
extern std::vector<feedback> getEventFeedback(event_db &event);
extern bool delete_event_feedback(int event_id);
// ==============================================
// Utility Functions
// ==============================================

// Get all users in database
// Returns vector of all user structs
extern std::vector<user> get_all_users();

// Get all attendees for specific event
// Returns vector of user structs attending the event
extern std::vector<user> get_all_event_attendees(event_db &event);

// to waiting list
extern bool add_user_to_waiting_list(const user&user,const event_db& event,int position);
extern bool delete_user_from_waiting_list(int user_id,int event_id);
extern bool update_user_position(user &user,event_db &event,int new_pos);
extern std::vector<user> return_event_waiting_list(event_db &event);
#endif //USERS_DBMANGER_H


