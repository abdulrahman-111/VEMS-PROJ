#include "user.h"
#include "users.dbmanger.h"
#include "event_dbmanager.h"
#include "virtualevent.h"
#include "conference.h"
#include "run_prog.h"
#include "webinar.h"
#include "workshops.h"
#include "feedback.h"

// Add feedback to one of the user's created events
void user::update_created_event_feedback(virtualevent* event, feedback f) {
    for (auto& e : my_created_events) {
        if (e->get_event_id() == f.get_event_id()) {
            e->add_feedback(f);
        }
    }
}

// Destructor: free memory used by created and registered events
user::~user() {
    for (int i = 0; i < my_created_events.size(); i++) {
        delete my_created_events[i];
    }
    for (int i = 0; i < my_registered_events.size(); i++) {
        delete my_registered_events[i];
    }
}

// ==================== UserManager ====================
// Load all users and their events from the database
void UserManager::loadUsers() {
    users = get_all_users();
    for (int i = 0; i < users.size(); i++) {
        users[i].load_created_events();
        users[i].load_registered_events();
    }
}

// Login function
string UserManager::login() {
    string username, password;

    cout << "Username: ";
    getline(cin, username);
    cout << "Password: ";
    getline(cin, password); // In real use, hide this input

    // Check credentials
    for (const auto& u : users) {
        if (u.getUsername() == username && u.getPassword() == password) {
            cout << "Welcome back, " << u.getUsername() << "!\n";
            return u.getEmail();
        }
    }

    cout << "Invalid credentials.\n";
    return "0";
}

// Sign-up for new users
string UserManager::signUp() {
    string name, username, email, password, affiliation;

    cout << "Enter your Name: ";
    getline(cin, name);

    // Ensure unique username
    while (true) {
        cout << "Choose a username: ";
        getline(cin, username);
        bool taken = false;

        for (const auto& u : users) {
            if (u.getUsername() == username) {
                cout << "Username already taken. Choose another one.\n";
                taken = true;
                break;
            }
        }

        if (!taken) break;
    }

    // Ensure unique email
    while (true) {
        cout << "Enter email: ";
        getline(cin, email);
        bool taken = false;

        for (const auto& u : users) {
            if (u.getEmail() == email) {
                cout << "This email is already signed before. Choose another one.\n";
                taken = true;
                break;
            }
        }

        if (!taken) break;
    }

    cout << "Choose a password: ";
    getline(cin, password);

    cout << "Enter affiliation (optional): ";
    getline(cin, affiliation);

    user newUser(name, username, email, password, affiliation);

    // Save to database
    if (add_user(newUser)) {
        users.push_back(newUser);
        cout << "Registration successful!\n";
        return newUser.getEmail();
    }

    return signUp(); // Retry if saving fails
}

// Run login/signup process and return user email
string UserManager::run() {
    string choice, userEmail;
    cout << "Do you have an account? (Yes/No/Exit): ";
    while (true) {
        getline(cin, choice);

        if (choice[0] == 'y' || choice[0] == 'Y') {
            for (int i = 0; i < 3; ++i) {
                userEmail = login();
                if (userEmail != "0") return userEmail;
            }

            cout << "Your 3 trials are over. Create an account? (Yes/No): ";
            getline(cin, choice);
            if (choice[0] == 'n' || choice[0] == 'N') exit(0);
            else if (choice[0] == 'y' || choice[0] == 'Y') return signUp();
        } else if (choice[0] == 'n' || choice[0] == 'N') return signUp();
        else if (choice[0] == 'e' || choice[0] == 'E') exit(0);
        else cout << "Invalid choice\n";
    }
}

// ==================== Load Events from DB ====================
// Load all registered events for the user
void user::load_registered_events() {
    vector<event_db> event_dbs = get_all_registered_events_(user_id);
    virtualevent* event;

    for (auto& db : event_dbs) {
        if (db.type == 1) event = new webinar;
        else if (db.type == 2) event = new workshop;
        else if (db.type == 3) event = new conference;

        event->convertetoobject(db);
        event->load_attendees_fromDatabase();
        event->load_feedbacks_fromDatabase();
        event->load_waiting_list_fromDatabase();
        my_registered_events.push_back(event);
    }
}

// Load all created events by the user
void user::load_created_events() {
    vector<event_db> event_dbs = get_all_created_events_(user_id);
    virtualevent* event;

    for (auto& db : event_dbs) {
        if (db.type == 1) event = new webinar;
        else if (db.type == 2) event = new workshop;
        else if (db.type == 3) event = new conference;

        event->convertetoobject(db);
        event->load_attendees_fromDatabase();
        event->load_feedbacks_fromDatabase();
        event->load_waiting_list_fromDatabase();
        my_created_events.push_back(event);
    }
}

// ==================== Show Events ====================
// Display events created by the user
void user::show_created_events() {
    cout << "Ur created Events\n";
    if (my_created_events.empty()) {
        cout << "No events available.\n";
    } else {
        cout << "\nDisplaying all " << my_created_events.size() << " created events:\n";
        for (const auto& e : my_created_events) {
            user u = return_user_by_id(e->get_org_id());
            cout << "- " << e->getname() << " Overall Rating: " << e->get_overall_feedback()
                 << " (" << e->getplatform() << ") ";
            cout << "[Organizer: " << u.getName() << " (#" << e->get_org_id() << ")]\n";
        }
    }
}

// Display events the user registered in
void user::show_registered_events() {
    cout << "registered\n";
    if (my_registered_events.empty()) {
        cout << "No events available.\n";
    } else {
        cout << "\nDisplaying all " << my_registered_events.size() << " Registers events:\n";
        for (const auto& e : my_registered_events) {
            user u = return_user_by_id(e->get_org_id());
            cout << "- " << e->getname() << " Overall Rating: " << e->get_overall_feedback()
                 << " (" << e->getplatform() << ") ";
            cout << "[Organizer: " << u.getName() << " (#" << e->get_org_id() << ")]\n";
        }
    }
}

// Register the user for an event
void user::register_event(virtualevent* event) {
    if (event->registerAttendee(*this)) {
        my_registered_events.push_back(event);
    }
}

// Unregister the user from an event using event ID
void user::unregister_event(int event_id) {
    virtualevent* targetEvent = nullptr;

    // Find the event in registered events
    for (auto it = my_registered_events.begin(); it != my_registered_events.end(); ++it) {
        if ((*it)->get_event_id() == event_id) {
            targetEvent = *it;
            my_registered_events.erase(it);
            unregisterAttendee_db(this->getUserID(), event_id);
            break;
        }
    }

    if (!targetEvent) {
        cout << " Event not found in your registered list.\n";
        return;
    }

    // Remove from attendee list in event
    targetEvent->unregisterAttendee(*this);
    cout << " You have successfully unregistered from \"" << targetEvent->getname() << "\".\n";
}

// Cancel a created event
void user::cancel_created_event(virtualevent* event) {
    bool found = false;
    for (int i = 0; i < my_created_events.size(); i++) {
        if (my_created_events[i]->get_event_id() == event->get_event_id()) {
            my_created_events.erase(my_created_events.begin() + i);
            found = true;
        }
        if (found) break;
    }
    if (!found) {
        cout << "No events created by that name.\n";
    }
}

// Update date of a created event
void user::update_created_event(virtualevent* event, const string& date) {
    bool found = false;
    for (int i = 0; i < my_created_events.size(); i++) {
        if (my_created_events[i]->get_event_id() == event->get_event_id()) {
            my_created_events[i]->setdate(date);
            found = true;
        }
        if (found) break;
    }
    if (!found) {
        cout << "No events created by that name.\n";
    }
}
