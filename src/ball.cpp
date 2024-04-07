#include "ball.h"
#include <cmath>

Ball::Ball(float x, float y, float vx, float vy, float r, float g, float b, int num_bounces, int screen_width, int screen_height) :
        x(x), y(y), vx(vx), vy(vy), r(r), g(g), b(b), bounces(0), max_bounces(num_bounces), width(screen_width), height(screen_height) {}

void Ball::draw() const {
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i < 360; i++) {
        float angle = 2.0f * 3.1415926f * float(i) / float(360);
        float cx = x + 10 * cosf(angle);
        float cy = y + 10 * sinf(angle);
        glVertex2f(cx, cy);
    }
    glEnd();
}

void Ball::move() {
    x += vx;
    y += vy;
    handleCollision();
}

void Ball::handleCollision() {
    if (x <= 10 || x >= (float)(width) - 10) {
        vx *= -1;
        bounces++;
    }
    if (y <= 10 || y >= (float)(height) - 10) {
        vy *= -1;
        bounces++;
    }
}

bool Ball::shouldRemove() const {
    return bounces >= max_bounces;
}

