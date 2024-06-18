#ifndef BALL_H
#define BALL_H

#include "rectangle.h" // Include the Rectangle class for collision handling
#include <vector>
#include <chrono>
#include <GL/glut.h> // Include OpenGL Utility Toolkit for drawing graphical elements

// Declare external global constants for screen dimensions
extern const int WIDTH, HEIGHT;

class Ball {
private:
    int id; // Unique identifier for each ball
    float x, y; // X and Y coordinates of the ball
    float vx, vy; // Horizontal and vertical velocity of the ball
    float radius; // Radius of the ball
    int num_bounces; // Counter for how many times the ball has bounced
    int max_bounces; // Maximum allowed bounces before the ball is removed
    float r, g, b; // RGB color values of the ball
    bool isColliding; // Boolean flag to check if the ball is currently colliding with the rectangle
    std::chrono::steady_clock::time_point start_time; // Time point marking the start of a collision
    int stick_duration; // Duration in seconds the ball should stick to the rectangle when colliding

    void boundaryCollisionHandling(); // Handles ball collisions with the screen boundaries
    void adjustBallPosition(bool top, bool bottom, bool right, bool left); // Adjusts the ball's position when unsticking from the rectangle

public:
    // Constructor initializes the ball with specified parameters
    Ball(float radius, float x, float y, float vx, float vy, float r, float g, float b, int num_bounces, int stick_duration);

    void draw() const; // Renders the ball using OpenGL
    void move(); // Updates the ball's position based on velocity and collision status
    void handleCollision(); // Handles collisions with the rectangle and the boundaries
    void unstick(); // Releases the ball from being stuck to the rectangle
    [[nodiscard]] bool shouldRemove() const; // Checks if the ball has exceeded its bounce limit and should be removed
    static int next_id; // Static member to generate a unique ID for each ball
    [[nodiscard]] int getId() const; // Returns the unique ID of the ball
    [[nodiscard]] bool isInsideRectangle() const; // Checks if the ball is inside the collision rectangle

};

#endif // BALL_H
