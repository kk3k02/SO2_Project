#include "ball.h"
#include <cmath>
#include <thread>
#include <iostream>

// Constructor to initialize Ball object with given parameters
Ball::Ball(Rectangle* rectangle, float radius, float x, float y, float vx, float vy, float r, float g, float b, int num_bounces, int screen_width, int screen_height, int stick_duration) :
        rect(rectangle), radius(radius), x(x), y(y), vx(vx), vy(vy), r(r), g(g), b(b), num_bounces(0), max_bounces(num_bounces), width(screen_width), height(screen_height), isColliding(false), stick_duration(stick_duration) {
    start_time = std::chrono::steady_clock::now(); // Initialize start_time
}

// Function to draw the Ball object
void Ball::draw() const {
    glColor3f(r, g, b); // Use RGB color provided during initialization
    glBegin(GL_TRIANGLE_FAN); // Begin drawing triangle fan
    for (int i = 0; i < 360; i++) {
        float angle = 2.0f * 3.1415926f * float(i) / float(360); // Calculate angle
        float cx = x + radius * cosf(angle); // Calculate x-coordinate of circle point
        float cy = y + radius * sinf(angle); // Calculate y-coordinate of circle point
        glVertex2f(cx, cy); // Set vertex
    }
    glEnd(); // End drawing
}

// Function to move the Ball object
void Ball::move() {
    while (!shouldRemove()) {
        if (!isColliding) {
            x += vx; // Update x-coordinate
            y += vy; // Update y-coordinate
        } else {
            Rectangle re = *rect;
            std::vector<float> rectangle = re.getRect();
            x += rectangle[4]; // Continue moving with the rectangle's horizontal velocity

            auto end_time = std::chrono::steady_clock::now();
            auto time_diff = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);

            if (time_diff.count() >= stick_duration) {
                isColliding = false;
                unstick(); // Unstick the ball if stick duration is exceeded
            }
        }

        handleCollision(); // Handle collision with boundaries and rectangle
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Sleep to simulate real-time movement
    }
}

// Function to handle collisions with boundaries and the rectangle
void Ball::handleCollision() {
    Rectangle re = *rect;
    std::vector<float> rectangle = re.getRect();
    std::vector<float> ball = getBall();

    // Check if the ball is inside the rectangle
    bool isInsideRectangle = ((x >= (rectangle[0] - radius)) && (x <= (rectangle[1] + radius)) &&
                              (y >= (rectangle[2] - radius)) && (y <= (rectangle[3] + radius)));

    if (isInsideRectangle) {
        if (!isColliding) {
            isColliding = true;
            start_time = std::chrono::steady_clock::now(); // Reset start time when collision starts
        }
    } else {
        isColliding = false; // The ball is not stuck to the rectangle

        // Check for collisions with screen boundaries
        if (x <= radius && vx < 0) {
            x = radius; // Set x to minimum value to avoid entering the boundary
            vx *= -1; // Reverse x-velocity
            num_bounces++;
        }
        if (x >= (float)(width) - radius && vx > 0) {
            x = (float)(width) - radius; // Set x to maximum value to avoid entering the boundary
            vx *= -1; // Reverse x-velocity
            num_bounces++;
        }
        if (y <= radius && vy < 0) {
            y = radius; // Set y to minimum value to avoid entering the boundary
            vy *= -1; // Reverse y-velocity
            num_bounces++;
        }
        if (y >= (float)(height) - radius && vy > 0) {
            y = (float)(height) - radius; // Set y to maximum value to avoid entering the boundary
            vy *= -1; // Reverse y-velocity
            num_bounces++;
        }
    }
}

// Function to check if the ball should be removed (if it exceeded bounce limit)
bool Ball::shouldRemove() const {
    return num_bounces >= max_bounces;
}

// Function to get the ball boundaries
std::vector<float> Ball::getBall() const {
    return {x, x + 2 * radius, float(height) - y, float(height) - (y + 2 * radius)};
}

// Function to unstick the ball from the rectangle
void Ball::unstick() {
    Rectangle re = *rect;
    std::vector<float> rectangle = re.getRect(); // [xMin, xMax, yMin, yMax]

    // Declare bool variables for conditions
    bool stuckOnTop = (y - radius <= rectangle[2] && x - radius <= rectangle[1] && x + radius >= rectangle[0]);
    bool stuckOnBottom = (y + radius >= rectangle[3] && x - radius <= rectangle[1] && x + radius >= rectangle[0]);
    bool stuckOnRight = (x + radius >= rectangle[1] && y + radius <= rectangle[3] && y - radius >= rectangle[2]);
    bool stuckOnLeft = (x - radius <= rectangle[0] && y + radius <= rectangle[3] && y - radius >= rectangle[2]);

    // Adjust ball position based on where it was sticking
    if (stuckOnBottom && (stuckOnLeft || stuckOnRight)) {
        stuckOnBottom = false;
    } else if (stuckOnTop && (stuckOnLeft || stuckOnRight)) {
        stuckOnTop = false;
    }

    if (stuckOnTop) {
        vy *= -1;
        y -= 2 * radius; // Move above the rectangle
        // std::cout << "top\n"; // Debugging aid
    } else if (stuckOnBottom) {
        vy *= -1;
        y += 2 * radius; // Move below the rectangle
        // std::cout << "bottom\n"; // Debugging aid
    } else if (stuckOnRight) {
        vx *= -1;
        x += 2 * radius; // Move to the left of the rectangle
        y += vy;
        // std::cout << "right\n"; // Debugging aid
    } else if (stuckOnLeft) {
        vx *= -1;
        x -= 2 * radius; // Move to the right of the rectangle
        y += vy;
        // std::cout << "left\n"; // Debugging aid
    }

    // Ensure ball is no longer colliding
    isColliding = false;
}
