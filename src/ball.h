#ifndef BALL_H
#define BALL_H

#include "rectangle.h"
#include <vector>
#include <chrono>
#include <GL/glut.h>

class Ball {
private:
    Rectangle* rect; // Pointer to Rectangle object for handling collisions
    float x, y; // Position of the ball
    float vx, vy; // Velocity of the ball
    float radius; // Radius of the ball
    int num_bounces; // Number of bounces
    int max_bounces; // Maximum number of bounces allowed
    int width, height; // Screen width and height
    float r, g, b; // Color of the ball
    bool isColliding; // Flag to check if ball is in collision
    std::chrono::steady_clock::time_point start_time; // Time when the ball starts to collide
    int stick_duration; // Duration for the ball to stick to the rectangle in milliseconds

public:
    Ball(Rectangle* rectangle, float radius, float x, float y, float vx, float vy, float r, float g, float b, int num_bounces, int screen_width, int screen_height, int stick_duration);
    void draw() const; // Draw the ball
    void move(); // Move the ball
    void handleCollision(); // Handle collision with the rectangle
    void unstick(); // Unstick the ball from the rectangle
    [[nodiscard]] bool shouldRemove() const; // Check if the ball should be removed from the scene
    [[nodiscard]] std::vector<float> getBall() const; // Get ball position and radius
};

#endif // BALL_H
