#ifndef USER_H
#define USER_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <sstream>

class feedback;              // Forward declaration of feedback class
class virtualevent;          // Forward declaration of virtualevent class
class EventManager;          // Forward declaration of EventManager class

using namespace std;

// ==================== User Class ====================
class user {
    // ---------- Private Members ----------
    int user_id;                        // Unique ID for the user
    string Name;                        // Full name of the user
    string username;                    // Login username
    string email;                       // User email
    string password;                    // User password
    string affiliation;                // Organization or institution (optional)

    vector<virtualevent*> my_created_events;     // Events created by the user
    vector<virtualevent*> my_registered_events;  // Events the user is registered for

    friend class EventManager;         // Allow EventManager to access private data

public:
    // ---------- Constructors & Destructor ----------
    user() {} // Default constructor

    // Constructor with full user info
    user(const string& name, const string& username, const string& email,
         const string& password, const string& affiliation = "")
        : Name(name), username(username), email(email),
          password(password), affiliation(affiliation) {}

    ~user(); // Destructor

    // ---------- Authentication ----------
    int getUserID() const { return user_id; }
    string getUsername() const { return username; }
    string getEmail() const { return email; }
    string getPassword() const { return password; }

    void setUsername(string u) { username = u; }
    void setEmail(string e) { email = e; }
    void setPassword(string p) { password = p; }
    void setuser_id(int id) { user_id = id; }

    // ---------- Event Management ----------
    // Add event to created list
    void set_created_event(virtualevent* event) {
        my_created_events.push_back(event);
    }

    // Add event to registered list
    void set_registerd_event(virtualevent* event) {
        my_registered_events.push_back(event);
    }

    int get_num_created() { return my_created_events.size(); }
    int get_num_registered() { return my_registered_events.size(); }

    // Manage event updates and cancelations
    void cancel_created_event(virtualevent* event);
    void update_created_event(virtualevent* event, const string& date);
    void update_created_event_feedback(virtualevent* event, feedback f);

    // Load events from database or file
    void load_created_events();
    void load_registered_events();

    // Display events on screen
    void show_created_events();
    void show_registered_events();

    // Register and unregister from events
    void register_event(virtualevent* event);
    void unregister_event(int event_id); // Not implemented yet

    // ---------- Profile Info ----------
    string getName() const { return Name; }
    string getAffiliation() const { return affiliation; }

    void setName(string n) { Name = n; }
    void setAffiliation(string f) { affiliation = f; }

    // Show user details
    void display_user_info() const {
        cout << "Name: " << Name << ", Email: " << email;
        if (!affiliation.empty())
            cout << ", Affiliation: " << affiliation;
        cout << endl;
    }

    // ---------- Operator Overload ----------
    // Compare users by email
    bool operator==(const user& other) const {
        return email == other.email;
    }
};

// ==================== UserManager Class ====================
class UserManager {
    vector<user> users; // List of all users in the system

    // Load users from file or database
    void loadUsers();

public:
    // Constructor loads user data
    UserManager() {
        loadUsers();
    }

    ~UserManager() {}

    // Get user by email
    user get_user(string email) {
        user temp;
        for (int i = 0; i < users.size(); i++) {
            if (users[i].getEmail() == email) {
                temp = users[i];
                break;
            }
        }
        return temp;
    }

    // Login and registration methods
    string login();       // Handles user login
    string signUp();      // Handles new user registration
    string run();         // Runs login/signup and returns email
};

#endif // USER_H