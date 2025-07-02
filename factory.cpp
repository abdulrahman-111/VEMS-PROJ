#include "factory.h"
#include <sstream>

// Constructor for eventclient class
// Initializes the event pointer to nullptr
eventclient::eventclient() {
    event = nullptr;
}

// Function to choose and create an event object based on the event type
// Parameters:
// - eventtype: the type of event to create
// - n: name of the event
// - id: unique event ID
// - orgid: ID of the organizer
// - desc: description of the event
// - date: scheduled date and time of the event
// - plat: platform or location of the event
// - cap: capacity (number of attendees allowed)
// - org: organizer name or info
// This function uses a factory method to create a specific event object and stores it in the event pointer
void eventclient::chooseevent(eventtype eventtype,
                              const std::string& n,
                              unsigned int id,
                              unsigned orgid,
                              const std::string& desc,
                              const std::tm& date,
                              const std::string& plat,
                              unsigned int cap,
                              const std::string& org) {
    event = virtualevent::build(eventtype, n, id, orgid, desc, date, plat, cap, org);
}

// Getter function to retrieve the created event object
// Returns a pointer to the virtualevent object stored in the event pointer
virtualevent* eventclient::get_event() {
    return event;
}
