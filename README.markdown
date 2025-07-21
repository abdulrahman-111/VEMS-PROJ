# Virtual Meetings System

## Overview
The Virtual Meetings System is a C++-based application designed to simplify the organization and participation in virtual events. It offers a secure, scalable, and user-friendly platform for managing events such as workshops, webinars, and conferences. The system features a modular design with robust class hierarchies, SQLite database integration, and an intuitive interface, making it ideal for universities, clubs, or small-to-medium groups.

## Features
- **User Management**: Secure user authentication with signup/login functionality, password protection, and email-based verification.
- **Event Management**: Create, update, postpone, or cancel events with details like title, description, date, and time.
- **Event Participation**: Browse public events, register for events, and track registered events.
- **Waiting List System**: Automatically manages waiting lists for full events, ensuring fair access when slots open.
- **Feedback System**: Submit and retrieve feedback for events to enhance user experience.
- **Dynamic Search**: Search events by name, date, organizer, type, or other criteria.
- **Database Integration**: Uses SQLite for lightweight, reliable data storage with support for SQL queries and transactions.
- **Design Patterns**: Implements the Factory Design Pattern for flexible event creation and Singleton for centralized event management.

## Technologies Used
- **Programming Language**: C++
- **Database**: SQLite3
- **IDE**: CLion (recommended for setup and execution)
- **Design Patterns**: Factory, Singleton

## Setup Instructions
1. **Install CLion**:
   - Download and install [CLion](https://www.jetbrains.com/clion/) on your computer.
2. **Clone the Repository**:
   ```bash
   git clone https://github.com/your-username/virtual-meetings-system.git
   ```
3. **Open the Project**:
   - Launch CLion and open the `main.cpp` file. CLion will automatically load the project files.
4. **Build and Run**:
   - Build the project in CLion and run the program.
5. **Account Setup**:
   - If you're a new user, sign up to create an account.
   - If you have an existing account, log in with your username and password.
6. **Explore Features**:
   - Once logged in, use the intuitive interface to create, manage, or join events.

## Database Setup
- The system uses SQLite3, storing data in a single file (`VMS1.sqlite`).
- Ensure the SQLite3 library is properly linked in your project settings.
- The database handles users, events, registrations, waiting lists, and feedback.

## Usage
- **Event Organizers**: Create and manage events, including postponing or canceling them.
- **Participants**: Browse events, register, join waiting lists, and provide feedback.
- **Administrators**: Manage user accounts and ensure secure access.

## Screenshots
(Include screenshots of the application interface here, as referenced in the presentation. You can add them to the repository and link them in this section, e.g., `![Login Screen](screenshots/login.png)`)

## Contribution
This project was developed by:
- Abdulrahman Gomaa (23-101279)
- Abdelrahman Essmat (23-101348)
- Ahmed Khairy (23-101243)
- Seif Haytham (23-101282)
- Maryam Rageh (23-101297)

Contributions are welcome! Please fork the repository, create a new branch, and submit a pull request with your changes.

## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Acknowledgments
- Thanks to Dr. Shaymaa for guidance and support.
- Built with the SQLite3 library for efficient data management.