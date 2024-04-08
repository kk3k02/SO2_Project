#include "rectangle.h" // Include the header file for the Rectangle class
#include <GLFW/glfw3.h> // Include GLFW library for OpenGL
#include <random> // Include the <random> header for random number generation

Rectangle::Rectangle(float x, float y, float vx, float width, float height) :
        x(x), y(y), vx(vx), width(width), height(height) {}

// Function to draw the Rectangle object
void Rectangle::draw() const {
    glColor3f(1.0f, 1.0f, 1.0f); // Set color to white
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}

// Function to move the Rectangle object
void Rectangle::move(float minX, float maxX) {
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
}
