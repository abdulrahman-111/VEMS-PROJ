#ifndef FEEDBACK_H
#define FEEDBACK_H

#include <iostream>
#include <string>
using namespace std;

// ===============================
// Class: feedback
// Purpose: Stores and manages feedback data from users for events
// ===============================
class feedback {
private:
    int user_id;               // ID of the user submitting the feedback
    int event_id;              // ID of the event the feedback is related to
    int rating;                // Rating given by the user (e.g., 1 to 5)
    string comments;           // User's written feedback
    string submission_date;    // Date when the feedback was submitted

public:
    // Default constructor
    // Initializes values to default; does not interact with any database
    feedback(): user_id(0), rating(0), comments(""), event_id(0) {}

    // Parameterized constructor
    // Initializes feedback with specific values
    feedback(int userID, int eventID, int r, string& c);

    // Saves the feedback to storage (e.g., file or database)
    bool save();

    // =======================
    // Setters
    // =======================
    void set_user_id(int e);              // Set user ID
    void set_event_id(int e);             // Set event ID
    void setrating(int r);                // Set rating
    void setcomments(string c);           // Set comments
    void set_submission_date(string s);   // Set submission date

    // =======================
    // Getters
    // =======================
    int get_user_id() const;              // Get user ID
    int get_event_id() const;             // Get event ID
    int getrating() const;                // Get rating
    string getcomments() const;           // Get comments
    string get_submission_date() const;   // Get submission date
};

#endif // FEEDBACK_H