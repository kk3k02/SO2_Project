#include "ball.h"
#include <cmath>
#include <thread>
#include <iostream>
#include <vector>

// Constructor to initialize Ball object with given parameters
Ball::Ball(float radius, float x, float y, float vx, float vy, float r, float g, float b, int num_bounces, int screen_width, int screen_height) :
        radius(radius), x(x), y(y), vx(vx), vy(vy), r(r), g(g), b(b), num_bounces(0), max_bounces(num_bounces), width(screen_width), height(screen_height) {}

// Function to draw the Ball object
void Ball::draw() const {
    glColor3f(r, g, b); // Set color

    glBegin(GL_TRIANGLE_FAN); // Begin drawing triangle fan
    for (int i = 0; i < 360; i++) {
        float angle = 2.0f * 3.1415926f * float(i) / float(360); // Calculate angle
        float cx = x + radius * cosf(angle); // Calculate x-coordinate of circle point
        float cy = y + radius * sinf(angle); // Calculate y-coordinate of circle point
        glVertex2f(cx, cy); // Set vertex
    }
    glEnd(); // End drawing

    // Get the rectangle boundaries and print them
    std::vector<float> ball = getBall();
    std::cout << "Ball boundaries: "
              << "xMin: " << ball[0] << ", "
              << "xMax: " << ball[1] << ", "
              << "yMin: " << ball[2] << ", "
              << "yMax: " << ball[3] << std::endl;
}

// Function to move the Ball object
void Ball::move() {
    while (!shouldRemove()) {
        x += vx; // Update x-coordinate
        y += vy; // Update y-coordinate
        handleCollision(); // Handle collisions
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Adjust sleep duration as needed
    }
}

// Function to handle collisions with the boundaries of the screen
void Ball::handleCollision() {
    // TODO Uzależnij wykrywanie handleCollision od czasu i przesunięcia a nie współrzędnych
    if (x <= 10 || x >= (float)(width) - 10) { // Check collision with left or right boundary
        vx *= -1; // Reverse horizontal velocity
        num_bounces++; // Increment bounce count
    }
    if (y <= 10 || y >= (float)(height) - 10) { // Check collision with top or bottom boundary
        vy *= -1; // Reverse vertical velocity
        num_bounces++; // Increment bounce count
    }
}

// Function to check if the Ball object should be removed
bool Ball::shouldRemove() const {
    return num_bounces >= max_bounces; // Return true if bounce count exceeds maximum num_bounces allowed
}

// Function to get the ball boundaries
std::vector<float> Ball::getBall() const {
    return {x, x + radius, y, y + radius};
}