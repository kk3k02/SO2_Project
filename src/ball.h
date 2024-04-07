#ifndef PROJEKT_1_BALL_H
#define PROJEKT_1_BALL_H

#include <GLFW/glfw3.h>

class Ball {
private:
    float x, y;
    float vx, vy;
    int bounces;
    int max_bounces;
    int width, height;
    float r, g, b;

public:
    Ball(float x, float y, float vx, float vy, float r, float g, float b, int num_bounces, int screen_width, int screen_height);
    void draw() const;
    void move();
    void handleCollision();
    [[nodiscard]] bool shouldRemove() const;
};

#endif //PROJEKT_1_BALL_H
