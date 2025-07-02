#include "virtualevent.h"
#include "workshops.h"
#include "webinar.h"
#include "conference.h"
#include <sstream>
#include <iomanip>
#include "user.h"
#include "feedback.h"

// Add feedback to the event and update average rating
void virtualevent:: add_feedback(feedback f) {
    event_feedbacks.push_back(f);
    overall_rating = calculateOverallRating();
}

// Constructor to initialize all main event properties
virtualevent::virtualevent(const std::string& n, unsigned int id,
                           unsigned int orgid,
                           const std::string& desc,
                           const std::tm& date,
                           const std::string& plat,
                           unsigned int cap,
                           const std::string& org) {
    name = n;
    event_id = id;
    description = desc;
    org_id = orgid;
    platform = plat;
    organizer = org;
    datetime = date;
    capacity = cap;
    overall_rating = 0;
}

// Show full event details in formatted text
std::string virtualevent::geteventdetails() const {
    std::ostringstream oss;

    std::cout << "=============" << std::endl;
    std::cout << " Event info: " << std::endl;
    std::cout << "=============" << std::endl;

    oss << "Event Name: " << name << "\n"
        << "Session ID: " << event_id << "\n"
        << "Description: " << description << "\n"
        << "Date & Time: " << std::put_time(&datetime, "%Y-%m-%d %H:%M:%S") << "\n"
        << "Platform: " << platform << "\n"
        << "Capacity: " << capacity << "\n"
        << "Organizer: " << organizer << "\n"
        << "Organizer ID: " << org_id << "\n"
        << "Attendees Count: " << attendees.size();

    return oss.str();
}

// Create (build) an event based on its type (factory pattern)
virtualevent* virtualevent::build(eventtype eventtype, const std::string& n,
                                  unsigned int id, unsigned orgid,
                                  const std::string& desc, const std::tm& date,
                                  const std::string& plat, unsigned int cap,
                                  const std::string& org) {
    if (eventtype == Workshop)
        return new workshop(n, id, orgid, desc, date, plat, cap, org);

    if (eventtype == Webinar)
        return new webinar(n, id, orgid, desc, date, plat, cap, org);

    if (eventtype == Conference)
        return new conference(n, id, orgid, desc, date, plat, cap, org);

    return nullptr;
}

// Simple reminder message for the event
void virtualevent::sendReminder() const {
    std::cout << "Don't forget you have an important event.\n";
}

// Register a user for the event
bool virtualevent::registerAttendee(user& a) {
    event_db temp = this->convertetostruct();

    // Prevent the host from registering
    if (a.getUserID() == org_id) {
        cout << "Host cannot register as an attendee for their own event.\n";
        return false;
    }

    // Check if already registered
    for (const auto& attendee : attendees) {
        if (attendee == a) {
            cout << "Attendee already registered.\n";
            return false;
        }
    }

    // Check if already in waiting list
    for (const auto& wait : waitingList) {
        if (wait == a) {
            cout << "Attendee already in the waiting list.\n";
            return false;
        }
    }

    // Add to attendees list if space available
    if (attendees.size() < capacity) {
        attendees.push_back(a);
        registerAttendee_db(a, temp);
        cout << "Attendee registered successfully.\n";
    }
    // Add to waiting list if attendees are full
    else if (waitingList.size() <= capacity) {
        waitingList.push_back(a);
        add_user_to_waiting_list(a, temp, attendees.size());
        cout << "Event is full. Attendee added to waiting list.\n";
        return false;
    }
    // If both lists are full
    else {
        cout << "Event and waiting list are full. Registration failed.\n";
        return false;
    }
    return true;
}

// Remove a user from attendees, and promote one from the waiting list if available
bool virtualevent::unregisterAttendee(user & a) {
    // Find and remove from attendee list
    for (auto it = attendees.begin(); it != attendees.end(); ++it) {
        if ((*it).getUserID() == a.getUserID()) {
            attendees.erase(it);
            break;
        }
    }
    // Promote first waiting user if any
    if (!waitingList.empty()) {
        user & promotedUser = waitingList.front();
        waitingList.erase(waitingList.begin());
        attendees.push_back(promotedUser);
        event_db temp = this->convertetostruct();
        registerAttendee_db(promotedUser, temp);
        promotedUser.set_registerd_event(this);
        delete_user_from_waiting_list(promotedUser.getUserID(), this->get_event_id());
        return true;
    }
    return false;
}

// Show list of attendees
void virtualevent::showAttendees() const {
    if(!attendees.empty()) {
        cout << "\nAttendees: " << endl;
        for (const auto& a : attendees)
            a.display_user_info();
    }
    else cout << "NO Attendees\n";
}

// Show list of users in the waiting list
void virtualevent::show_waiting_list() const {
    cout << "\nWaiting list for event \"" << name << "\":" << endl;
    if (!waitingList.empty()) {
        for (const auto& w : waitingList)
            w.display_user_info();
    }
    else cout << "no waiting list\n";
}

// Show all feedbacks submitted for the event
void virtualevent::get_all_event_feedback() {
    if (!event_feedbacks.empty()) {
        int i = 0;
        cout << "Event: Feedbacks:\n";
        for (auto f : event_feedbacks) {
            user u = return_user_by_id(f.get_user_id());
            cout << "Attendee[" << i++ << "]: " << u.getName() << " submiision data: " << f.get_submission_date() << endl;
            cout << "Rating: " << f.getrating() << " comments:" << f.getcomments() << endl;
        }
    } else {
        cout << "no feedbacks available\n";
    }
}

// ================= DATABASE FUNCTIONS =================
// Calculate the average rating
double virtualevent::calculateOverallRating() {
    if (event_feedbacks.empty()) return 0.0;

    double sum = 0;
    for (auto r : event_feedbacks) {
        sum += r.getrating();
    }
    return sum / event_feedbacks.size();
}

// Load feedbacks from database
void virtualevent::load_feedbacks_fromDatabase() {
    event_db tempo = this->convertetostruct();
    event_feedbacks = getEventFeedback(tempo);
    overall_rating = calculateOverallRating();
}

// Load all attendees from database
void virtualevent::load_attendees_fromDatabase() {
    event_db temp = this->convertetostruct();
    attendees = get_all_event_attendees(temp);
}

// Load waiting list from database
void virtualevent::load_waiting_list_fromDatabase() {
    event_db temp = this->convertetostruct();
    waitingList = return_event_waiting_list(temp);
}

// ================= DATE & TIME =================
// Set event date/time from string
void virtualevent::setdate(const std::string datetime) {
    tm newdate = converttime(datetime);
    this->datetime = newdate;
}

// Convert tm to string format
std::string virtualevent::timetostring(tm date) {
    std::ostringstream timestring;
    timestring << date.tm_year + 1900 << "-"
               << date.tm_mon + 1 << "-"
               << date.tm_mday << " "
               << date.tm_hour << ":"
               << date.tm_min << ":"
               << date.tm_sec;
    return timestring.str();
}

// Convert date/time string to tm structure
std::tm virtualevent::converttime(const std::string datetime) {
    tm timeinfo = {0};
    std::istringstream iss(datetime);
    char discard;

    if (!(iss >> timeinfo.tm_year >> discard >> timeinfo.tm_mon >> discard >> timeinfo.tm_mday
              >> timeinfo.tm_hour >> discard >> timeinfo.tm_min >> discard >> timeinfo.tm_sec)) {
        std::cerr << "Invalid date/time format!" << std::endl;
        return tm();
    }

    timeinfo.tm_year -= 1900;
    timeinfo.tm_mon -= 1;

    time_t temp = mktime(&timeinfo);
    if (temp == -1) {
        std::cerr << "Time conversion failed!" << std::endl;
        return tm();
    }

    tm* result = localtime(&temp);
    if (!result) {
        std::cerr << "localtime() failed!" << std::endl;
        return tm();
    }

    return *result;
}
