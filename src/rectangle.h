#ifndef PROJEKT_1_RECTANGLE_H
#define PROJEKT_1_RECTANGLE_H

class Rectangle {
private:
    float x, y;
    float vx;
    float width, height;

public:
    Rectangle(float x, float y, float vx, float width, float height);
    void draw() const;
    void move(float minX, float maxX);
};

#endif //PROJEKT_1_RECTANGLE_H
