#include "feedback.h"
#include "users.dbmanger.h"

// ==================== CONSTRUCTOR ====================
// Initializes a feedback object with user ID, event ID, rating, and comment
feedback::feedback(int userID,int eventID, int r, string& c){
    user_id= userID;
    event_id=eventID;
    rating = r;
    comments = c;
}

// ==================== DATABASE SAVE ====================
// Saves the current feedback object to the database
bool feedback:: save() {
    return submitFeedback(*this); // Calls external function to handle DB write
}

// ==================== SETTERS ====================
// Set the user ID for this feedback
void feedback:: set_user_id(int e){
    user_id= e;
}

// Set the event ID for this feedback
void feedback:: set_event_id(int e){
    event_id= e;
}

// Set the numerical rating (e.g., 1–5 stars)
void feedback:: setrating(int r){
    rating = r;
}

// Set the user's written comment
void feedback:: setcomments(string c){
    comments = c;
}

// Set the submission date (as a string, e.g., "2025-05-19")
void feedback:: set_submission_date(string s) {
    submission_date=s;
}

// ==================== GETTERS ====================
// Get the user ID who submitted the feedback
int feedback:: get_user_id() const{
    return user_id;
}

// Get the event ID this feedback is associated with
int feedback:: get_event_id() const{
    return event_id;
}

// Get the rating value
int feedback:: getrating() const{
    return rating;
}

// Get the textual comments submitted
string feedback:: getcomments() const {
    return comments;
}

// Get the date the feedback was submitted
string feedback:: get_submission_date() const {
    return submission_date;
}