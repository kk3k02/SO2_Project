#include "ball.h"
#include <cmath>
#include <thread>
#include <iostream>

// Constructor to initialize Ball object with given parameters
Ball::Ball(Rectangle* rectangle, float radius, float x, float y, float vx, float vy, float r, float g, float b, int num_bounces, int screen_width, int screen_height, int stick_duration) :
        rect(rectangle), radius(radius), x(x), y(y), vx(vx), vy(vy), r(r), g(g), b(b), num_bounces(0), max_bounces(num_bounces), width(screen_width), height(screen_height), isColliding(false), stick_duration(stick_duration) {
    start_time = std::chrono::steady_clock::now(); // Initialize start_time
}

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
                unstick();
            }
        }

        handleCollision();
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Sleep to simulate real-time movement
    }
}

void Ball::handleCollision() {
    Rectangle re = *rect;
    std::vector<float> rectangle = re.getRect();
    std::vector<float> ball = getBall();

    if ((ball[1] >= rectangle[0]) && (ball[0] <= rectangle[1]) &&
        (ball[3] >= rectangle[2]) && (ball[2] <= rectangle[3])) {
        if (!isColliding) {
            isColliding = true;
            start_time = std::chrono::steady_clock::now(); // Reset start time when collision starts
        }
    } else {
        isColliding = false;  // Ensure ball can stop sticking if it's no longer within rectangle

        if (x <= 10 || x >= (float)(width) - 10) {
            vx *= -1; // Reverse horizontal velocity
            num_bounces++;
        }
        if (y <= 10 || y >= (float)(height) - 10) {
            vy *= -1; // Reverse vertical velocity
            num_bounces++;
        }
    }
}

bool Ball::shouldRemove() const {
    return num_bounces >= max_bounces; // Check if the ball exceeded the bounce limit
}

std::vector<float> Ball::getBall() const {
    return {x, x + 2*radius, y, y + 2*radius}; // Return ball boundaries
}

void Ball::unstick() {
    Rectangle re = *rect;
    std::vector<float> rectangle = re.getRect(); // [xMin, xMax, yMin, yMax]
    std::vector<float> ball = getBall();

    // Adjust ball position based on where it was sticking
    if (ball[2] <= rectangle[3]) { // Ball stuck on top
        y += 2 * radius; // Move above the rectangle
        vy *= -1;
    } else if (ball[3] >= rectangle[2]) { // Ball stuck on bottom
        y += 2 * radius; // Move below the rectangle
        vy *= -1;
    } else if (ball[1] > rectangle[1] && ball[0] < rectangle[1]) { // Ball stuck on right
        x = rectangle[1]; // Move to the right of the rectangle
        vx = -std::abs(vx); // Ensure moving left
    } else if (ball[0] < rectangle[0] && ball[1] > rectangle[0]) { // Ball stuck on left
        x = rectangle[0] - 2 * radius; // Move to the left of the rectangle
        vx = std::abs(vx); // Ensure moving right
    }

    // Update ball position to ensure it is clearly outside the rectangle bounds
    x += vx;
    y += vy;

    // Ensure ball is no longer colliding
    isColliding = false;
}

