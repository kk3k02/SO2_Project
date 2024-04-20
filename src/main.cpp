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
const int NUM_BOUNCES = 6; // Number of bounces before a ball disappears

std::mutex mtx; // Mutex for synchronizing access to the balls vector

std::vector<Ball> balls; // Vector to hold Ball objects
Rectangle rect(0.0f, 50.0f, 2.0f, 150.0f, 80.0f); // Create a rectangle object

bool CLOSE_WINDOW = false; // Flag to indicate whether the window should close

// Function to generate a single ball
void generateBall() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distribution_x(-5.0, 5.0);
    std::uniform_real_distribution<float> distribution_y(-5.0, 5.0);
    std::uniform_real_distribution<float> colorDistribution(0.0, 1.0);

    // Create a new ball
    Ball ball(200, 460, distribution_x(gen), distribution_y(gen),
              colorDistribution(gen), colorDistribution(gen), colorDistribution(gen), NUM_BOUNCES, WIDTH, HEIGHT);

    // Add the ball to the vector
    mtx.lock(); // Lock the mutex before accessing the balls vector
    balls.push_back(ball);
    mtx.unlock(); // Unlock the mutex after modifying the balls vector

    // Run the ball simulation until it should be removed
    while (!ball.shouldRemove()) {
        ball.move(); // Move the ball
    }
}

// Function to generate and move the rectangle
void generateRectangle() {
    while (!CLOSE_WINDOW) {
        mtx.lock(); // Lock the mutex before accessing the rectangle object
        rect.move(0.0f, WIDTH); // Move the rectangle
        mtx.unlock(); // Unlock the mutex after modifying the rectangle object
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Adjust sleep duration as needed
    }
}

// Function to create threads for generating balls
void generateBalls() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(500, 1500); // Random delay from 500 ms to 1500 ms

    while (!CLOSE_WINDOW) {
        std::thread t(generateBall); // Create a new thread for each ball
        t.detach(); // Detach the thread

        int delay = distribution(gen); // Random delay
        std::this_thread::sleep_for(std::chrono::milliseconds(delay)); // Sleep with random delay
    }
}

// Function to render the scene
void renderScene(GLFWwindow* window) {
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer

    // Draw and move all balls
    for (auto& ball : balls) {
        ball.draw();
        ball.move();
    }

    mtx.lock(); // Lock the mutex before accessing the rectangle object
    rect.draw(); // Draw the rectangle
    mtx.unlock(); // Unlock the mutex after modifying the rectangle object

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
    std::thread rectangleGenerator(generateRectangle); // Start a thread to generate the rectangle

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

        // Join ballGenerator thread and rectangleGenerator thread and exit the loop if space key is pressed
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            CLOSE_WINDOW = true; // Set the flag to close the window
            ballGenerator.join(); // Join the ballGenerator thread
            rectangleGenerator.join(); // Join the rectangleGenerator thread
            break; // Exit the loop
        }
    }

    glfwDestroyWindow(window); // Destroy the window
    glfwTerminate(); // Terminate GLFW
    return 0;
}
