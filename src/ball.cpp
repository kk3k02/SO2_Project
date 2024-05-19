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
    return {x - radius, x + radius, y - radius, y + radius}; // Return ball boundaries
}

void Ball::unstick() {
    Rectangle re = *rect;
    std::vector<float> rectangle = re.getRect(); // [xMin, xMax, yMin, yMax]

    float ballTop = y - radius;
    float ballBottom = y + radius;
    float ballLeft = x - radius;
    float ballRight = x + radius;

    // Check if the ball is stuck to the top of the rectangle
    if (ballBottom >= rectangle[2] && ballBottom <= rectangle[3] && ballTop < rectangle[2]) {
        y += vy; // Move downward
    }
        // Check if the ball is stuck to the bottom of the rectangle
    else if (ballTop <= rectangle[3] && ballTop >= rectangle[2] && ballBottom > rectangle[3]) {
        vy *= -1; // Reverse the movement
        y += vy; // Move upward
    }
    // Check if the ball is stuck to the right side of the rectangle
    if (ballLeft <= rectangle[1] && ballLeft >= rectangle[0] && ballRight > rectangle[1] && x < (width - radius)) {
        x += vx; // Move right
        y += vy; // Move y
    }
        // Check if the ball is stuck to the left side of the rectangle
    else if (ballRight >= rectangle[0] && ballRight <= rectangle[1] && ballLeft < rectangle[0] && x > radius) {
        vx *= -1; // Reverse the movement
        x += vx; // Move left
        y += vy; // Move y
    }

    // Adjust position to ensure it's not stuck
    x += vx;
    y += vy;
}
