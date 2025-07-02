cmake_minimum_required(VERSION 3.27.8)
project(Main_proj)

set(CMAKE_CXX_STANDARD 20)

add_executable(
        Main_proj
        main.cpp
        factory.cpp
        virtualevent.cpp
        workshops.cpp
        webinar.cpp
        virtualevent.h
        event_dbmanager.h
        event_dbmanager.cpp
        run_prog.cpp
        run_prog.h
        webinar.h
        workshops.h
        sqlite3.h
        sqlite3.c
        users.dbmanger.cpp
        users.dbmanger.h
        conference.cpp
        user.cpp
        user.h
        Event_Manager.cpp
        Event_Manager.h
        feedback.cpp
        feedback.h
        Event_Manager.cpp
)
