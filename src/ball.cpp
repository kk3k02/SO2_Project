#include "ball.h"
#include <cmath>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <list>
#include <iostream>

extern Rectangle rect;                  // Global reference to a Rectangle object
extern int sticking_balls;              // Counter for the number of balls currently sticking to the rectangle
extern std::condition_variable cv;      // Condition variable used for synchronization
extern std::mutex mtx;                  // Mutex for protecting shared data
extern std::list<Ball*> ballPointers;   // List to manage pointers to Ball objects

std::vector<Ball*> waiting_balls;       // Vector to manage balls waiting to stick

int Ball::next_id = 0;                  // Static member to generate unique IDs for balls

// Ball constructor initializes member variables and assigns a unique ID
Ball::Ball(float radius, float x, float y, float vx, float vy, float r, float g, float b, int num_bounces, int stick_duration)
        : radius(radius), x(x), y(y), vx(vx), vy(vy), r(r), g(g), b(b), num_bounces(0), max_bounces(num_bounces), isColliding(false), stick_duration(stick_duration) {
    start_time = std::chrono::steady_clock::now(); // Initialize start time for stick duration
    id = next_id++; // Assign unique ID
}

// Renders the ball as a circle with a triangle fan
void Ball::draw() const {
    glColor3f(r, g, b); // Set color of the ball
    glBegin(GL_TRIANGLE_FAN); // Start drawing a triangle fan
    for (int i = 0; i < 360; i++) {
        float angle = 2.0f * 3.1415926f * float(i) / 360.0f;
        float cx = x + radius * cosf(angle); // Compute x coordinate for circle point
        float cy = y + radius * sinf(angle); // Compute y coordinate for circle point
        glVertex2f(cx, cy); // Draw circle point
    }
    glEnd(); // End drawing
}

// Moves the ball, handles sticking, and collision with the rectangle or boundaries
void Ball::move() {
    while (!shouldRemove()) {
        if (!isColliding) {
            x += vx; // Move ball in x direction
            y += vy; // Move ball in y direction
        } else {
            std::vector<float> rectangle = rect.getRect();
            x += rectangle[4]; // Move with the rectangle if stuck

            auto end_time = std::chrono::steady_clock::now();
            auto time_diff = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);

            if (time_diff.count() >= stick_duration) {
                isColliding = false;
                unstick(); // Release ball if stick duration is exceeded
                // std::cout << "Sticking balls: " << sticking_balls << "\n";
            }
        }

        handleCollision(); // Manage collision with boundaries and rectangle
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Simulate real-time movement
    }
}

// Determines if ball is inside the rectangle
bool Ball::isInsideRectangle() const {
    std::vector<float> rectangle = rect.getRect();
    return (x >= (rectangle[0] - radius)) && (x <= (rectangle[1] + radius)) &&
           (y >= (rectangle[2] - radius)) && (y <= (rectangle[3] + radius));
}

// Handles ball collision with rectangle and boundary conditions
void Ball::handleCollision() {
    std::unique_lock<std::mutex> lock(mtx); // Lock for synchronized access

    if (isInsideRectangle()) {
        if (!isColliding) {
            Ball* current_ball = nullptr;
            for (Ball* ball : ballPointers) {
                if (ball->getId() == id) {
                    current_ball = ball;
                    break;
                }
            }

            if (current_ball != nullptr) {
                waiting_balls.emplace_back(current_ball);
            }

            cv.wait(lock, [this, current_ball] { // Wait until it's safe to stick
                return (sticking_balls < 2) &&
                       (current_ball == waiting_balls.front()) &&
                       isInsideRectangle();
            });
            waiting_balls.erase(waiting_balls.begin());

            isColliding = true;
            sticking_balls++;
            std::cout << "Ball stuck: " << id << ", sticking balls: " << sticking_balls << "\n";
            start_time = std::chrono::steady_clock::now();
        }
    } else {
        isColliding = false;
    }

    boundaryCollisionHandling(); // Handle collision with screen boundaries
}

// Handles boundary collisions and updates position and velocity accordingly
void Ball::boundaryCollisionHandling() {
    if (x <= radius && vx < 0) {
        x = radius; vx *= -1; num_bounces++;
    }
    if (x >= (float)(WIDTH) - radius && vx > 0) {
        x = (float)(WIDTH) - radius; vx *= -1; num_bounces++;
    }
    if (y <= radius && vy < 0) {
        y = radius; vy *= -1; num_bounces++;
    }
    if (y >= (float)(HEIGHT) - radius && vy > 0) {
        y = (float)(HEIGHT) - radius; vy *= -1; num_bounces++;
    }
}

// Checks if the ball should be removed based on bounce count
bool Ball::shouldRemove() const {
    return num_bounces >= max_bounces;
}

// Unsticks the ball from the rectangle, adjusting its position and ensuring it moves away to avoid immediate re-collision
void Ball::unstick() {
    std::vector<float> rectangle = rect.getRect();
    adjustBallPosition((y - radius <= rectangle[2]) && (x - radius <= rectangle[1]) && (x + radius >= rectangle[0]),
                       (y + radius >= rectangle[3]) && (x - radius <= rectangle[1]) && (x + radius >= rectangle[0]),
                       (x + radius >= rectangle[1]) && (y - radius >= rectangle[2]) && (y + radius <= rectangle[3]),
                       (x - radius <= rectangle[0]) && (y - radius >= rectangle[2]) && (y + radius <= rectangle[3]));

    isColliding = false; // Mark as not colliding
    if (sticking_balls > 0) sticking_balls--;
    std::cout << "Ball unstuck: " << id << ", sticking balls: " << sticking_balls << "\n";
}

// Adjusts ball's position to avoid overlap with rectangle and ensures it is moved further away to avoid immediate resticking
void Ball::adjustBallPosition(bool top, bool bottom, bool right, bool left) {
    float safeDistance = 5.0 * radius;  // Define a safe distance to move the ball away from the rectangle

    if (bottom && (left || right)) {
        bottom = false; // Adjust bottom position
    } else if (top && (left || right)) {
        top = false; // Adjust top position
    }

    if (top) {
        vy *= -1;
        y -= 2 * radius + safeDistance; // Move further away after unsticking
    } else if (bottom) {
        vy *= -1;
        y += 2 * radius + safeDistance; // Move further away after unsticking
    } else if (right) {
        vx *= -1;
        x += 2 * radius + safeDistance; // Move further away after unsticking
        y += vy; // Maintain vertical movement
    } else if (left) {
        vx *= -1;
        x -= 2 * radius + safeDistance; // Move further away after unsticking
        y += vy; // Maintain vertical movement
    }
}


// Retrieves the unique ID of the ball
int Ball::getId() const {
    return id;
}
