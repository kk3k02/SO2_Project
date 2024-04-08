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
const int NUM_BALLS = 20; // Number of balls to generate
const int NUM_BOUNCES = 6; // Number of bounces before a ball disappears

std::mutex mtx; // Mutex for synchronizing access to the balls vector

std::vector<Ball> balls; // Vector to hold the balls

Rectangle rect(0.0f, 50.0f, 2.0f, 150.0f, 80.0f); // Create a rectangle object

// Function to generate balls
void generateBalls() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distribution_x(-5.0, 5.0);
    std::uniform_real_distribution<float> distribution_y(-5.0, 5.0);
    std::uniform_real_distribution<float> colorDistribution(0.0, 1.0);

    for (int i = 0; i < NUM_BALLS; i++) {
        mtx.lock(); // Lock the mutex before accessing the balls vector
        balls.emplace_back(200, 460, distribution_x(gen), distribution_y(gen),
                           colorDistribution(gen), colorDistribution(gen), colorDistribution(gen), NUM_BOUNCES, WIDTH, HEIGHT);
        mtx.unlock(); // Unlock the mutex after modifying the balls vector

        // Random delay before generating the next ball
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 + std::uniform_int_distribution<int>(0, 2000)(gen)));
    }
}

// Function to render the scene
void renderScene(GLFWwindow* window) {
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer

    mtx.lock(); // Lock the mutex before accessing the balls vector
    for (auto it = balls.begin(); it != balls.end();) {
        it->draw(); // Draw the ball
        it->move(); // Move the ball
        if (it->shouldRemove()) {
            it = balls.erase(it);  // Remove the ball if it should be removed and get the iterator to the next element
        } else {
            ++it;  // Move the iterator only if the ball is not being removed
        }
    }
    mtx.unlock(); // Unlock the mutex after modifying the balls vector

    rect.draw(); // Draw the rectangle
    rect.move(0.0f, WIDTH); // Move the rectangle

    glfwSwapBuffers(window); // Swap the front and back buffers
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n"; // Print error message if GLFW initialization fails
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Jakub Jakubowicz - Projekt 1", nullptr, nullptr); // Create a GLFW window
    if (!window) {
        glfwTerminate(); // Terminate GLFW if window creation fails
        std::cerr << "Failed to create window\n"; // Print error message
        return -1;
    }

    glfwMakeContextCurrent(window); // Set the window context

    std::thread ballGenerator(generateBalls); // Start a thread to generate balls

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.0f, 0.5f, 0.0f, 1.0f); // Set clear color to green
        glMatrixMode(GL_PROJECTION); // Set matrix mode to projection
        glLoadIdentity(); // Load identity matrix
        glOrtho(0.0, WIDTH, HEIGHT, 0.0, 0.0, 1.0); // Set orthographic projection
        glMatrixMode(GL_MODELVIEW); // Set matrix mode to modelview
        glLoadIdentity(); // Load identity matrix

        renderScene(window); // Render the scene

        glfwPollEvents(); // Poll for events

        // Detach ballGenerator thread and exit the loop if space key is pressed
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            ballGenerator.detach(); // Detach the ballGenerator thread
            break; // Exit the loop
        }
    }

// Check if the ballGenerator thread is joinable, meaning it is not already joined or detached
    if (ballGenerator.joinable()) {
        // If the ballGenerator thread is joinable, detach it before closing program
        ballGenerator.detach();
    }

    glfwDestroyWindow(window); // Destroy the window
    glfwTerminate(); // Terminate GLFW
    return 0;
}
