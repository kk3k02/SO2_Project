#include "rectangle.h" // Include the header file for the Rectangle class
#include <GLFW/glfw3.h> // Include GLFW library for OpenGL
#include <random> // Include the <random> header for random number generation
#include <thread>
#include <iostream>

bool STOP_THREAD = false; // Global flag to control the thread loop

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

//    // Get the rectangle boundaries and print them
//    std::vector<float> rect = getRect();
//    std::cout << "Rectangle boundaries: "
//              << "xMin: " << rect[0] << ", "
//              << "xMax: " << rect[1] << ", "
//              << "yMin: " << rect[2] << ", "
//              << "yMax: " << rect[3] << std::endl;

}

// Function to move the Rectangle object
void Rectangle::move() {
    while (!STOP_THREAD) {
        x += vx; // Update x-coordinate

        if (x <= minX || x >= maxX - width) { // Check collision with left or right boundary

            vx *= -1; // Reverse direction of movement

            float minSpeed, maxSpeed;

            if (vx > 0) { // If moving to the right
                minSpeed = 1.0f;
                maxSpeed = 7.0f;
            } else { // If moving to the left
                minSpeed = -7.0f;
                maxSpeed = -1.0f;
            }

            // Generate a random speed within the specified range
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> distribution(minSpeed, maxSpeed);
            vx = distribution(gen);

            if (x <= minX) { // If the Rectangle hits the left boundary
                x = minX + 1.0f; // Move it slightly inside the window
            } else if (x >= maxX - width) { // If the Rectangle hits the right boundary
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

std::vector<float> Rectangle::getScreen() {
    return {minX, maxX};
}
