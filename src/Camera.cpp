#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <Camera.h>
#include <iostream>

Camera::Camera(float left, float right, float bottom, float top, float near, float far)
{
    this->left = left;
    this->right = right;
    this->bottom = bottom;
    this->top = top;
    this->near = near;
    this->far = far;
    proj = glm::ortho(left, right, bottom, top, near, far);
    view = glm::mat4(1.0f);
}

Camera::~Camera()
{

}

void Camera::matrix(GLuint shader, const char* uniform)
{
    glUniformMatrix4fv(glGetUniformLocation(shader, uniform), 1, GL_FALSE, glm::value_ptr(viewProj));
}

void Camera::update(GLFWwindow *window, const float& deltaTime) 
{ 
    static bool pan_start = false;
    static bool zoom_start = false;
    static glm::vec2 mouse_start;

    static double mouseX;
    static double mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);   

    if (!pan_start && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) 
    {   
        pan_start = true;
        mouse_start = glm::vec2(mouseX, mouseY);
    }
    else if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) 
    {

        view = glm::translate(view, glm::vec3((mouse_start.x - mouseX) / (right - left) * deltaTime * cam_speed, 
                                             -(mouse_start.y - mouseY) / (top - bottom) * deltaTime * cam_speed, 0.0f));
    }
    else 
    {
        pan_start = false;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) 
    {   
        left -= deltaTime * cam_speed;
        right += deltaTime * cam_speed;
        bottom -= deltaTime * cam_speed;
        top += deltaTime * cam_speed;
        proj = glm::ortho(left, right, bottom, top, near, far);
        zoom_start = true;
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) 
    {   
        left += deltaTime * cam_speed;
        right -= deltaTime * cam_speed;
        bottom += deltaTime * cam_speed;
        top -= deltaTime * cam_speed;
        proj = glm::ortho(left, right, bottom, top, near, far);
    }

    viewProj = proj * view;
}
