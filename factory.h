#ifndef FACTORY_H
#define FACTORY_H

#include "virtualevent.h"

class eventclient {
    virtualevent* event;

public:
    eventclient();

    void chooseevent(eventtype eventtype,
                     const std::string& n,
                     unsigned int id,
                     unsigned orgid,
                     const std::string& desc,
                     const std::tm& date,
                     const std::string& plat,
                     unsigned int cap,
                     const std::string& org);

    virtualevent* get_event();

    ~eventclient() {
        if (event)
            delete event;
        event = nullptr;
    }
};

#endif
