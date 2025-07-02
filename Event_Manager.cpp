#include "Event_Manager.h"
#include "user.h"
#include "virtualevent.h"
#include "run_prog.h"

// ==================== STATIC MEMBER INITIALIZATION ====================
EventManager* EventManager::ptr = nullptr;

// ==================== HELPER FUNCTIONS ====================
void clear_screen() {
    system("cls||clear");
}

// ==================== MENU FUNCTIONS ====================
// Display the main menu and handle user choices


void EventManager::disp_menu() {
    int choice;
    do {
        clear_screen();
        cout << "\n========== 5leha 3la Allah VEMS ==========\n";
        cout << "Welcome, " << user_run.getName() << "!\n";
        cout << "-----------------------------------------\n";
        cout << "1. Create New Event\n";
        cout << "2. Manage My Events\n";
        cout << "3. View Registered Events\n";
        cout << "4. Browse All Events\n";
        cout << "5. Credits\n";
        cout << "6. Logout\n";
        cout << "-----------------------------------------\n";

        cout << "Enter your choice (1-6): ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        while (choice < 1 || choice > 6) {
            cout << "Invalid choice. Please select 1-6: ";
            cin >> choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

            switch (choice) {
                case 1: create_choice(); break;
                case 2: manage_choice(); break;
                case 3: show_choice_3(); break;
                case 4: show_choice_4(); break;
                case 5: cout<<"\n 5leha 3la Allah \n"; break;
                case 6:
                    cout << "\nLogging out... Goodbye!\n";
                    VMS();
                break;
            }
        } while (choice != 6);
    }
// Handle creation of a new event and optionally allow creating another

void EventManager::create_choice() {
    clear_screen();
    create_event();
    int choice;
    do {
        cout << "\n=== EVENT CREATION OPTIONS ===\n";
        cout << "1. Create Another Event\n";
        cout << "2. Return to Main Menu\n";
        cout << "-----------------------------\n";
        while (true) {
            cout << "Enter your choice (1-3): ";
            cin >> choice;
            if (cin.fail()) {
                cin.clear(); // clear the fail state
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter a number between 1 and 2.\n";
            } else if (choice < 1 || choice > 2) {
                cout << "Invalid choice. Please select a number between 1 and 2.\n";
            } else {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
            }
        }

        if (choice == 1) create_event();
    } while (choice != 2);
}

// ==================== EVENT MANAGEMENT ====================
// Manage events created by the current user

void EventManager::manage_choice() {
    clear_screen();
    user_run.show_created_events();
    string newdate;
    if(user_run.get_num_created()==0) {
        cout<<"Enter 0 to return to menu: ";
        int i;
        cin>>i;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        while (i!=0) {
            cout << "Invalid choice. Please select 0: ";
            cin >> i;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        return;
    }

    cout << "\nEnter the name of the event you want to manage: (or 'back' to return):";
    string Event_name;
    getline(cin, Event_name);
    if (Event_name == "back") return;

    virtualevent* event = findEventByName(Event_name, user_run);
    if (event) {
        int choice;
        do {
            cout << "\n=== EVENT MANAGEMENT MENU ===\n";
            cout << "1. Cancel This Event\n";
            cout << "2. Reschedule Event\n";
            cout << "3. View Event Details\n";
            cout << "4. View Attendee List\n";
            cout << "5. View Feedback\n";
            cout << "6. View Waiting List\n";
            cout << "7. Select Different Event\n";
            cout << "8. Return to Main Menu\n";
            cout << "-----------------------------\n";
            cout << "Enter your choice (1-8): ";
            cin >> choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            while (choice < 1 || choice > 8) {
                cout << "Invalid choice. Please select 1-8: ";
                cin >> choice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }

            switch (choice) {
                case 1: cancelEvent(Event_name, user_run); break;
                case 2:
                    cout << "Enter new date and time (YYYY-MM-DD HH:MM:SS): ";
                    getline(cin, newdate);
                    reschedule(event->getname(), newdate);
                    break;
                case 3: cout << event->geteventdetails(); break;
                case 4: event->showAttendees(); break;
                case 5: event->get_all_event_feedback(); break;
                case 6: event->show_waiting_list(); break;
                case 7: manage_choice();
                case 8: return;
            }
        } while (choice != 8);
    } else {
        cout << "\nError: Event not found in your created events.\n";
        manage_choice();
    }
}
// Display events the user is registered in and allow actions on them

void EventManager::show_choice_3() {
    clear_screen();
    user_run.show_registered_events();
    if(user_run.get_num_registered()==0) {
        cout<<"Enter 0 to return to menu: ";
        int i;
        cin>>i;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        while (i!=0) {
            cout << "Invalid choice. Please select 1-8: ";
            cin >> i;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        return;
    }
    cout << "\nEnter the name of the event you want to view(or 'back' to return):   ";
    string Event_name;
    getline(cin, Event_name);
    if (Event_name == "back") return;
    cout<<"Enter org_code:  ";
    int id;
    cin>>id;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    //find from registered
    virtualevent* event = findEventByName(Event_name,user_run,id);
    if (event) {
        int event_id = event->get_event_id();
        int choice;
        do {
            cout << "\n=== REGISTERED EVENT OPTIONS ===\n";
            cout << "1. Unregister from This Event\n";
            cout << "2. View Event Details\n";
            cout << "3. Submit Feedback\n";
            cout << "4. Select Different Event\n";
            cout << "5. Return to Main Menu\n";
            cout << "-----------------------------\n";
            cout << "Enter your choice (1-5): ";
            cin >> choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            while (choice < 1 || choice > 5) {
                cout << "Invalid choice. Please select 1-5: ";
                cin >> choice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            switch (choice) {
                case 1: user_run.unregister_event(event_id); break;
                case 2: cout << event->geteventdetails(); break;
                case 3: enter_feedback(event); break;
                case 4: show_choice_3();
                case 5: return;
            }
        } while (choice != 5);
    } else {
        cout << "\nError: Event not found in your registered events.\n";
        show_choice_3();
    }
}
// Browse all events and register or view details

void EventManager::show_choice_4() {
    clear_screen();
    showAllEvents();

    search_events();

    cout << "\nEnter the name of the event you're interested in (or 'back' to return to menu): ";
    string Event_name;
    getline(cin, Event_name);

    if (Event_name == "back") return;
    cout<<"Enter org_code:  ";
    int id;
    cin>>id;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    virtualevent* event = findEventByName(Event_name,id);
    if (event) {
        int choice;
        do {
            cout << "\n=== EVENT BROWSING OPTIONS ===\n";
            cout << "1. Register for This Event\n";
            cout << "2. View Event Details\n";
            cout << "3. New Search\n";
            cout << "4. Return to Main Menu\n";
            cout << "-----------------------------\n";
            cout << "Enter your choice (1-4): ";
            cin >> choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            while (choice < 1 || choice > 4) {
                cout << "Invalid choice. Please select 1-4: ";
                cin >> choice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }

            switch (choice) {
                case 1: user_run.register_event(event); break;
                case 2: cout << event->geteventdetails(); break;
                case 3: show_choice_4();
                case 4: return;
            }
        } while (choice != 4);
    } else {
        cout << "\nError: Event not found. Please try again.\n";
        show_choice_4();
    }
}

// ==================== FEEDBACK MANAGEMENT ====================
// Allow user to submit feedback for a specific event

void EventManager::enter_feedback(virtualevent* event) {
    feedback f;
    string comment;
    int review;

    cout << "\n=== EVENT FEEDBACK ===\n";
    cout << "Please share your experience (max 500 characters):\n";
    getline(cin, comment);

    do {
        cout << "Rate this event (1-5 stars): ";
        cin >> review;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        while (review < 1 || review > 5) {
            cout << "Invalid choice. Please select a number between 1 and 5.\n";
            cin >> review;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    } while (review < 1 || review > 5);

    f.set_user_id(user_run.getUserID());
    f.set_event_id(event->get_event_id());
    f.setcomments(comment);
    f.setrating(review);

    event->add_feedback(f);
    user_run.update_created_event_feedback(event, f);
    if (f.save()) {
        cout << "\nThank you for your valuable feedback!\n";
    } else {
        cout << "\nError: Could not save your feedback. Please try again.\n";
    }
}

// ==================== EVENT SEARCH ====================
// Display search criteria and show matching events from database

void EventManager::search_events() {
    event_criteria ec = get_filter();
    clear_screen();
    cout << "\nSearching with criteria:\n";
    cout << "- Name: " << (ec.name.empty() ? "Any" : ec.name) << "\n";
    cout << "- Date: " << (ec.datetime.empty() ? "Any" : ec.datetime) << "\n";
    cout << "- End-date: " << (ec.end_date.empty() ? "Any" : ec.end_date) << "\n";
    cout << "- Start-date: " << (ec.start_date.empty() ? "Any" : ec.start_date) << "\n";
    cout << "- Oraganizer: " << (ec.organizer==0 ? "Any" : to_string(ec.organizer)) << "\n";
    cout << "- Platform: " << (ec.platform.empty() ? "Any" : ec.platform) << "\n";
    cout << "- Type: " << (ec.type==0 ? "Any" :to_string( ec.type)) << "\n";
    cout << "- Min Capacity: " << (ec.min_capacity==0 ? "Any" : to_string(ec.min_capacity)) << "\n";
    cout << "- Max Capacity: " << (ec.max_capacity==0 ? "Any" :to_string( ec.max_capacity)) << "\n";
    vector<event_db> result = dynamic_search(ec);
    cout << "\nFound " << result.size() << " matching events:\n";
    for (auto& event_i : result) {
        user u = return_user_by_id(event_i.organizer_id);
        cout << "- " << event_i.name <<" ";
        cout << "[Organizer: " << u.getName() << " (#" << event_i.organizer_id << ")]\n";
    }
}
// Get search filter input from user for event searching

event_criteria EventManager::get_filter() {
    event_criteria ec;
    int choice;
    do {
        cout << "\n=== SEARCH FILTERS ===\n";
        cout << "1. Filter by Event Name\n";
        cout << "2. Filter by Event Type\n";
        cout << "3. Filter by Organizer\n";
        cout << "4. Filter by Exact Date\n";
        cout << "5. Filter by Start Date\n";
        cout << "6. Filter by End Date\n";
        cout << "7. Filter by Minimum Capacity\n";
        cout << "8. Filter by Maximum Capacity\n";
        cout << "9. Filter by Platform\n";
        cout << "0. Execute Search\n";
        cout << "-----------------------------\n";
        cout << "Enter filter option (0-9): ";
            cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                while(choice < 0 || choice > 9) {
                    cout << "Invalid choice. Please select a number between 0 and 9.\n";
                    cin >> choice;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }

        switch (choice) {
            case 1: cout << "Enter event name: "; getline(cin, ec.name); break;
            case 2: cout << "Enter event type (1=Webinar, 2=Workshop, 3=Conference): ";
            cin >> ec.type;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
            case 3: cout << "Enter organizer ID: "; cin >> ec.organizer; break;
            case 4: cout << "Enter exact date (YYYY-MM-DD HH:MM:SS): "; getline(cin, ec.datetime); break;
            case 5: cout << "Enter start date (YYYY-MM-DD): "; getline(cin, ec.start_date); break;
            case 6: cout << "Enter end date (YYYY-MM-DD): "; getline(cin, ec.end_date); break;
            case 7: cout << "Enter minimum capacity: ";
            cin >> ec.min_capacity;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
            case 8: cout << "Enter maximum capacity: ";
            cin >> ec.max_capacity;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
            case 9: cout << "Enter platform name: "; getline(cin, ec.platform); break;
            case 0: cout << "\nSearching events...\n"; break;
           // default: cout << "Invalid filter option.\n"; break;
        }
    } while (choice != 0);
    return ec;
}

// ==================== EVENT DISPLAY ====================
// Display all available events with their details

void EventManager::showAllEvents() const {
    if (events.empty()) {
        cout << "\nCurrently no events available.\n";
    } else {
        cout << "\nDisplaying all " << events.size() << " events:\n";
        for (const auto& e : events) {
            user u = return_user_by_id(e->get_org_id());
            cout << "- " << e->getname() <<" Overall Rating: "<<e->get_overall_feedback() <<" (" << e->getplatform() << ") ";
            cout << "[Organizer: " << u.getName() << " (#" << e->get_org_id() << ")]\n";
        }
    }
}

// ==================== UTILITY FUNCTIONS ====================
// Find event by name and organizer ID from all events
// Find event by name from user's created events
// Find event by name and organizer ID from user's registered events

virtualevent* EventManager::findEventByName(const string& name,int org_id) const {
    for (const auto& e : events) {
        if (e->getname() == name&&e->get_org_id()==org_id) {
            return e;
        }
    }
    cout << "\nError: No event found with that name for this host.\n";
    return nullptr;
}
// created events
virtualevent* EventManager::findEventByName(const string& name, const user& user) const {
    for (const auto& e : user.my_created_events) {
        if (e->getname() == name) {
            return e;
        }
    }
    cout << "\nError: You have not created an event with that name.\n";
    return nullptr;
}
//registered
virtualevent* EventManager::findEventByName(const string& name, const user& user,int org_id) const {
    for (const auto& e : user.my_registered_events) {
        if (e->getname() == name && e->get_org_id()==org_id) {
            return e;
        }
    }
    cout << "\nError: You have not registered an event with that name for this organizer.\n";
    return nullptr;
}

// ==================== EVENT MODIFICATION ====================
// Cancel a specific event created by the user

void EventManager::cancelEvent(const string& name, const user& user) {
    for (auto it = events.begin(); it != events.end(); ++it) {
        if ((*it)->getname() == name) {
            if ((*it)->getorganizer() != user.getUserID()) {
                cout << "\nError: Only the event organizer can cancel this event.\n";
                return;
            }
            (*it)->notify_attendees_cancelation();
            cancel_event_by_name((*it)->get_event_id());
            delete_event_feedback((*it)->get_event_id());
            user_run.cancel_created_event(*it);
            events.erase(it);
            cout << "\nSuccess: Event \"" << name << "\" has been cancelled.\n";
            return;
        }
    }
    cout << "\nError: Could not find event with that name.\n";
}

void EventManager::reschedule(const string& name, const string& newDate) {
    // Reschedule the date of an event and update attendees
    auto e = findEventByName(name, user_run);
    if (e) {
        e->setdate(newDate);
        e->notify_attendees_postponing(newDate);
        cout << "\nSuccess: Event \"" << name << "\" has been rescheduled.\n";
        update_event_date(e->get_event_id(), newDate);
        for (auto& event : events) {
            if (event->get_event_id() == e->get_event_id()) {
                event->setdate(newDate);
            }
        }
    }
}

// ==================== DATABASE OPERATIONS ====================
// Load all events from database into memory, including attendees, feedback, and waiting lists

void EventManager::load_db() {
    if(!events.empty()){return;}
    vector<event_db> event_dbs = get_all_events();
    virtualevent* event;

    for (auto& db : event_dbs) {
        if (db.type == 1) event = new webinar;
        else if (db.type == 2) event = new workshop;
        else event = new conference;

        event->convertetoobject(db);
        event->load_attendees_fromDatabase();
        event->load_feedbacks_fromDatabase();
        event->load_waiting_list_fromDatabase();
        events.push_back(event);
    }
}

// ==================== EVENT CREATION ====================
// Create a new event with user input (type, name, date, capacity, etc.)


eventclient* EventManager::create_event() {
    cout << "\n========== CREATE NEW EVENT ==========\n";

    eventtype eventname;
    string str;
    cout << "Enter event type (Workshop/Webinar/Conference): ";
    cin >> str;

    for (char& c : str) c = tolower(c);

    map<string, eventtype> eventMap = {
        {"workshop", Workshop},
        {"webinar", Webinar},
        {"conference", Conference}
    };

    auto it = eventMap.find(str);
    if (it == eventMap.end()) {
        cerr << "\nError: Invalid event type specified.\n";
        return nullptr;
    }
    eventname = it->second;

    string name, desc, platform;
    cout << "Event name: "; cin.ignore(); getline(cin, name);
    cout << "Description: "; getline(cin, desc);
    cout << "Platform: "; getline(cin, platform);

    int capacity, orgid = user_run.getUserID();
    string org = user_run.getName();
    cout << "Maximum attendees: ";
    cin >> capacity;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    tm timeinfo = {};
    string input;
    cout << "Event date/time (YYYY-MM-DD HH:MM:SS): ";
    getline(cin, input);

    istringstream iss(input);
    char discard;
    if (!(iss >> timeinfo.tm_year >> discard >> timeinfo.tm_mon >> discard >> timeinfo.tm_mday
              >> timeinfo.tm_hour >> discard >> timeinfo.tm_min >> discard >> timeinfo.tm_sec)) {
        cerr << "\nError: Invalid date/time format entered.\n";
        return nullptr;
    }

    timeinfo.tm_year -= 1900;
    timeinfo.tm_mon -= 1;
    time_t temp = mktime(&timeinfo);
    if (temp == -1 || !localtime(&temp)) {
        cerr << "\nError: Could not process the entered date/time.\n";
        return nullptr;
    }

    timeinfo = *localtime(&temp);

    eventclient* event = new eventclient();
    event->chooseevent(eventname, name, 1, orgid, desc, timeinfo, platform, capacity, org);
    event->get_event()->getter();
    addEvent(event->get_event());

    return event;
}
// Save new event to database and add it to current user's created events

void EventManager::addEvent(virtualevent* event) {
    event_db temp = event->convertetostruct();
    if (add_event(temp)) {
        event->set_id(temp.event_id);
        events.push_back(event);
        user_run.set_created_event(event);
        cout << "\nSuccess: Event created successfully!\n";
    }
}
