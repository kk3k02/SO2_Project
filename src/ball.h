#ifndef BALL_H
#define BALL_H

#include "rectangle.h" // Include the definition of Rectangle, used for collision detection
#include <vector>
#include <chrono>
#include <GL/glut.h> // Include OpenGL Utility Toolkit for drawing

extern const int WIDTH, HEIGHT; // Define external variables for screen width and height

class Ball {
private:
    int id;
    float x, y; // Position of the ball
    float vx, vy; // Velocity of the ball
    float radius; // Radius of the ball
    int num_bounces; // Number of bounces the ball has undergone
    int max_bounces; // Maximum number of bounces allowed before removal
    float r, g, b; // RGB color values of the ball
    bool isColliding; // Flag to indicate if the ball is currently colliding
    std::chrono::steady_clock::time_point start_time; // Time when the ball starts to collide
    int stick_duration; // Duration in seconds for the ball to stick to the rectangle

    bool isInsideRectangle() const;
    void boundaryCollisionHandling(); // Internal helper method for boundary collision handling
    void adjustBallPosition(bool top, bool bottom, bool right, bool left); // Method to adjust the ball's position when unsticking

public:
    // Constructor for initializing the ball with specific parameters
    Ball(float radius, float x, float y, float vx, float vy, float r, float g, float b, int num_bounces, int stick_duration);

    void draw() const; // Method to draw the ball using OpenGL commands
    void move(); // Method to update the position of the ball based on its velocity
    void handleCollision(); // Method to handle collision with the rectangle and screen boundaries
    void unstick(); // Method to unstick the ball from the rectangle
    [[nodiscard]] bool shouldRemove() const; // Method to determine if the ball should be removed from the game or scene
    static int next_id;
    [[nodiscard]] int getId() const;
};

#endif // BALL_H
