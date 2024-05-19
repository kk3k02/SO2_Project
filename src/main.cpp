#include <iostream>
#include <thread>
#include <list> // Include list header
#include <random>
#include <chrono>
#include <mutex>
#include <GLFW/glfw3.h>
#include <algorithm>
#include "ball.h"
#include "rectangle.h"

const int WIDTH = 800;
const int HEIGHT = 480;
const int NUM_BOUNCES = 16; // Number of bounces before a ball disappears

std::mutex mtx; // Mutex for synchronizing access to the balls list

std::list<Ball*> ballPointers; // List to hold pointers to Ball objects
std::vector<std::thread> threads;
Rectangle rect(0.0f, 50.0f, 2.0f, 150.0f, 80.0f, 0.0f, WIDTH); // Create a rectangle object
float radius = 10; // Radius of the ball
int stick_duration = 5;

bool CLOSE_WINDOW = false; // Flag to indicate whether the window should close

// Function to generate a single ball
void generateBall() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distribution_x(-5.0, 5.0);
    std::uniform_real_distribution<float> distribution_y(-5.0, 5.0);
    std::uniform_real_distribution<float> colorDistribution(0.0, 1.0);

    // Create a new ball
    Ball* ball = new Ball(&rect, radius, 200, 460, distribution_x(gen), distribution_y(gen),
                          colorDistribution(gen), colorDistribution(gen), colorDistribution(gen), NUM_BOUNCES, WIDTH, HEIGHT, stick_duration);

    mtx.lock(); // Lock the mutex before accessing the balls list
    ballPointers.push_back(ball); // Add the ball pointer to the list
    mtx.unlock(); // Unlock the mutex after modifying the balls list

    ball->move();

    // Remove the specific ball pointer from the list
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Wait for some time before removing the ball
    mtx.lock();
    ballPointers.remove(ball);
    mtx.unlock();

    delete ball; // Delete the ball object to avoid memory leaks
}

// Function to create threads for generating balls
void generateBalls() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(500, 1500); // Random delay from 500 ms to 1500 ms

    //while (!CLOSE_WINDOW) {
        threads.emplace_back(generateBall); // Create a new thread for each ball and store it in the vector

        // Sleep for a random duration
        std::this_thread::sleep_for(std::chrono::milliseconds(distribution(gen)));
    //}

    // Join all threads before exiting
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
}

// Function to generate and move the rectangle
void generateRectangle() {
    rect.move(); // Move the rectangle
}

// Function to render the scene
void renderScene(GLFWwindow* window) {
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer

    // Draw and move all balls
    for (auto& ballPtr : ballPointers) {
        mtx.lock();
        Ball& ball = *ballPtr;
        ball.draw();
        mtx.unlock();
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
            Rectangle::stop(); // Break the while loop in rectangle
            ballGenerator.join(); // Join the ballGenerator thread
            rectangleGenerator.join(); // Join the rectangleGenerator thread
            break; // Exit the loop
        }
    }

    // Delete dynamically allocated Ball objects
    for (auto& ballPtr : ballPointers) {
        delete ballPtr;
    }
    ballPointers.clear(); // Clear the list of ball pointers

    glfwDestroyWindow(window); // Destroy the window
    glfwTerminate(); // Terminate GLFW
    return 0;
}