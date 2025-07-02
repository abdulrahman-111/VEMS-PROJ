#ifndef CONFERENCE_H
#define CONFERENCE_H

#include <map>
#include "virtualevent.h"

// Structure representing a speaker with name, affiliation and role
struct speaker {
    std::string name;
    std::string association;
    std::string role;
};

// Structure representing a conference session
struct session {
    std::string topic;      // Session topic/title
    speaker tutor;          // Session speaker
    int id;                 // Session ID
    std::string url;        // Session URL/link
};

// Conference class inheriting from virtualevent
class conference : public virtualevent {
    std::map<std::string, session> sessiontrack;  // Stores all sessions keyed by topic

public:
    conference() {}  // Default constructor

    // Parameterized constructor
    conference(const std::string& n,
               unsigned int id,
               unsigned int orgid,
               const std::string& desc,
               const std::tm& date,
               const std::string& plat,
               unsigned int cap,
               const std::string& org);

    // Converts all tutor info to formatted string
    std::string converttutor();

    // Adds a new session to the conference
    void addSessionToTrack();

    // Displays info about a specific session
    void session_info(std::map<std::string, session> sessiontrack);

    // Interactive method to add multiple sessions
    void getter();

    ~conference() {}  // Destructor

    // Returns formatted string with all conference details
    std::string geteventdetails() const ;

    // Sends conference reminder
    void sendReminder() const;

    // Converts database structure to conference object
    void convertetoobject(event_db e);

    // Converts conference object to database structure
    event_db convertetostruct();

    // Returns session track as formatted string
    std::string gettrack(std::map<std::string, session> sessiontrack);
};

#endif