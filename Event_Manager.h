#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

// ==================== INCLUDES ====================

#include "event_dbmanager.h"
class user;
class virtualevent;
#include "factory.h"
#include "conference.h"
#include "webinar.h"
#include "workshops.h"

#include <map>
#include <sstream>
#include <limits>
#include <memory>

using namespace std;

// ==================== EVENT MANAGER CLASS ====================
class EventManager {
private:
    // ==================== PRIVATE MEMBERS ====================
    vector<virtualevent*> events;   // All events in system
    user user_run;//this is the error //at the set get all info fromm vector not db
    static EventManager* ptr;

    // ==================== PRIVATE METHODS ====================
    EventManager() {}
    EventManager(const EventManager&) = delete;
    EventManager operator=(const EventManager&) = delete;

public:
    // ==================== SINGLETON ACCESS ====================
    static EventManager* getInstance() {
        if (ptr == nullptr)
            ptr = new EventManager();
        return ptr;
    }

    ~EventManager() {
        for (int i = 0; i < events.size(); i++) {
            delete events[i];
        }
    }

    // ==================== USER MANAGEMENT ====================
    void set_user(user& u) {
        user_run=u;
    }

    // ==================== EVENT DISPLAY METHODS ====================
    void showAllEvents() const;
    void disp_menu();

    // ==================== EVENT SEARCH METHODS ====================

    virtualevent* findEventByName(const string& name,int org_id) const;//all events
    virtualevent* findEventByName(const string& name, const user& user) const;// created events
    virtualevent* findEventByName(const string& name, const user& user,int i) const;//registered events
    void search_events();
    void searchByDate(const string& date) const;
    void dynamic_search_bar();

    // ==================== EVENT MODIFICATION METHODS ====================
    void addEvent(virtualevent* event);
    void reschedule(const string& name, const string& newDate);
    void cancelEvent(const string& name, const user& user);

    // ==================== EVENT ATTENDEE METHODS ====================
    void loadAttendeesForEvent(const string& eventName, const user& user);

    // ==================== MENU OPTION METHODS ====================
    void create_choice();
    void manage_choice();
    void show_choice_3();
    void show_choice_4();
    void enter_feedback(virtualevent* event);

    // ==================== EVENT CREATION ====================
    eventclient* create_event();
    event_criteria get_filter();
    // ==================== DATABASE METHODS ====================
    void load_db();
};

#endif // EVENT_MANAGER_H
