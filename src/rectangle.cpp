#include "rectangle.h" // Include the header file for the Rectangle class
#include <GLFW/glfw3.h> // Include GLFW library for OpenGL
#include <random> // Include the <random> header for random number generation
#include <thread> // Include the <thread> header for multithreading
#include <iostream> // Include <iostream> for debug printing

// Global flag to control the thread loop
bool STOP_THREAD = false;

// Constructor definition
Rectangle::Rectangle(float x, float y, float vx, float width, float height, float minX, float maxX) :
        x(x), y(y), vx(vx), width(width), height(height), minX(minX), maxX(maxX) {}

// Function to draw the Rectangle object
void Rectangle::draw() const {
    glColor3f(1.0f, 1.0f, 1.0f); // Set color to white
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();

    // Uncomment for debugging
    // std::vector<float> rect = getRect();
    // std::cout << "Rectangle boundaries: "
    //           << "xMin: " << rect[0] << ", "
    //           << "xMax: " << rect[1] << ", "
    //           << "yMin: " << rect[2] << ", "
    //           << "yMax: " << rect[3] << std::endl;
}

// Function to move the Rectangle object
void Rectangle::move() {
    while (!STOP_THREAD) {
        x += vx; // Update x-coordinate

        if (x <= minX || x >= maxX - width) { // Check collision with left or right boundary
            vx *= -1; // Reverse direction of movement

            float minSpeed = vx > 0 ? 1.0f : -7.0f; // Minimum speed based on direction
            float maxSpeed = vx > 0 ? 7.0f : -1.0f; // Maximum speed based on direction

            // Generate a random speed within the specified range
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> distribution(minSpeed, maxSpeed);
            vx = distribution(gen);

            // Adjust position if Rectangle hits the boundaries
            if (x <= minX) {
                x = minX + 1.0f; // Move it slightly inside the window
            } else if (x >= maxX - width) {
                x = maxX - width - 1.0f; // Move it slightly inside the window
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Adjust sleep duration as needed
    }
}

// Function to stop the Rectangle movement thread
void Rectangle::stop() {
    STOP_THREAD = true; // Set the global flag to stop the thread
}

// Function to get the rectangle boundaries
std::vector<float> Rectangle::getRect() const {
    return {x, x + width, y, y + height, vx};
}

// Function to get the screen boundaries
std::vector<float> Rectangle::getScreen() {
    return {minX, maxX};
}