#include "rectangle.h"
#include <GLFW/glfw3.h>
#include <random>

Rectangle::Rectangle(float x, float y, float vx, float width, float height) :
        x(x), y(y), vx(vx), width(width), height(height) {}

void Rectangle::draw() const {
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}

void Rectangle::move(float minX, float maxX) {
    x += vx;
    if (x <= minX || x >= maxX - width) {
        vx *= -1;
        // Losowanie nowej prędkości w zakresie od -5.0 do 5.0
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> distribution(-5.0f, 5.0f);
        vx = distribution(gen);
    }
}

