#include <iostream>
#include <thread>
#include <vector>
#include <random>
#include <chrono>
#include <mutex>
#include <GLFW/glfw3.h>
#include "ball.h"
#include "rectangle.h"

const int WIDTH = 800;
const int HEIGHT = 480;
const int NUM_BALLS = 5;
const int NUM_BOUNCES = 6;

std::mutex mtx;

std::vector<Ball> balls;
Rectangle rect(0.0f, 50.0f, 2.0f, 150.0f, 80.0f);

[[noreturn]] void generateBalls() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distribution(1.0, 5.0);
    std::uniform_real_distribution<float> colorDistribution(0.0, 1.0);

    for (int i = 0; i < NUM_BALLS; i++) {
        mtx.lock();
        balls.emplace_back(20, 20, distribution(gen), distribution(gen),
                           colorDistribution(gen), colorDistribution(gen), colorDistribution(gen), NUM_BOUNCES, WIDTH, HEIGHT);
        mtx.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 + rand() % 2000));
    }
}

void renderScene(GLFWwindow* window) {
    glClear(GL_COLOR_BUFFER_BIT);

    mtx.lock();
    for (auto it = balls.begin(); it != balls.end(); ++it) {
        it->draw();
        it->move();
        if (it->shouldRemove()) {
            it = balls.erase(it);
        }
    }
    mtx.unlock();

    rect.draw(); // Rysowanie prostokąta
    rect.move(0.0f, WIDTH - 100.0f); // Przesuwanie prostokąta

    glfwSwapBuffers(window);
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Bouncing Balls", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        std::cerr << "Failed to create window\n";
        return -1;
    }

    glfwMakeContextCurrent(window);

    std::thread ballGenerator(generateBalls);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, WIDTH, HEIGHT, 0.0, 0.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        renderScene(window);

        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            break;
        }
    }

    ballGenerator.join();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
