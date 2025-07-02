#include"run_prog.h"
using namespace std;

// ==================== PROGRAM ENTRY POINT FUNCTION ====================
// This function initializes and runs the entire Event Management System
void VMS(){
    UserManager um; // Create an instance of UserManager

    // Prompt user to login or register, returns email of authenticated user
    string email = um.run();

    // Get the singleton instance of EventManager
    EventManager *em = EventManager::getInstance();

    // Load all events from the database into memory
    em->load_db();

    // Retrieve user data by email and set it as the current user
    user u = um.get_user(email);
    em->set_user(u);

    // Display the main menu for event operations
    em->disp_menu();

    // Cleanup: delete the EventManager instance
    delete em;
}