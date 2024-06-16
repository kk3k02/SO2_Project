#include "ball.h"
#include <cmath>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <iostream>
#include <list>

extern Rectangle rect;
extern int sticking_balls;
extern std::condition_variable cv;
extern std::mutex mtx;  // Mutex to be declared globally
extern std::list<Ball*> ballPointers; // List to hold pointers to Ball objects

std::vector<Ball*> waiting_balls;

int Ball::next_id = 0;

// Constructor to initialize Ball object with given parameters
Ball::Ball(float radius, float x, float y, float vx, float vy, float r, float g, float b, int num_bounces, int stick_duration)
        : radius(radius), x(x), y(y), vx(vx), vy(vy), r(r), g(g), b(b), num_bounces(0), max_bounces(num_bounces), isColliding(false), stick_duration(stick_duration) {
    start_time = std::chrono::steady_clock::now(); // Initialize start_time
    id = next_id++; // Assign unique id to each ball
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
            std::vector<float> rectangle = rect.getRect();
            x += rectangle[4]; // Continue moving with the rectangle's horizontal velocity

            auto end_time = std::chrono::steady_clock::now();
            auto time_diff = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);

            if (time_diff.count() >= stick_duration) {
                isColliding = false;
                unstick(); // Unstick the ball if stick duration is exceeded
            }
        }

        handleCollision(); // Handle collision with boundaries and rectangle
        cv.notify_all(); // Notify all waiting threads when a ball is unstuck or the condition changes
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Sleep to simulate real-time movement
    }
}

bool Ball::isInsideRectangle() const {
    std::vector<float> rectangle = rect.getRect();

    return (x >= (rectangle[0] - radius)) && (x <= (rectangle[1] + radius)) &&
           (y >= (rectangle[2] - radius)) && (y <= (rectangle[3] + radius));
}

// Function to handle collisions with boundaries and the rectangle
void Ball::handleCollision() {
    std::unique_lock<std::mutex> lock(mtx);  // Lock the mutex with unique_lock for condition_variable

    if (isInsideRectangle()) {
        if (!isColliding) {
            Ball* current_ball = nullptr; // Initialize current_ball to nullptr
            for (Ball* ball : ballPointers) {
                if (ball->getId() == id) {
                    current_ball = ball;
                    break;
                }
            }

            if (current_ball != nullptr) {
                waiting_balls.emplace_back(current_ball);
            }

            cv.wait(lock, [] { return (sticking_balls < 2); });
            if (!waiting_balls.empty()) {
                cv.wait(lock, [this, current_ball] { return (current_ball == waiting_balls.front());});
                cv.wait(lock, [this]{return (isInsideRectangle());});

                waiting_balls.erase(waiting_balls.begin());
            }

            isColliding = true;
            sticking_balls++;
            start_time = std::chrono::steady_clock::now(); // Reset start time when collision starts
        }
    } else {
        isColliding = false;
    }

    // Collision handling with screen boundaries
    boundaryCollisionHandling();
}

void Ball::boundaryCollisionHandling() {
    if (x <= radius && vx < 0) {
        x = radius;
        vx *= -1;
        num_bounces++;
    }
    if (x >= (float)(WIDTH) - radius && vx > 0) {
        x = (float)(WIDTH) - radius;
        vx *= -1;
        num_bounces++;
    }
    if (y <= radius && vy < 0) {
        y = radius;
        vy *= -1;
        num_bounces++;
    }
    if (y >= (float)(HEIGHT) - radius && vy > 0) {
        y = (float)(HEIGHT) - radius;
        vy *= -1;
        num_bounces++;
    }
}

// Function to check if the ball should be removed (if it exceeded bounce limit)
bool Ball::shouldRemove() const {
    return num_bounces >= max_bounces;
}

// Function to unstick the ball from the rectangle
void Ball::unstick() {
    std::vector<float> rectangle = rect.getRect(); // [xMin, xMax, yMin, yMax]
    adjustBallPosition((y - radius <= rectangle[2]) && (x - radius <= rectangle[1]) && (x + radius >= rectangle[0]),
                       (y + radius >= rectangle[3]) && (x - radius <= rectangle[1]) && (x + radius >= rectangle[0]),
                       (x + radius >= rectangle[1]) && (y - radius >= rectangle[2]) && (y + radius <= rectangle[3]),
                       (x - radius <= rectangle[0]) && (y - radius >= rectangle[2]) && (y + radius <= rectangle[3]));

    isColliding = false; // Ensure ball is no longer colliding
    sticking_balls--;
}

void Ball::adjustBallPosition(bool top, bool bottom, bool right, bool left) {
    if (bottom && (left || right)) {
        bottom = false;
    } else if (top && (left || right)) {
        top = false;
    }

    if (top) {
        vy *= -1;
        y -= 2 * radius;
    } else if (bottom) {
        vy *= -1;
        y += 2 * radius;
    } else if (right) {
        vx *= -1;
        x += 2 * radius;
        y += vy;
    } else if (left) {
        vx *= -1;
        x -= 2 * radius;
        y += vy;
    }
}

int Ball::getId() const {
    return id;
}
