#include <cstdint>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <string.h>

// #define NDEBUG

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
} 

const uint32_t width = 1600;
const uint32_t height = 800;

int main() {
  // Initialize GLFW
  glfwInit();

  // Tell GLFW what version of OpenGL we are using
  // In this case we are using OpenGL 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  // Tell GLFW we are using the CORE profile
  // So that means we only have the modern functions
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // glfwWindowHint(GLFW_SAMPLES, 4);

  // Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
  GLFWwindow *window =
      glfwCreateWindow(width, height, "GOL", nullptr, nullptr);
  // Error check if the window fails to create
  if (window == nullptr) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  // Introduce the window into the current context
  glfwMakeContextCurrent(window);
  // Load GLAD so it configures OpenGL
  gladLoadGL();
  // Specify the viewport of OpenGL in the Window
  // In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  

  // Enables the Depth Buffer
  // glEnable(GL_DEPTH_TEST);

  glfwSwapInterval(0); // Disable vSync
  //TODO: Fix face culling (relative to the camera)
  // Face culling
  // glEnable(GL_CULL_FACE);
  // glCullFace(GL_FRONT);
  // glFrontFace(GL_CW);
  // glEnable(GL_BLEND);
  // glEnable(GL_POLYGON_OFFSET_FILL);
  // glEnable(GL_MULTISAMPLE);  

  // Delta time for clocking frames
  float deltaTime = 0.0f;
  float lastFrame = 0.0f;

  uint32_t counter = 0.0f;
  float lastFrameCouner = 0.0f;
  float timeDiffCounter = 0.0f;
  
  // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

  // Main while loop
  while (!glfwWindowShouldClose(window)) {
    // Delta time
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    timeDiffCounter = currentFrame - lastFrameCouner;
    counter++;

    if (timeDiffCounter >= 1.0f / 30.0f) {
      std::string FPS = std::to_string((1.0 / timeDiffCounter) * counter);
      std::string ms = std::to_string((timeDiffCounter / counter) * 1000);
      std::string newTitle = "GOL - " + FPS + " FPS / " + ms + " ms";
      glfwSetWindowTitle(window, newTitle.c_str());
      lastFrameCouner = currentFrame;
      counter = 0;  
    }

    lastFrame = currentFrame;

    // Specify the color of the background
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    // Clean the back buffer and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Swap the back buffer with the front buffer
    glfwSwapBuffers(window);
    // Take care of all GLFW events
    glfwPollEvents();
  }

  // Delete window before ending the program
  glfwDestroyWindow(window);
  // Terminate GLFW before ending the program
  glfwTerminate();
  exit(0);
  return 0;
}
