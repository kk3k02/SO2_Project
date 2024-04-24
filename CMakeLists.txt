cmake_minimum_required(VERSION 3.27)
project(Projekt_1)

set(CMAKE_CXX_STANDARD 17)

add_executable(Projekt_1
        src/main.cpp
        src/ball.cpp
        src/ball.h
        src/rectangle.cpp
        src/rectangle.h
)

target_link_libraries(Projekt_1 PRIVATE GL GLU glut GLEW glfw X11 Xxf86vm Xrandr pthread Xi dl Xinerama Xcursor)