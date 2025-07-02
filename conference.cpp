#include "conference.h"
#include <sstream>
#include<limits>

// Constructor - Initializes conference with base virtualevent parameters
conference::conference(const std::string& n, unsigned int id,
                       unsigned int orgid,
                       const std::string& desc,
                       const std::tm& date,
                       const std::string& plat,
                       unsigned int cap,
                       const std::string& org)
    : virtualevent(n, id, orgid, desc, date, plat, cap, org) {
}

// Returns formatted string with all conference details including sessions
std::string conference::geteventdetails() const {
    std::ostringstream oss;
    oss << virtualevent::geteventdetails() << "\n"
        << "Type: conference\n"
        << "Session track ";
    for (const auto& [name, session] : sessiontrack) {
        oss << "  - Session: " << name << "\n"
            << "    Details:\n"
            << "      Speaker name: " << session.tutor.name << "\n"
            << "      affiliation: " << session.tutor.association << "\n"
            << "      role: " << session.tutor.role << "\n"
            << "      ID: " << session.id << "\n"
            << "      URL: " << session.url << "\n";
    }
    return oss.str();
}

// Returns formatted string of all tutor/speaker information
std::string conference::converttutor() {
    std::ostringstream tutor;
    for (const auto& [name, session] : sessiontrack) {
        tutor << "      Speaker name: " << session.tutor.name << "\n"
            << "      affiliation: " << session.tutor.association << "\n"
            << "      role: " << session.tutor.role << "\n";
    }
    return tutor.str();
}

// Interactive method to add a new session to the conference
void conference::addSessionToTrack() {
        session session1;
        std::cout << "enter the session name: ";
         getline(cin,session1.topic );
        std::cout << "enter the session url: ";
        getline(cin, session1.url);
        std::cout << "enter the tutor name: ";
        getline(cin,session1.tutor.name);
        std::cout << "enter the tutor affiliation: ";
        getline(cin,session1.tutor.association);
        std::cout << "And its role: ";
        getline(cin,session1.tutor.role);
        std::cout << "enter the session id: ";
        std::cin >> session1.id;
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear input buffer
        sessiontrack.insert({session1.topic, session1});
}

// Returns session track data as pipe-delimited string for storage
std::string conference::gettrack(std::map<std::string, session> sessiontrack) {
    std::ostringstream track;
    const std::string delimiter = "|";
    for (auto &[name, session]: sessiontrack) {
        track << name << delimiter
              << session.id << delimiter
              << session.url << delimiter
              << session.tutor.name << delimiter
              << session.tutor.association << delimiter
              << session.tutor.role << delimiter;
    }
    return track.str();
}

// Displays information about a specific session by name
void conference::session_info(std::map<std::string, session> sessiontrack) {
    std::string sessionname;
    std::cout << "enter the name of the session you want its info: ";
    getline(cin, sessionname);

    auto session = sessiontrack.find(sessionname);
    if (session != sessiontrack.end()) {
        std::cout << "session: " << session->first
                  << " session details -> speaker: " << session->second.tutor.name
                  << " id: " << session->second.id
                  << " url: " << session->second.url;
    } else {
        std::cout << "no session with this name in this conference";
    }
}

// Sends reminder including all conference details
void conference::sendReminder() const {
    virtualevent::sendReminder();
    std::cout << conference::geteventdetails();
}

// Interactive method to add multiple sessions to conference
void conference::getter() {
    int sessionnum;
    std::cout << "enter the number of session of the conference: ";
    std::cin >> sessionnum;
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear input buffer

    for (int i = 0; i < sessionnum; i++) {
        std::cout << "session [" << i + 1 << "] details" << std::endl;
        conference::addSessionToTrack();
    }
}

// Converts database structure to conference object
void conference::convertetoobject(event_db e) {
    tm timeinfo = {0};
    timeinfo = virtualevent::converttime(e.datetime);
    user u = return_user_by_id(e.organizer_id);

    conference conferenceevent(e.name, e.event_id, e.organizer_id, e.description, timeinfo,
                               e.platform, e.capacity, u.getName());

    char delimeter;
    std::istringstream iss(e.sessions_track);
    session session1;

    while ((iss >> session1.topic >> delimeter >> session1.id >> delimeter >> session1.url
                >> session1.tutor.name >> session1.tutor.association >> delimeter >> session1.tutor.role)) {
        conferenceevent.sessiontrack.insert({session1.topic, session1});
    }

    *this = conferenceevent;
}

// Converts conference object to database structure
event_db conference::convertetostruct() {
    event_db eventinfo;
    eventinfo.event_id=this->get_event_id();
    eventinfo.name = this->getname();
    eventinfo.description = this->getdescription();
    eventinfo.platform = this->getplatform();
    eventinfo.capacity = this->getcapacity();
    eventinfo.organizer_id = this->get_org_id();
    eventinfo.type = 3;
    eventinfo.datetime = this->timetostring(this->getdatetime());
    eventinfo.event_id = this->get_event_id();
    eventinfo.sessions_track = this->gettrack(this->sessiontrack);
    eventinfo.current_attendees=this->getattendeescount();
    eventinfo.tutor=this->converttutor();
    return eventinfo;
}