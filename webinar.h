#ifndef WEBINAR_H
#define WEBINAR_H

#include"virtualevent.h"  // Base class header
#include<sstream>         // For string stream operations

// Class representing a webinar, inherits from virtualevent
class webinar:public virtualevent{
    bool qnaenabled;      // Flag indicating if Q&A feature is enabled
    std::string webtopic; // Topic/subject of the webinar

public:
    // Default constructor
    webinar (){}

    // Parameterized constructor
    webinar(const std::string& n,unsigned int id,
            unsigned int orgid,
            const std::string& desc,
            const std::tm& date,
            const std::string& plat,
            unsigned int cap,
            const std::string& org);

    // Method to get webinar details (implementation elsewhere)
    void getter();

    // Method to send reminder about the webinar
    void sendReminder() const;

    // Destructor
    ~webinar() {
    }

    // Returns formatted string with webinar details
    std::string geteventdetails()const;

    // Converts database structure to webinar object
    void convertetoobject(event_db e);

    // Converts webinar object to database structure
    event_db convertetostruct( );
};
#endif