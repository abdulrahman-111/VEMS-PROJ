#include"webinar.h"
#include<sstream>
#include<limits>

// ==================== CONSTRUCTOR ====================
// Initializes a webinar object with all base attributes via virtualevent
webinar::webinar(const std::string& name,
                 unsigned int id,
                 unsigned int orgid,
                 const std::string& desc,
                 const std::tm& date,
                 const std::string& plat,
                 unsigned int cap,
                 const std::string& org)
    : virtualevent(name, id, orgid, desc, date, plat, cap, org) {}

// ==================== GETTER METHOD FOR WEBINAR-SPECIFIC INPUT ====================
// Gathers additional details specific to a webinar: topic and Q/A option
void webinar::getter() {
    std::cout << "what is the topic in your mind to this webinar ";
    getline(std::cin, webtopic); // Input webinar topic

    std::cout << "will you allow the Q/A (1 for Yes, 0 for No):";
    std::cin >> qnaenabled; // Input Q/A flag (true or false)
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Flush leftover newline
}

// ==================== EVENT DETAILS DISPLAY ====================
// Returns full webinar details as a formatted string
std::string webinar::geteventdetails() const {
    std::ostringstream oss;
    oss << virtualevent::geteventdetails() << "\n"           // Include base details
        << "Type: webinar\n"
        << "webinar topic: " << webtopic << "\n"
        << "QNA is " << (qnaenabled ? "Enabled" : "Not enabled") << "\n";
    return oss.str();
}

// ==================== REMINDER FUNCTION ====================
// Sends a reminder to attendees and shows webinar-specific info
void webinar::sendReminder() const {
    virtualevent::sendReminder();               // Call base reminder
    std::cout << webinar::geteventdetails();    // Show full details
}

// ==================== DATABASE -> OBJECT CONVERSION ====================
// Converts a database event record to a webinar object
void webinar::convertetoobject(event_db e) {
    tm timeinfo = {0};
    timeinfo = virtualevent::converttime(e.datetime); // Convert datetime string to tm struct
    user u = return_user_by_id(e.organizer_id);       // Fetch organizer user details

    // Create a new webinar object from database fields
    webinar webinarevent(e.name, e.event_id, e.organizer_id, e.description, timeinfo, e.platform, e.capacity, u.getName());
    webinarevent.webtopic = e.webinar_topic;
    webinarevent.qnaenabled = e.questions_and_answers_enabled;

    *this = webinarevent; // Assign all data to current instance
}

// ==================== OBJECT -> DATABASE CONVERSION ====================
// Converts a webinar object to an event_db struct for database storage
event_db webinar::convertetostruct() {
    event_db eventinfo;

    // Populate all fields
    eventinfo.event_id = this->get_event_id();
    eventinfo.name = this->getname();
    eventinfo.description = this->getdescription();
    eventinfo.platform = this->getplatform();
    eventinfo.capacity = this->getcapacity();
    eventinfo.type = 1; // Type 1 for webinar
    eventinfo.organizer_id = this->get_org_id();
    eventinfo.datetime = this->timetostring(this->getdatetime());
    eventinfo.webinar_topic = this->webtopic;
    eventinfo.questions_and_answers_enabled = this->qnaenabled;
    eventinfo.current_attendees = this->getattendeescount();

    return eventinfo;
}