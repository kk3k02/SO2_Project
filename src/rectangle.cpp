#include "rectangle.h"  // Include the header file for the Rectangle class definition
#include "ball.h"
 // Include GLFW library for window management and OpenGL context
#include <random>       // For random number generation
#include <thread>       // For threading
#include <iostream>     // For debug printing
#include <condition_variable>
#include <list>

// Global flag to control the thread loop, indicating when to stop threads
bool STOP_THREAD = false;

extern int sticking_balls;  // Declared externally, tracks number of balls sticking to the rectangle
extern std::condition_variable cv;  // Used for synchronizing ball sticking behavior
extern std::list<Ball*> ballPointers;  // List to store pointers to balls

// Constructor definition
Rectangle::Rectangle(float x, float y, float vx, float width, float height, float minX, float maxX) :
        x(x), y(y), vx(vx), width(width), height(height), minX(minX), maxX(maxX) {}

// Draws the Rectangle using OpenGL
void Rectangle::draw() const {
    glColor3f(1.0f, 1.0f, 1.0f); // Set color to white
    glBegin(GL_QUADS);  // Start drawing a quadrilateral
    glVertex2f(x, y);  // Top-left point
    glVertex2f(x + width, y);  // Top-right point
    glVertex2f(x + width, y + height);  // Bottom-right point
    glVertex2f(x, y + height);  // Bottom-left point
    glEnd();  // End drawing

    // Debugging print for checking rectangle boundaries
    // std::vector<float> rect = getRect();
    // std::cout << "Rectangle boundaries: xMin: " << rect[0]
    //           << ", xMax: " << rect[1] << ", yMin: " << rect[2]
    //           << ", yMax: " << rect[3] << std::endl;
}

// Moves the Rectangle, handling boundary collisions and changing direction/speed
void Rectangle::move() {
    std::chrono::steady_clock::time_point lastTime = std::chrono::steady_clock::now(); // Time when sticking_balls first became 2
    bool isStable = false; // Flag to indicate if the condition has been met

    while (!STOP_THREAD) {
        x += vx; // Update horizontal position

        // Boundary collision detection and handling
        if (x <= minX || x >= maxX - width) {
            vx *= -1; // Reverse direction on collision

            // Define minimum and maximum speeds based on direction
            float minSpeed = vx > 0 ? 1.0f : -7.0f;
            float maxSpeed = vx > 0 ? 7.0f : -1.0f;

            // Generate a new random speed within the specified range
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> distribution(minSpeed, maxSpeed);
            vx = distribution(gen);

            // Ensure the rectangle stays within bounds after reversing direction
            if (x <= minX) {
                x = minX + 1.0f; // Adjust to stay within the screen
            } else if (x >= maxX - width) {
                x = maxX - width - 1.0f;
            }
        }

        // Checking sticking_balls condition
        if (sticking_balls == 2) {
            if (!isStable) {
                lastTime = std::chrono::steady_clock::now(); // Record the time when it first hits 2
                isStable = true;
            } else if (std::chrono::steady_clock::now() - lastTime >= std::chrono::seconds(2)) {
                sticking_balls = 0; // Reset to 0 after 2 seconds
                isStable = false; // Reset the flag
            }
        } else {
            isStable = false; // Reset the stable flag if the condition is no longer met
        }

        // Notify waiting threads if conditions are met
        if (sticking_balls < 2) {
            cv.notify_all();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Control frame rate of movement
    }
}

// Stops the Rectangle movement by setting the global stop flag
void Rectangle::stop() {
    STOP_THREAD = true;
}

// Returns current boundaries of the rectangle
std::vector<float> Rectangle::getRect() const {
    return {x, x + width, y, y + height, vx};
}

// Returns the boundaries of the screen
std::vector<float> Rectangle::getScreen() {
    return {minX, maxX};
}
