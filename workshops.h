#ifndef WORKSHOP_H
#define WORKSHOP_H

#include "virtualevent.h"

// ==================== CLASS DEFINITION: workshop ====================
// This class represents a Workshop event type, derived from virtualevent
class workshop : public virtualevent {
    std::vector<std::string> breakoutrooms;       // List of breakout room names
    std::vector<std::string> materialsrequired;   // List of required materials for the workshop

public:
    // Default constructor
    workshop() {}

    // Parameterized constructor to initialize all event details
    workshop(const std::string& n,
             unsigned int id,
             unsigned int orgid,
             const std::string& desc,
             const std::tm& date,
             const std::string& plat,
             unsigned int cap,
             const std::string& org);

    // Used to gather additional data related to the workshop
    void getter();

    // Returns the list of breakout rooms
    const std::vector<std::string>& getBreakoutRooms() const { return breakoutrooms; }

    // Override: Returns a string with all event details
    std::string geteventdetails() const;

    // Destructor
    ~workshop() {}

    // Sends reminders to attendees
    void sendReminder() const;

    // Converts database record to workshop object
    void convertetoobject(event_db e);

    // Concatenates all materials into a single string
    std::string concatinatematerials();

    // Converts workshop object into database record format
    event_db convertetostruct();

    // Concatenates all breakout room names into a single string
    std::string concatinaterooms();
};

#endif