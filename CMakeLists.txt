cmake_minimum_required(VERSION 3.10)
project(lemon)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

include_directories(${GTK4_INCLUDE_DIRS})
link_directories(${GTK4_LIBRARY_DIRS})
add_executable(lemon 
    main.c
    exampleapp.h
    exampleappwin.h
    exampleappwin.c
    exampleapp.c
    
)
target_link_libraries(lemon ${GTK4_LIBRARIES})