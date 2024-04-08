#ifndef PROJEKT_1_RECTANGLE_H
#define PROJEKT_1_RECTANGLE_H

class Rectangle {
private:
    float x, y; // Position of the rectangle
    float vx; // Velocity of the rectangle
    float width, height; // Width and height of the rectangle

public:
    Rectangle(float x, float y, float vx, float width, float height);

    // Function to draw the Rectangle object
    void draw() const;

    // Function to move the Rectangle object
    void move(float minX, float maxX);
};

#endif
