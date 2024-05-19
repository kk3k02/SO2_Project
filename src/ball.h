#ifndef PROJEKT_1_BALL_H
#define PROJEKT_1_BALL_H

#include <GLFW/glfw3.h>
#include <vector>

class Ball {
private:
    float x, y; // Position of the ball
    float vx, vy; // Velocity of the ball
    float radius; // Radius of the ball
    int num_bounces; // Number of bounces
    int max_bounces; // Maximum number of bounces allowed
    int width, height; // Screen width and height
    float r, g, b; // Color of the ball

public:
    Ball(float radius, float x, float y, float vx, float vy, float r, float g, float b, int num_bounces, int screen_width, int screen_height);

    // Function to draw the Ball object
    void draw() const;

    // Function to move the Ball object
    void move();

    // Function to handle collisions with the boundaries of the screen
    void handleCollision();

    // Function to check if the Ball object should be removed
    [[nodiscard]] bool shouldRemove() const;

    // Function to get the rectangle boundaries
    [[nodiscard]] std::vector<float> getBall() const;

};

#endif