#include "ball.h"
#include <cmath>

// Constructor to initialize Ball object with given parameters
Ball::Ball(float x, float y, float vx, float vy, float r, float g, float b, int num_bounces, int screen_width, int screen_height) :
        x(x), y(y), vx(vx), vy(vy), r(r), g(g), b(b), num_bounces(0), max_bounces(num_bounces), width(screen_width), height(screen_height) {}

// Function to draw the Ball object
void Ball::draw() const {
    glColor3f(r, g, b); // Set color

    glBegin(GL_TRIANGLE_FAN); // Begin drawing triangle fan
    for (int i = 0; i < 360; i++) {
        float angle = 2.0f * 3.1415926f * float(i) / float(360); // Calculate angle
        float cx = x + 10 * cosf(angle); // Calculate x-coordinate of circle point
        float cy = y + 10 * sinf(angle); // Calculate y-coordinate of circle point
        glVertex2f(cx, cy); // Set vertex
    }
    glEnd(); // End drawing
}

// Function to move the Ball object
void Ball::move() {
    x += vx; // Update x-coordinate
    y += vy; // Update y-coordinate
    handleCollision(); // Handle collisions
}

// Function to handle collisions with the boundaries of the screen
void Ball::handleCollision() {
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