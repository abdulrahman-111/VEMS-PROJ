#include "users.dbmanger.h"
#include "feedback.h"
using namespace std;

// ==============================================
// Feedback Operations
// ==============================================

// Submit feedback for an event
// Returns true if successful, false if failed
bool submitFeedback( feedback &feedback) {
    if(!opendb()) {
        std::cout<< "Database error: Failed to open database connection." << std::endl;
        db=nullptr;
        return false;
    }

    sqlite3_stmt *stmt;
    string sql="INSERT INTO feedback (user_id,event_id,rating,comment,submission_date) VALUES (?,?,?,?,datetime('now'));";

    if(sqlite3_prepare_v2(db,sql.c_str(),-1,&stmt,nullptr)!=SQLITE_OK) {
        std::cout<<"Failed to prepare statement:"<<sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        db=nullptr;
        return false;
    }

    sqlite3_bind_text(stmt,4,feedback.getcomments().c_str(),-1,SQLITE_STATIC);
    sqlite3_bind_int(stmt,1,feedback.get_user_id());
    sqlite3_bind_int(stmt,2,feedback.get_event_id());
    sqlite3_bind_int(stmt,3,feedback.getrating());

    if(sqlite3_step(stmt)==SQLITE_DONE) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        db=nullptr;
        return true;
    }

    std::cout << "Database error: Failed to execute query - " << sqlite3_errmsg(db) << std::endl;
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    db=nullptr;
    return false;
}

// Get all feedback for specific event
// Returns vector of feedback structs
std::vector<feedback> getEventFeedback(event_db &event) {
    vector<feedback> feedbacks;
    feedback temp;

    if(!opendb()) {
        std::cout<< "Database error: Failed to open database connection." << std::endl;
        db=nullptr;
        return feedbacks;
    }

    sqlite3_stmt *stmt;
    string sql="SELECT * FROM feedback WHERE event_id = ? ORDER BY submission_date ASC ;";

    if(sqlite3_prepare_v2(db,sql.c_str(),-1,&stmt,nullptr)!=SQLITE_OK) {
        std::cout<<"Failed to prepare statement:"<<sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        db=nullptr;
        return feedbacks;
    }

    sqlite3_bind_int(stmt,1,event.event_id);

    while(sqlite3_step(stmt)==SQLITE_ROW) {
        temp.set_event_id(sqlite3_column_int(stmt,2));
        temp.set_user_id(sqlite3_column_int(stmt,1));
        temp.setrating(sqlite3_column_int(stmt,3));
        temp.setcomments(reinterpret_cast<const char*>(sqlite3_column_text(stmt,4)));
        temp.set_submission_date(reinterpret_cast<const char*>(sqlite3_column_text(stmt,5)));
        feedbacks.push_back(temp);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    db=nullptr;
    return feedbacks;
}
bool delete_event_feedback(int event_id) {
    if(!opendb()) {
        std::cout<< "Database error: Failed to open database connection." << std::endl;
        db=nullptr;
        return false;
    }

    sqlite3_stmt *stmt;
    string sql="DELETE FROM feedback WHERE event_id= ?;";

    if(sqlite3_prepare_v2(db,sql.c_str(),-1,&stmt,nullptr)!=SQLITE_OK) {
        std::cout<<"Failed to prepare statement:"<<sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        db=nullptr;
        return false;
    }
    sqlite3_bind_int(stmt,1,event_id);
    if(sqlite3_step(stmt)==SQLITE_DONE) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        db=nullptr;
        return true;
    }

    std::cout << "Database error: Failed to execute query - " << sqlite3_errmsg(db) << std::endl;
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    db=nullptr;
    return false;
}
// ==============================================
// Attendee Operations
// ==============================================

// Register user for an event
// Returns true if successful, false if failed
bool registerAttendee_db(user & user, event_db &event) { //NOTE:: if the capacity is full add to waiting list with its postion
    if(!opendb()) {
        std::cout<< "Database error: Failed to open database connection." << std::endl;
        db=nullptr;
        return false;
    }

    sqlite3_stmt *stmt;
    string sql="INSERT INTO event_attendee (user_id,event_id,date) VALUES (?,?,datetime('now'));";

    if(sqlite3_prepare_v2(db,sql.c_str(),-1,&stmt,nullptr)!=SQLITE_OK) {
        std::cout<<"Failed to prepare statement:"<<sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        db=nullptr;
        return false;
    }

    sqlite3_bind_int(stmt,1,user.getUserID());
    sqlite3_bind_int(stmt,2,event.event_id);

    if(sqlite3_step(stmt)==SQLITE_DONE) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        db=nullptr;
        return true;
    }

    std::cout << "Database error: Failed to execute query - " << sqlite3_errmsg(db) << std::endl;
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    db=nullptr;
    return false;
}

// Unregister user from an event
// Returns true if successful, false if failed
bool unregisterAttendee_db(int user_id, int event_id ) {
    if(!opendb()) {
        std::cout<< "Database error: Failed to open database connection." << std::endl;
        db=nullptr;
        return false;
    }

    sqlite3_stmt *stmt;
    string sql="DELETE FROM event_attendee where user_id=? AND event_id=?;";

    if(sqlite3_prepare_v2(db,sql.c_str(),-1,&stmt,nullptr)!=SQLITE_OK) {
        std::cout<<"Failed to prepare statement:"<<sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        db=nullptr;
        return false;
    }

    sqlite3_bind_int(stmt,1,user_id);
    sqlite3_bind_int(stmt,2,event_id);

    if(sqlite3_step(stmt)==SQLITE_DONE) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        db=nullptr;
        return true;
    }

    std::cout << "Database error: Failed to execute query - " << sqlite3_errmsg(db) << std::endl;
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    db=nullptr;
    return false;
}

// ==============================================
// User Account Functions
// ==============================================

// Add new user to database
// Returns true if successful, false if failed
bool add_user(user &user ) {
    if(!opendb()) {
        std::cout<< "Database error: Failed to open database connection." << std::endl;
        db=nullptr;
        return false;
    }

    sqlite3_stmt *stmt;
    string sql="INSERT INTO users (email,password,username,Name,affiliation) VALUES (?,?,?,?,?);";

    if(sqlite3_prepare_v2(db,sql.c_str(),-1,&stmt,nullptr)!=SQLITE_OK) {
        std::cout<<"Failed to prepare statement:"<<sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        db=nullptr;
        return false;
    }

    sqlite3_bind_text(stmt,1,user.getEmail().c_str(),-1,SQLITE_STATIC);
    sqlite3_bind_text(stmt,2,user.getPassword().c_str(),-1,SQLITE_STATIC);
    sqlite3_bind_text(stmt,3,user.getUsername().c_str(),-1,SQLITE_STATIC);
    sqlite3_bind_text(stmt,4,user.getName().c_str(),-1,SQLITE_STATIC);
    sqlite3_bind_text(stmt,5,user.getAffiliation().c_str(),-1,SQLITE_STATIC);

    if(sqlite3_step(stmt)==SQLITE_DONE) {
        sqlite3_finalize(stmt);
        string user_email=user.getEmail();
        int user_id=return_user_id(user_email);
        user.setuser_id(user_id);
        sqlite3_close(db);
        db=nullptr;
        return true;
    }

    std::cout << "Database error: Failed to execute query - " << sqlite3_errmsg(db) << std::endl;
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    db=nullptr;
    return false;
}

// Get user_id by email
// Returns user_id if found, 0 if not found
int return_user_id(const std::string &email) {
    int id=0;
    if(!opendb()) {
        std::cout<< "Database error: Failed to open database connection." << std::endl;
        db=nullptr;
        return id;
    }

    sqlite3_stmt *stmt;
    string sql="SELECT * FROM users WHERE email= ? ;";

    if(sqlite3_prepare_v2(db,sql.c_str(),-1,&stmt,nullptr)!=SQLITE_OK) {
        std::cout<<"Failed to prepare statement:"<<sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        db=nullptr;
        return false;
    }

    sqlite3_bind_text(stmt,1,email.c_str(),-1,SQLITE_STATIC);

    int check =sqlite3_step(stmt);
    if(check==SQLITE_ROW) {
        id=sqlite3_column_int(stmt,0);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        db=nullptr;
        return id;
    }
    else if (check == SQLITE_DONE) {
        std::cout << "Info: No user found with ID: " << email << std::endl;
    }

    std::cout << "Database error: Failed to execute query - " << sqlite3_errmsg(db) << std::endl;
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    db=nullptr;
    return id;
}

// Delete user account
// Returns true if successful, false if failed
bool delete_user(user &user) {
    if(!opendb()) {
        std::cout<< "Database error: Failed to open database connection." << std::endl;
        db=nullptr;
        return false;
    }

    sqlite3_stmt *stmt;
    string sql="DELETE FROM users WHERE user_id = ? AND email = ? ";

    if(sqlite3_prepare_v2(db,sql.c_str(),-1,&stmt,nullptr)!=SQLITE_OK) {
        std::cout<<"Failed to prepare statement:"<<sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        db=nullptr;
        return false;
    }

    sqlite3_bind_int(stmt,1,user.getUserID());
    sqlite3_bind_text(stmt,2,user.getEmail().c_str(),-1,SQLITE_STATIC);

    if(sqlite3_step(stmt)==SQLITE_DONE) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        db=nullptr;
        return true;
    }

    std::cout << "Database error: Failed to execute query - " << sqlite3_errmsg(db) << std::endl;
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    db=nullptr;
    return false;
}

// Authenticate user with email and password
// Returns true if successful, false if failed
bool authenticate_user(user &user) {
    if(!opendb()) {
        std::cout<< "Database error: Failed to open database connection." << std::endl;
        db=nullptr;
        return false;
    }

    sqlite3_stmt *stmt;
    string sql="SELECT * FROM users WHERE email=?;";

    if(sqlite3_prepare_v2(db,sql.c_str(),-1,&stmt,nullptr)!=SQLITE_OK) {
        std::cout<<"Failed to prepare statement:"<<sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        db=nullptr;
        return false;
    }

    sqlite3_bind_text(stmt,1,user.getEmail().c_str(),-1,SQLITE_STATIC);
    int check=sqlite3_step(stmt);

    if(check==SQLITE_ROW) {
        std::string pass_temp = reinterpret_cast<const char*>(sqlite3_column_text(stmt,2));
        if(user.getPassword()==pass_temp) {
            std::cout<<"pass is correct";
            user=return_user_by_email(user.getEmail());
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            db=nullptr;
            return true;
        }
        else {
            std::cout<<"pass is not correct";
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            db=nullptr;
            return false;
        }
    }
    else if(check==SQLITE_DONE){
        std::cout<<"uncorrect email";
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        db=nullptr;
        return false;
    }

    std::cout << "Database error: Failed to execute query - " << sqlite3_errmsg(db) << std::endl;
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    db=nullptr;
    return false;
}

// Get user by email
// Returns user struct with data if found, empty struct if not found
user return_user_by_email(const std::string &email) {
    string temp_email=email;
    user temp;
    if(!opendb()) {
        std::cout << "operation failed" << std::endl;
        return temp;
    }
    string sql="SELECT * FROM users WHERE email=?;";
    sqlite3_stmt *stmt;
    int check=sqlite3_prepare_v2(db,sql.c_str(),-1,&stmt,nullptr);

    if(check!=SQLITE_OK) {
        std::cout<<"Failed to prepare statement:"<<sqlite3_errmsg(db);
        sqlite3_close(db);
        db=nullptr;
        return temp;
    }
    sqlite3_bind_text(stmt,1,temp_email.c_str(),-1,SQLITE_STATIC);

    check=(sqlite3_step(stmt));
    if(check==SQLITE_ROW) {
        temp.setuser_id(sqlite3_column_int(stmt, 0));
        string temp_email= reinterpret_cast<const char*>(sqlite3_column_text(stmt,1));
        temp.setEmail(temp_email);
        string password= reinterpret_cast<const char*>(sqlite3_column_text(stmt,2));
        temp.setPassword(password);
        string username= reinterpret_cast<const char*>(sqlite3_column_text(stmt,3));
        temp.setUsername(username);
        string Name= reinterpret_cast<const char*>(sqlite3_column_text(stmt,4));
        temp.setName(Name);
        const char * text= reinterpret_cast<const char*>(sqlite3_column_text(stmt,5)); //can be null so it is required to check
        string affiliation=text ? text : "";
        temp.setAffiliation(affiliation);
    }
    else if (check == SQLITE_DONE) {
        std::cout << "Info: No user found with email: " << email << std::endl;
    }
    else {
        std::cout << "Database error: Failed to execute query - " << sqlite3_errmsg(db) << std::endl;
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    db=nullptr;
    return temp;
}
user return_user_by_id(int user_id) {
    user temp;
    if(!opendb()) {
        std::cout << "operation failed" << std::endl;
        return temp;
    }

    string  sql="SELECT * FROM users WHERE user_id=?;";
    sqlite3_stmt *stmt;
    int check=sqlite3_prepare_v2(db,sql.c_str(),-1,&stmt,nullptr);

    if(check!=SQLITE_OK) {
        std::cout<<"Failed to prepare statement:"<<sqlite3_errmsg(db);
        sqlite3_close(db);
        db=nullptr;
        return temp;
    }
    sqlite3_bind_int(stmt,1,user_id);
    check=sqlite3_step(stmt);
    if(check==SQLITE_ROW) {

        temp.setuser_id(sqlite3_column_int(stmt, 0));
        string email= reinterpret_cast<const char*>(sqlite3_column_text(stmt,1));
        temp.setEmail(email);
        string password= reinterpret_cast<const char*>(sqlite3_column_text(stmt,2));
        temp.setPassword(password);
        string username= reinterpret_cast<const char*>(sqlite3_column_text(stmt,3));
        temp.setUsername(username);
        string Name= reinterpret_cast<const char*>(sqlite3_column_text(stmt,4));
        temp.setName(Name);
        const char * text= reinterpret_cast<const char*>(sqlite3_column_text(stmt,5)); //can be null so it is required to check
        string affiliation=text ? text : "";
        temp.setAffiliation(affiliation);
    }
    else if (check == SQLITE_DONE) {
        std::cout << "Info: No user found with user_id: " << user_id << std::endl;
    }
    else {
        std::cout << "Database error: Failed to execute query - " << sqlite3_errmsg(db) << std::endl;
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    db=nullptr;
    return temp;
}
// Get all users in database
// Returns vector of all user structs
std::vector<user> get_all_users() {
    std::vector<user> users;
    if(!opendb()) {
        std::cout << "operation failed" << std::endl;
        return users;
    }

    string sql="SELECT * FROM users;";
    sqlite3_stmt *stmt;
    int check=sqlite3_prepare_v2(db,sql.c_str(),-1,&stmt,nullptr);

    if(check!=SQLITE_OK) {
        std::cout<<"Failed to prepare statement:"<<sqlite3_errmsg(db);
        sqlite3_close(db);
        db=nullptr;
        return users;
    }
    int step_result;
    while((step_result=sqlite3_step(stmt))==SQLITE_ROW) {
        user temp;
        temp.setuser_id(sqlite3_column_int(stmt, 0));
        string email= reinterpret_cast<const char*>(sqlite3_column_text(stmt,1));
        temp.setEmail(email);
        string password= reinterpret_cast<const char*>(sqlite3_column_text(stmt,2));
        temp.setPassword(password);
        string username= reinterpret_cast<const char*>(sqlite3_column_text(stmt,3));
        temp.setUsername(username);
        string Name= reinterpret_cast<const char*>(sqlite3_column_text(stmt,4));
        temp.setName(Name);
        const char * text= reinterpret_cast<const char*>(sqlite3_column_text(stmt,5)); //can be null so it is required to check
        string affiliation=text ? text : "";
        temp.setAffiliation(affiliation);

        users.push_back(temp);
    }
    if(step_result!=SQLITE_DONE)  std::cout << "Database error: Failed to execute query - " << sqlite3_errmsg(db) << std::endl;

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    db=nullptr;
    return users;
}

// Get all attendees for specific event
// Returns vector of user structs attending the event
std::vector<user> get_all_event_attendees(event_db & event) {
    std::vector<user> users;

    if(!opendb()) {
        std::cout << "operation failed" << std::endl;
        return users;
    }

    string  sql="SELECT * FROM users e JOIN event_attendee ea ON e.user_id=ea.user_id WHERE ea.event_id = ? ";
    sqlite3_stmt *stmt;
    int check=sqlite3_prepare_v2(db,sql.c_str(),-1,&stmt,nullptr);

    if(check!=SQLITE_OK) {
        std::cout<<"Failed to prepare statement:"<<sqlite3_errmsg(db);
        sqlite3_close(db);
        db=nullptr;
        return users;
    }

    sqlite3_bind_int(stmt,1,event.event_id);

    int step_result;
    while((step_result=sqlite3_step(stmt))==SQLITE_ROW) {
        user temp;
        temp.setuser_id(sqlite3_column_int(stmt, 0));
        string email= reinterpret_cast<const char*>(sqlite3_column_text(stmt,1));
        temp.setEmail(email);
        string password= reinterpret_cast<const char*>(sqlite3_column_text(stmt,2));
        temp.setPassword(password);
        string username= reinterpret_cast<const char*>(sqlite3_column_text(stmt,3));
        temp.setUsername(username);
        string Name= reinterpret_cast<const char*>(sqlite3_column_text(stmt,4));
        temp.setName(Name);
        const char * text= reinterpret_cast<const char*>(sqlite3_column_text(stmt,5)); //can be null so it is required to check
        string affiliation=text ? text : "";
        temp.setAffiliation(affiliation);

        users.push_back(temp);
    }
    if(step_result!=SQLITE_DONE)  std::cout << "Database error: Failed to execute query - " << sqlite3_errmsg(db) << std::endl;

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    db=nullptr;
    return users;
}


// Waiting list

bool add_user_to_waiting_list(const user &user,const event_db &event, int position) {
    if(!opendb()) {
        std::cout<< "Database error: Failed to open database connection." << std::endl;
        db=nullptr;
        return false;
    }

    sqlite3_stmt *stmt;
    string sql="INSERT INTO waiting_list (user_id,event_id,register_date,position) VALUES (?,?,datetime('now'),?);";

    if(sqlite3_prepare_v2(db,sql.c_str(),-1,&stmt,nullptr)!=SQLITE_OK) {
        std::cout<<"Failed to prepare statement:"<<sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        db=nullptr;
        return false;
    }

    sqlite3_bind_int(stmt,1,user.getUserID());
    sqlite3_bind_int(stmt,2,event.event_id);
    sqlite3_bind_int(stmt,3,position);

    if(sqlite3_step(stmt)==SQLITE_DONE) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        db=nullptr;
        return true;
    }

    std::cout << "Database error: Failed to execute query - " << sqlite3_errmsg(db) << std::endl;
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    db=nullptr;
    return false;
}

bool delete_user_from_waiting_list(int user_id,int event_id) {
    if(!opendb()) {
        std::cout<< "Database error: Failed to open database connection." << std::endl;
        db=nullptr;
        return false;
    }

    sqlite3_stmt *stmt;
    string sql="DELETE FROM waiting_list WHERE event_id= ? AND user_id= ?;";

    if(sqlite3_prepare_v2(db,sql.c_str(),-1,&stmt,nullptr)!=SQLITE_OK) {
        std::cout<<"Failed to prepare statement:"<<sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        db=nullptr;
        return false;
    }

    sqlite3_bind_int(stmt,2,user_id);
    sqlite3_bind_int(stmt,1,event_id);

    if(sqlite3_step(stmt)==SQLITE_DONE) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        db=nullptr;
        return true;
    }

    std::cout << "Database error: Failed to execute query - " << sqlite3_errmsg(db) << std::endl;
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    db=nullptr;
    return false;
}

bool update_user_position(user &user,event_db &event,int new_pos) {
    if(!opendb()) {
        std::cout<< "Database error: Failed to open database connection." << std::endl;
        db=nullptr;
        return false;
    }

    sqlite3_stmt *stmt;
    string sql=" UPDATE waiting_list SET position= ? WHERE event_id=? AND user_id=?;";

    if(sqlite3_prepare_v2(db,sql.c_str(),-1,&stmt,nullptr)!=SQLITE_OK) {
        std::cout<<"Failed to prepare statement:"<<sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        db=nullptr;
        return false;
    }

    sqlite3_bind_int(stmt,3,user.getUserID());
    sqlite3_bind_int(stmt,2,event.event_id);
    sqlite3_bind_int(stmt,1,new_pos);

    if(sqlite3_step(stmt)==SQLITE_DONE) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        db=nullptr;
        return true;
    }

    std::cout << "Database error: Failed to execute query - " << sqlite3_errmsg(db) << std::endl;
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    db=nullptr;
    return false;
}

std::vector<user> return_event_waiting_list(event_db &event) {
    std::vector<user> users;

    if(!opendb()) {
        std::cout << "operation failed" << std::endl;
        return users;
    }

    string  sql="SELECT * FROM users e JOIN waiting_list ea ON e.user_id=ea.user_id WHERE ea.event_id = ?; ";
    sqlite3_stmt *stmt;
    int check=sqlite3_prepare_v2(db,sql.c_str(),-1,&stmt,nullptr);

    if(check!=SQLITE_OK) {
        std::cout<<"Failed to prepare statement:"<<sqlite3_errmsg(db);
        sqlite3_close(db);
        db=nullptr;
        return users;
    }

    sqlite3_bind_int(stmt,1,event.event_id);
    int step_result;
    while((step_result=sqlite3_step(stmt))==SQLITE_ROW) {
        user temp;
        temp.setuser_id(sqlite3_column_int(stmt, 0));
        string email= reinterpret_cast<const char*>(sqlite3_column_text(stmt,1));
        temp.setEmail(email);
        string password= reinterpret_cast<const char*>(sqlite3_column_text(stmt,2));
        temp.setPassword(password);
        string username= reinterpret_cast<const char*>(sqlite3_column_text(stmt,3));
        temp.setUsername(username);
        string Name= reinterpret_cast<const char*>(sqlite3_column_text(stmt,4));
        temp.setName(Name);
        const char * text= reinterpret_cast<const char*>(sqlite3_column_text(stmt,5)); //can be null so it is required to check
        string affiliation=text ? text : "";
        temp.setAffiliation(affiliation);

        users.push_back(temp);
    }

    if(step_result!=SQLITE_DONE)  std::cout << "Database error: Failed to execute query - " << sqlite3_errmsg(db) << std::endl;
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    db=nullptr;
    return users;
}