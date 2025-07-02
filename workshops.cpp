#include"workshops.h"
#include<sstream>
#include <limits>

// Constructor for workshop class
// Initializes the workshop by forwarding values to the base class virtualevent
workshop::workshop(const std::string& n,unsigned int id,
                   unsigned int orgid,
                   const std::string& desc,
                   const std::tm& date,
                   const std::string& plat,
                   unsigned int cap,
                   const std::string& org): virtualevent(n,id,orgid,desc,date,plat,cap,org){}

// Prompts user to enter materials and breakout rooms interactively
void workshop::getter() {
    std::cout << "Enter materials required (one per line, type 'done' to finish):\n";
    materialsrequired.clear(); // Clear existing materials if any

    std::string material;
    while (true) {
        getline(std::cin, material);
        if (material == "done" || material.empty()) {
            break;
        }
        materialsrequired.push_back(material);
    }

    int num;
    std::string room;
    std::cout<<"enter the number of the breakout rooms you want : ";
    std::cin>>num;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear input buffer

    std::cout << "Enter the names of the room"<<std::endl;
    for(int i=0;i<num;i++){
        std::cout<<"room "<<i+1<<" : ";
        getline(std::cin,room);
        breakoutrooms.push_back(room);
    }
}

// Builds and returns a detailed string description of the workshop
std::string workshop::geteventdetails() const {
    std::ostringstream oss;
    oss << virtualevent::geteventdetails() << "\n"
        << "Type: Workshop\n"
        << "Materials Required: ";

    for (const auto& material : materialsrequired) {
        oss << material << ", ";
    }

    oss << "\nBreakout Rooms: ";
    for (const auto& room : breakoutrooms) {
        oss << room << ", ";
    }

    return oss.str();
}

// Sends reminder and prints full event details
void workshop::sendReminder() const {
    virtualevent::sendReminder();
    std::cout<<workshop::geteventdetails();
}

// Converts an event_db structure into a workshop object
void workshop::convertetoobject(event_db e) {
    tm timeinfo = {0};
    timeinfo=virtualevent::converttime(e.datetime); // Parse datetime string into tm struct
    user u = return_user_by_id(e.organizer_id); // Fetch organizer by ID

    // Create a temporary workshop object using data from the struct
    workshop workshopevent(e.name,e.event_id,e.organizer_id,e.description,timeinfo,e.platform,e.capacity,u.getName());

    // Parse and store breakout rooms (comma-separated)
    if (!e.breakout_rooms.empty()) {
        std::istringstream iss(e.breakout_rooms);
        std::string room;
        while (std::getline(iss, room, ',')) {
            room.erase(0, room.find_first_not_of(" \t\n\r\f\v")); // Trim leading whitespace
            room.erase(room.find_last_not_of(" \t\n\r\f\v") + 1); // Trim trailing whitespace
            if (!room.empty()) {
                workshopevent.breakoutrooms.push_back(room);
            }
        }
    }

    // Parse and store materials (comma-separated)
    if (!e.materials_required.empty()) {
        std::istringstream mat_iss(e.materials_required);
        std::string material;
        while (std::getline(mat_iss, material, ',')) {
            material.erase(0, material.find_first_not_of(" \t\n\r\f\v"));
            material.erase(material.find_last_not_of(" \t\n\r\f\v") + 1);
            if (!material.empty()) {
                workshopevent.materialsrequired.push_back(material);
            }
        }
    }

    // Copy all parsed data into current object
    *this=workshopevent;
}

// Concatenates all workshop materials into a single comma-separated string
std::string workshop::concatinatematerials() {
    std::ostringstream material;
    const std::string delimiter = ",";

    for(size_t i = 0; i < this->materialsrequired.size(); ++i) {
        material << this->materialsrequired[i];
        if(i != this->materialsrequired.size() - 1) {
            material << delimiter;
        }
    }

    return material.str();
}

// Concatenates all breakout room names into a comma-separated string
std::string workshop::concatinaterooms() {
    std::ostringstream rooms;
    const std::string delimiter = ",";

    for(size_t i = 0; i < this->breakoutrooms.size(); ++i) {
        rooms << this->breakoutrooms[i];
        if(i != this->breakoutrooms.size() - 1) {
            rooms << delimiter;
        }
    }

    return rooms.str();
}

// Converts the workshop object into an event_db structure to be saved or processed
event_db workshop::convertetostruct(){
    event_db eventinfo;
    eventinfo.event_id=this->get_event_id();
    eventinfo.name=this->getname();
    eventinfo.description=this->getdescription();
    eventinfo.platform=this->getplatform();
    eventinfo.capacity=this->getcapacity();
    eventinfo.type=2; // 2 stands for Workshop type
    eventinfo.organizer_id=this->get_org_id();
    eventinfo.datetime=this->timetostring(this->getdatetime()); // Convert time to string
    eventinfo.materials_required=this->concatinatematerials();
    eventinfo.breakout_rooms=this->concatinaterooms();
    eventinfo.current_attendees=this->getattendeescount();
    return eventinfo;
}