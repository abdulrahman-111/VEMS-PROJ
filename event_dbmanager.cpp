#include "event_dbmanager.h"
#include "users.dbmanger.h"
#include <iostream>
#include <vector>
#include <string>

// Global database connection handle
sqlite3* db = nullptr; // Database connection handle (null when closed)

// ==============================================
// Database Connection Management
// ==============================================

// Opens a connection to the SQLite database
// Returns true if successful, false if failed
bool opendb() {
    if (db) return true; // Already open
    int check = sqlite3_open("VMS1.sqlite", &db);
    if (check != SQLITE_OK) {
        std::cout << "Database could not be opened: " << sqlite3_errmsg(db);
        sqlite3_close(db);
        db = nullptr;
        return false;
    }
    return true;
}

// ==============================================
// Core Event Operations
// ==============================================

// Adds a new event to the database
// Checks for event availability first
// Handles different event types (webinar, workshop, conference)
bool add_event(event_db &event) {
    // Check if event name and time are available for this organizer
    if(!check_event_available(event.name, event.datetime, event.organizer_id)) {
        std::cout << "Cannot add event: An event with the same name and datetime already exists." << std::endl;
        return false;
    }

    if(!opendb()) {
        std::cout << "Database error: Failed to open database connection." << std::endl;
        return false;
    }

    // Prepare SQL based on event type (1=Webinar, 2=Workshop, 3=Conference)
    std::string sql;
    if(event.type == 1) { // Webinar
        sql = "INSERT INTO events (name, description, datetime, platform, capacity, type_id, organizer_id, current_attendees, price, questions_and_answers_enabled, webinar_topic) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
    }
    else if (event.type == 2) { // Workshop
        sql = "INSERT INTO events (name, description, datetime, platform, capacity, type_id, organizer_id, current_attendees, price, breakout_rooms, materials_required) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
    }
    else { // Conference
        sql = "INSERT INTO events (name, description, datetime, platform, capacity, type_id, organizer_id, current_attendees, price, session_info, tutor, sessions_track) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
    }

    // Prepare SQL statement
    sqlite3_stmt *stmt;
    int check = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    if(check != SQLITE_OK) {
        std::cout << "Database error: Failed to prepare statement - " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        db = nullptr;
        return false;
    }

    // Bind common parameters for all event types
    sqlite3_bind_text(stmt, 1, event.name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, event.description.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, event.datetime.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, event.platform.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 5, event.capacity);
    sqlite3_bind_int(stmt, 6, event.type);
    sqlite3_bind_int(stmt, 7, event.organizer_id);
    sqlite3_bind_int(stmt, 8, event.current_attendees);
    sqlite3_bind_int(stmt, 9, event.price);

    // Bind type-specific parameters
    if(event.type == 1) {
        sqlite3_bind_int(stmt, 10, event.questions_and_answers_enabled);
        sqlite3_bind_text(stmt, 11, event.webinar_topic.c_str(), -1, SQLITE_STATIC);
    }
    else if (event.type == 2) {
        sqlite3_bind_text(stmt, 10, event.breakout_rooms.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 11, event.materials_required.c_str(), -1, SQLITE_STATIC);
    }
    else {
        sqlite3_bind_text(stmt, 10, event.session_info.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 11, event.tutor.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 12, event.sessions_track.c_str(), -1, SQLITE_STATIC);
    }

    // Execute statement
    check = sqlite3_step(stmt);
    if(check != SQLITE_DONE) {
        std::cout << "Database error: Failed to execute statement - " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        db = nullptr;
        return false;
    }

    // Get the auto-generated event ID
    event.event_id=sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    db = nullptr;
    return true;
}

// Returns the ID of an event based on name, datetime and organizer
int return_id(event_db event) {
    if(!opendb()) {
        std::cout << "Database error: Failed to open database connection." << std::endl;
        return 0;
    }

    std::string sql = "SELECT * FROM events WHERE name = ? AND datetime = ? AND organizer_id = ?;";
    sqlite3_stmt *stmt;
    int check = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    if(check != SQLITE_OK) {
        std::cout << "Failed to prepare statement:" << sqlite3_errmsg(db);
        return 0;
    }

    // Bind search parameters
    sqlite3_bind_text(stmt, 1, event.name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, event.datetime.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, event.organizer_id);

    // Execute query
    check = sqlite3_step(stmt);
    if(check == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        return id;
    }

    sqlite3_finalize(stmt);
    return 0;
}

// Checks if an event slot is available for the given organizer
bool check_event_available(std::string name, std::string datetime, int organizer_id) {
    if(!opendb()) {
        std::cout << "Database error: Failed to open database connection." << std::endl;
        return false;
    }

    std::string sql = "SELECT * FROM events WHERE name = ? AND datetime = ? AND organizer_id = ?;";
    sqlite3_stmt *stmt;
    int check = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    if(check != SQLITE_OK) {
        std::cout << "Database error: Failed to prepare statement - " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        db = nullptr;
        return false;
    }

    // Bind search parameters
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, datetime.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, organizer_id);

    check = sqlite3_step(stmt);

    if(check == SQLITE_ROW) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        db = nullptr;
        return false; // Event exists
    }

    if(check != SQLITE_DONE) {
        std::cout << "Database error: Failed to execute query - " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        db = nullptr;
        return false;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    db = nullptr;
    return true; // The time is available for this user
}

// ==============================================
// Event Search Functions
// ==============================================

// Helper function to build WHERE clause for dynamic searches
void dyanmic_helper(event_criteria &criteria, std::string &where, std::vector<std::string> &parameters) {
    std::vector<std::string> conditions;

    // Build conditions based on provided criteria
    if(criteria.event_id) {
        conditions.push_back(" event_id = ?");
        parameters.push_back(std::to_string(criteria.event_id));
    }
    if(!criteria.name.empty()) {
        conditions.push_back(" name LIKE ?");
        parameters.push_back("%" + criteria.name + "%");
    }
    if(criteria.type) {
        conditions.push_back(" type_id = ?");
        parameters.push_back(to_string(criteria.type));
    }
    if(!criteria.platform.empty()) {
        conditions.push_back(" platform = ?");
        parameters.push_back(criteria.platform);
    }
    if(criteria.organizer) {
        conditions.push_back(" organizer_id = ?");
        parameters.push_back(to_string(criteria.organizer));
    }
    if(criteria.min_capacity > 0) {
        conditions.push_back(" capacity >= ?");
        parameters.push_back(std::to_string(criteria.min_capacity));
    }
    if(criteria.max_capacity > 0) {
        conditions.push_back(" capacity <= ?");
        parameters.push_back(std::to_string(criteria.max_capacity));
    }
    if(!criteria.datetime.empty()) {
        conditions.push_back(" datetime LIKE ? ");
        parameters.push_back("%"+criteria.datetime+"%");
    }
    else if(!criteria.start_date.empty() && !criteria.end_date.empty()) {
        conditions.push_back(" datetime BETWEEN ? AND ? ");
        parameters.push_back(criteria.start_date);
        parameters.push_back(criteria.end_date);
    }
    else if(!criteria.start_date.empty()) {
        conditions.push_back(" datetime >= ? ");
        parameters.push_back(criteria.start_date);
    }
    else if(!criteria.end_date.empty()) {
        conditions.push_back(" datetime <= ? ");
        parameters.push_back(criteria.end_date);
    }

    // Combine conditions with AND
    if(!conditions.empty()) {
        where = conditions[0];
        for(size_t i = 1; i < conditions.size(); i++) {
            where += " AND " + conditions[i];
        }
    }
}

// Performs a dynamic search based on multiple criteria
std::vector<event_db> dynamic_search(event_criteria &criteria) {
    std::vector<event_db> events;
    if(!opendb()) {
        return events;
    }

    // Build the WHERE clause dynamically
    std::string sql = "SELECT * FROM events ";
    std::string where_clause;
    std::vector<std::string> parameters;

    dyanmic_helper(criteria, where_clause, parameters);

    if(!parameters.empty()) {
        sql += " WHERE " + where_clause;
    }
    sql += " ORDER BY datetime ASC;";

    // Prepare SQL statement
    sqlite3_stmt *stmt;
    if(sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cout << "Failed to prepare statement:" << sqlite3_errmsg(db);
        sqlite3_close(db);
        db = nullptr;
        return events;
    }

    // Bind parameters
    for(size_t i = 0; i < parameters.size(); i++) {
        sqlite3_bind_text(stmt, i+1, parameters[i].c_str(), -1, SQLITE_STATIC);
    }

    // Process results
    int step_result;
    while((step_result = sqlite3_step(stmt)) == SQLITE_ROW) {
        event_db temp;

        // Extract all event fields from the result row
        temp.event_id = sqlite3_column_int(stmt, 0);
        temp.organizer_id = sqlite3_column_int(stmt, 1);
        temp.capacity = sqlite3_column_int(stmt, 6);
        temp.type = sqlite3_column_int(stmt, 7);
        temp.current_attendees = sqlite3_column_int(stmt, 8);
        temp.price = sqlite3_column_int(stmt, 9);

        temp.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        temp.description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        temp.datetime = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        temp.platform = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));

        temp.questions_and_answers_enabled = sqlite3_column_int(stmt, 10);
        const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 11));
        temp.webinar_topic = text ? text : "";

        text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 12));
        temp.breakout_rooms = text ? text : "";

        text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 13));
        temp.materials_required = text ? text : "";

        text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 14));
        temp.session_info = text ? text : "";
        text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 15));
        temp.tutor = text ? text : "";
        text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 16));
        temp.sessions_track = text ? text : "";

        events.push_back(temp);
    }

    if(step_result != SQLITE_DONE) {
        std::cout << "Database error: Failed to execute query - " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    db = nullptr;
    return events;
}

// Returns all events in the database
std::vector<event_db> get_all_events() {
    std::vector<event_db> events;

    if(!opendb()) {
        std::cout << "operation failed" << std::endl;
        return events;
    }

    string sql = "SELECT * FROM events;";
    sqlite3_stmt *stmt;
    int check = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    if(check != SQLITE_OK) {
        std::cout << "Failed to prepare statement:" << sqlite3_errmsg(db);
        sqlite3_close(db);
        db = nullptr;
        return events;
    }

    // Process all rows
    int step_result;
    while((step_result = sqlite3_step(stmt)) == SQLITE_ROW) {
        event_db temp;

        // Extract all event fields (same as dynamic_search)
        temp.event_id = sqlite3_column_int(stmt, 0);
        temp.organizer_id = sqlite3_column_int(stmt, 1);
        temp.capacity = sqlite3_column_int(stmt, 6);
        temp.type = sqlite3_column_int(stmt, 7);
        temp.current_attendees = sqlite3_column_int(stmt, 8);
        temp.price = sqlite3_column_int(stmt, 9);

        temp.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        temp.description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        temp.datetime = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        temp.platform = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));

        temp.questions_and_answers_enabled = sqlite3_column_int(stmt, 10);
        const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 11));
        temp.webinar_topic = text ? text : "";

        text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 12));
        temp.breakout_rooms = text ? text : "";

        text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 13));
        temp.materials_required = text ? text : "";

        text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 14));
        temp.session_info = text ? text : "";
        text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 15));
        temp.tutor = text ? text : "";
        text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 16));
        temp.sessions_track = text ? text : "";

        events.push_back(temp);
    }

    if(step_result != SQLITE_DONE) {
        std::cout << "Database error: Failed to execute query - " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    db = nullptr;
    return events;
}

// Returns all events a user has registered for
std::vector<event_db> get_all_registered_events_(int user_id) {
    std::vector<event_db> events;

    if(!opendb()) {
        std::cout << "operation failed" << std::endl;
        return events;
    }

    string sql = "SELECT * FROM events e JOIN event_attendee ea ON e.event_id=ea.event_id WHERE ea.user_id=?;";
    sqlite3_stmt *stmt;
    int check = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    if(check != SQLITE_OK) {
        std::cout << "Failed to prepare statement:" << sqlite3_errmsg(db);
        sqlite3_close(db);
        db = nullptr;
        return events;
    }

    // Bind user ID parameter
    sqlite3_bind_int(stmt, 1, user_id);

    // Process results (same field extraction as get_all_events)
    int step_result;
    while((step_result = sqlite3_step(stmt)) == SQLITE_ROW) {
        event_db temp;

        temp.event_id = sqlite3_column_int(stmt, 0);
        temp.organizer_id = sqlite3_column_int(stmt, 1);
        temp.capacity = sqlite3_column_int(stmt, 6);
        temp.type = sqlite3_column_int(stmt, 7);
        temp.current_attendees = sqlite3_column_int(stmt, 8);
        temp.price = sqlite3_column_int(stmt, 9);

        temp.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        temp.description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        temp.datetime = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        temp.platform = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));

        temp.questions_and_answers_enabled = sqlite3_column_int(stmt, 10);
        const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 11));
        temp.webinar_topic = text ? text : "";

        text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 12));
        temp.breakout_rooms = text ? text : "";

        text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 13));
        temp.materials_required = text ? text : "";

        text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 14));
        temp.session_info = text ? text : "";
        text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 15));
        temp.tutor = text ? text : "";
        text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 16));
        temp.sessions_track = text ? text : "";

        events.push_back(temp);
    }

    if(step_result != SQLITE_DONE) {
        std::cout << "Database error: Failed to execute query - " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    db = nullptr;
    return events;
}

// Returns all events created by a specific organizer
std::vector<event_db> get_all_created_events_(int user_id) {
    std::vector<event_db> events;

    if(!opendb()) {
        std::cout << "operation failed" << std::endl;
        return events;
    }

    string sql = "SELECT * FROM events WHERE organizer_id=?;";
    sqlite3_stmt *stmt;
    int check = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    if(check != SQLITE_OK) {
        std::cout << "Failed to prepare statement:" << sqlite3_errmsg(db);
        sqlite3_close(db);
        db = nullptr;
        return events;
    }

    // Bind organizer ID parameter
    sqlite3_bind_int(stmt, 1, user_id);

    // Process results (same field extraction as get_all_events)
    int step_result;
    while((step_result = sqlite3_step(stmt)) == SQLITE_ROW) {
        event_db temp;

        temp.event_id = sqlite3_column_int(stmt, 0);
        temp.organizer_id = sqlite3_column_int(stmt, 1);
        temp.capacity = sqlite3_column_int(stmt, 6);
        temp.type = sqlite3_column_int(stmt, 7);
        temp.current_attendees = sqlite3_column_int(stmt, 8);
        temp.price = sqlite3_column_int(stmt, 9);

        temp.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        temp.description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        temp.datetime = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        temp.platform = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));

        temp.questions_and_answers_enabled = sqlite3_column_int(stmt, 10);
        const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 11));
        temp.webinar_topic = text ? text : "";

        text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 12));
        temp.breakout_rooms = text ? text : "";

        text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 13));
        temp.materials_required = text ? text : "";

        text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 14));
        temp.session_info = text ? text : "";
        text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 15));
        temp.tutor = text ? text : "";
        text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 16));
        temp.sessions_track = text ? text : "";

        events.push_back(temp);
    }

    if(step_result != SQLITE_DONE) {
        std::cout << "Database error: Failed to execute query - " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    db = nullptr;
    return events;
}

// Returns list of all event types in the system
std::vector<std::string> getEventTypes() {
    std::vector<std::string> types;
    int type_id;

    if(!opendb()) {
        std::cout << "operation failed" << std::endl;
        return types;
    }

    string sql = "SELECT DISTINCT type_id FROM events;";
    sqlite3_stmt *stmt;
    int check = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    if(check != SQLITE_OK) {
        std::cout << "Failed to prepare statement:" << sqlite3_errmsg(db);
        sqlite3_close(db);
        db = nullptr;
        return types;
    }

    // Convert numeric type IDs to descriptive strings
    while(sqlite3_step(stmt) == SQLITE_ROW) {
        type_id = sqlite3_column_int(stmt, 0);
        if(type_id == 1) {
            types.push_back("webinar");
        }
        else if(type_id == 2) {
            types.push_back("workshop");
        }
        else {
            types.push_back("conference");
        }
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    db = nullptr;
    return types;
}

// Returns list of all platforms used by events
std::vector<std::string> getPlatforms() {
    std::vector<std::string> platforms;
    std::string platform;

    if(!opendb()) {
        std::cout << "operation failed" << std::endl;
        return platforms;
    }

    string sql = "SELECT DISTINCT platform FROM events;";
    sqlite3_stmt *stmt;
    int check = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    if(check != SQLITE_OK) {
        std::cout << "Failed to prepare statement:" << sqlite3_errmsg(db);
        sqlite3_close(db);
        db = nullptr;
        return platforms;
    }

    // Collect all unique platforms
    while(sqlite3_step(stmt) == SQLITE_ROW) {
        platform = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        platforms.push_back(platform);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    db = nullptr;
    return platforms;
}

// ==============================================
// Event Modification Functions
// ==============================================

// Deletes all attendees for a specific event
bool delete_attendees_by_name(int event_id) {
    if(!opendb()) {
        std::cout << "Error: Failed to open database connection" << std::endl;
        return false;
    }

    string sql = "DELETE FROM event_attendee WHERE event_id = ? ;";
    sqlite3_stmt *stmt;
    int check = sqlite3_prepare(db, sql.c_str(), -1, &stmt, nullptr);

    if(check != SQLITE_OK) {
        std::cout << "Failed to prepare statement:" << sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        db = nullptr;
        return false;
    }

    // Bind event ID parameter
    sqlite3_bind_int(stmt, 1, event_id);

    if(sqlite3_step(stmt) == SQLITE_DONE) {
        std::cout << "Success: Event attendees'" << event_id << "' deleted successfully" << std::endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        db = nullptr;
        return true;
    }

    std::cout << "Error: Failed to cancel event - " << sqlite3_errmsg(db) << std::endl;
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    db = nullptr;
    return false;
}

// Cancels an event by first deleting attendees then the event itself
bool cancel_event_by_name(int event_id) {
    // First delete all attendees
    delete_attendees_by_name(event_id);

    if(!opendb()) {
        std::cout << "Error: Failed to open database connection" << std::endl;
        return false;
    }

    string sql = "DELETE FROM events WHERE event_id=?;";
    sqlite3_stmt *stmt;
    int check = sqlite3_prepare(db, sql.c_str(), -1, &stmt, nullptr);

    if(check != SQLITE_OK) {
        std::cout << "Failed to prepare statement:" << sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        db = nullptr;
        return false;
    }

    // Bind event ID parameter
    sqlite3_bind_int(stmt, 1, event_id);

    if(sqlite3_step(stmt) == SQLITE_DONE) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        db = nullptr;
        return true;
    }

    std::cout << "Error: Failed to cancel event - " << sqlite3_errmsg(db) << std::endl;
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    db = nullptr;
    return false;
}

// Updates the date/time for an existing event
bool update_event_date(int event_id, std::string date) {
    if(!opendb()) {
        std::cout << "Execution failed \n";
        return false;
    }

    string sql = "UPDATE events SET datetime = ? WHERE event_id = ?;";
    sqlite3_stmt *stmt;
    int check = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    if(check != SQLITE_OK) {
        std::cout << "Failed to prepare statement:" << sqlite3_errmsg(db);
        sqlite3_close(db);
        db = nullptr;
        return false;
    }

    // Bind parameters
    sqlite3_bind_text(stmt, 1, date.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, event_id);

    check = sqlite3_step(stmt);
    if(check == SQLITE_DONE) {
        std::cout << "Event date updated to " << date << std::endl;
    }
    else {
        std::cout << "Failed to update event date: " << sqlite3_errmsg(db) << "\n";
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    db = nullptr;
    return true;
}

// Increases the capacity of an event by 1
bool increment_capacity(event_db event) {
    if(!opendb()) {
        std::cout << "Database error: Failed to open database connection." << std::endl;
        db = nullptr;
        return false;
    }

    sqlite3_stmt *stmt;
    std::string sql = "UPDATE events SET capacity = ? WHERE event_id = ?";

    if(sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cout << "Failed to prepare statement:" << sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        db = nullptr;
        return false;
    }

    // Bind parameters
    sqlite3_bind_int(stmt, 1, event.capacity + 1);
    sqlite3_bind_int(stmt, 2, event.event_id);

    if(sqlite3_step(stmt) == SQLITE_DONE) {
        std::cout << "Capacity incremented successfully";
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        db = nullptr;
        return true;
    }

    std::cout << "Database error: Failed to execute query - " << sqlite3_errmsg(db) << std::endl;
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    db = nullptr;
    return false;
}

// Returns the name of an event given its ID
std::string return_name_for_id(int event_id) {
    std::string name = "";

    if(!opendb()) {
        std::cout << "Error: Failed to open database connection" << std::endl;
        sqlite3_close(db);
        db = nullptr;
        return name;
    }

    std::string sql = "SELECT name FROM events WHERE event_id = ?;";
    sqlite3_stmt *stmt;
    int check = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    if(check != SQLITE_OK) {
        std::cout << "Error: Failed to prepare statement - " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        db = nullptr;
        return name;
    }

    // Bind event ID parameter
    sqlite3_bind_int(stmt, 1, event_id);
    check = sqlite3_step(stmt);

    if(check == SQLITE_ROW) {
        name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    }
    else if(check == SQLITE_DONE) {
        std::cout << "Info: No event found with ID: " << event_id << std::endl;
    }
    else {
        std::cout << "Error: Database query failed - " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    db = nullptr;
    return name;
}