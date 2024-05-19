#ifndef PROJEKT_1_RECTANGLE_H
#define PROJEKT_1_RECTANGLE_H

#include <vector> // Include the <vector> header for std::vector

class Rectangle {
private:
    float x, y; // Position of the rectangle
    float vx; // Velocity of the rectangle
    float width, height; // Width and height of the rectangle
    float minX, maxX; // Minimum and maximum x-coordinates

public:
    Rectangle(float x, float y, float vx, float width, float height, float minX, float maxX);

    // Function for getting area of the Rectangle object

    // Function to draw the Rectangle object
    void draw() const;

    // Function to move the Rectangle object
    void move();

    // Static function to stop the Rectangle movement thread
    static void stop();

    // Function to get the rectangle boundaries
    std::vector<float> getRect() const;
};

#endif //PROJEKT_1_RECTANGLE_H
