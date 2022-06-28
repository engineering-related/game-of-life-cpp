#ifndef CAMERA

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Camera
{
public:
    Camera(float left, float right, float bottom, float top, float near, float far);
    ~Camera();

    void update(GLFWwindow *window, const float& deltaTime);
    void matrix(GLuint shader, const char* uniform);

private:
    glm::mat4 proj;
    glm::mat4 view;
    glm::mat4 viewProj;
    float left;
    float right;
    float bottom;
    float top;
    float near;
    float far;
    const float cam_speed = 100.f;
};


#endif // !CAMERA
