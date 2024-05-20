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

    // Get the rectangle boundaries and print them
//    std::vector<float> ball = getBall();
//    std::cout << "Rectangle boundaries: "
//              << "xMin: " << ball[0] << ", "
//              << "xMax: " << ball[1] << ", "
//              << "yMin: " << ball[2] << ", "
//              << "yMax: " << ball[3] << std::endl;
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

    bool isInsideRectangle = ((x >= (rectangle[0] - radius)) && (x <= (rectangle[1] + radius)) &&
                              (y >= (rectangle[2] - radius)) && (y <= (rectangle[3] + radius)));

    if (isInsideRectangle) {
        if (!isColliding) {
            isColliding = true;
            start_time = std::chrono::steady_clock::now(); // Reset start time when collision starts
        }
    } else {
        isColliding = false;  // Piłka nie jest przyklejona do prostokąta

        // Sprawdzamy, czy piłka uderzyła w lewą krawędź ekranu
        if (x <= radius && vx < 0) {
            x = radius; // Ustawiamy x na minimalną wartość, aby uniknąć wejścia w prostokąt
            vx *= -1; // Odwracamy kierunek poruszania się piłki w osi x
            num_bounces++;
        }
        // Sprawdzamy, czy piłka uderzyła w prawą krawędź ekranu
        if (x >= (float)(width) - radius && vx > 0) {
            x = (float)(width) - radius; // Ustawiamy x na maksymalną wartość, aby uniknąć wejścia w prostokąt
            vx *= -1; // Odwracamy kierunek poruszania się piłki w osi x
            num_bounces++;
        }
        // Sprawdzamy, czy piłka uderzyła w górną krawędź ekranu
        if (y <= radius && vy < 0) {
            y = radius; // Ustawiamy y na minimalną wartość, aby uniknąć wejścia w prostokąt
            vy *= -1; // Odwracamy kierunek poruszania się piłki w osi y
            num_bounces++;
        }
        // Sprawdzamy, czy piłka uderzyła w dolną krawędź ekranu
        if (y >= (float)(height) - radius && vy > 0) {
            y = (float)(height) - radius; // Ustawiamy y na maksymalną wartość, aby uniknąć wejścia w prostokąt
            vy *= -1; // Odwracamy kierunek poruszania się piłki w osi y
            num_bounces++;
        }
    }
}


bool Ball::shouldRemove() const {
    return num_bounces >= max_bounces; // Check if the ball exceeded the bounce limit
}

std::vector<float> Ball::getBall() const {
    return {x, x + 2 * radius, float(height) - y, float (height) - (y + 2 * radius)}; // Return ball boundaries
}

void Ball::unstick() {
    Rectangle re = *rect;
    std::vector<float> rectangle = re.getRect(); // [xMin, xMax, yMin, yMax]

    // Adjust ball position based on where it was sticking
    if (y - radius <= rectangle[2] && x - radius <= rectangle[1] && x + radius >= rectangle[0]) { // Ball stuck on top
        vy *= -1;
        y -= 2 * radius; // Move above the rectangle
        std::cout << "top\n";
    } else if (y + radius >= rectangle[3] && x - radius <= rectangle[1] && x + radius >= rectangle[0]) { // Ball stuck on bottom
        vy *= -1;
        y += 2 * radius; // Move below the rectangle
        std::cout << "bottom\n";
    } else if (x + radius >= rectangle[1] && y + radius <= rectangle[3] && y - radius >= rectangle[2]) { // Ball stuck on right
        vx *= -1;
        x += 2 * radius; // Move to the left of the rectangle
        y += vy;
        std::cout << "right\n";
    } else if (x - radius <= rectangle[0] && y + radius <= rectangle[3] && y - radius >= rectangle[2]) { // Ball stuck on left
        vx *= -1;
        x -= 2 * radius; // Move to the right of the rectangle
        y += vy;
        std::cout << "left\n";
    }

    // Ensure ball is no longer colliding
    isColliding = false;
}





