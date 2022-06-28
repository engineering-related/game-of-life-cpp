#include<iostream>
#include <math.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <cassert>
#include "Camera.h"

const unsigned int SCREEN_WIDTH = 1024;
const unsigned int SCREEN_HEIGHT = 1024;

const unsigned short OPENGL_MAJOR_VERSION = 4;
const unsigned short OPENGL_MINOR_VERSION = 6;

bool vSync = false;

GLfloat vertices[] =
{
	 0.0f,          0.0f,          0.0f, 0.0f, 0.0f,
	 0.0f,          SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f,
	 SCREEN_WIDTH,  SCREEN_HEIGHT, 0.0f, 1.0f, 1.0f,
	 SCREEN_WIDTH,  0.0f,          0.0f, 1.0f, 0.0f,
};

GLuint indices[] =
{
	0, 2, 1,
	0, 3, 2
};


const char* screenVertexShaderSource = R"(#version 460 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uvs;
out vec2 UVs;

uniform mat4 u_ViewProj;

void main()
{
	gl_Position = u_ViewProj * vec4(pos.x, pos.y, pos.z, 1.0);
	UVs = uvs;
})";

const char* screenFragmentShaderSource = R"(#version 460 core
out vec4 FragColor;
uniform sampler2D screen;
in vec2 UVs;
void main()
{
	FragColor = texture(screen, UVs);
})";

const char* screenComputeShaderSource = R"(#version 460 core
layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;

layout(rgba32f, binding = 0) writeonly uniform image2D screen;
layout(rgba32f, binding = 1) readonly uniform image2D screen_old;

#define DEAD_COLOR vec4(0.0f, 0.0f, 0.0f, 1.0f)  
#define ALIVE_COLOR vec4(1.0f, 1.0f, 1.0f, 1.0f) 

int cell(vec2 offset, vec2 coord)
{
	return int(imageLoad(screen_old, ivec2(coord - offset)).r);
}

void main()
{
    // Cell Data
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
    vec4 pixel_color = imageLoad(screen_old, pixel_coords);
    ivec2 image_size = imageSize(screen_old);

    int neighborCount = // Number of live neighboring cells
	  cell(vec2(-1.0f, -1.0f), pixel_coords)
	+ cell(vec2(-1.0f, 0.0), pixel_coords)
	+ cell(vec2(-1.0f, 1.0f), pixel_coords)
	+ cell(vec2(0.0, -1.0f), pixel_coords)
	+ cell(vec2(0.0, 1.0f), pixel_coords)
	+ cell(vec2(1.0f, -1.0f), pixel_coords)
	+ cell(vec2(1.0f, 0.0), pixel_coords)
	+ cell(vec2(1.0f, 1.0f), pixel_coords);
    
    if(neighborCount == 3) // Come to life
	{
      imageStore(screen, pixel_coords, ALIVE_COLOR); 
	}
	else if(neighborCount == 2) // Continue to live
	{
      imageStore(screen, pixel_coords, pixel_color); 
	}
	else // Die from underpopulation or overpopulation
	{
      imageStore(screen, pixel_coords, DEAD_COLOR); 
	}
})";

int main()
{
	glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR_VERSION);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR_VERSION);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL Game Of Life", NULL, NULL);
	if (!window)
	{
		std::cout << "Failed to create the GLFW window\n";
		glfwTerminate();
        return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(vSync);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
	}
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);


	GLuint VAO, VBO, EBO;
	glCreateVertexArrays(1, &VAO);
	glCreateBuffers(1, &VBO);
	glCreateBuffers(1, &EBO);

	glNamedBufferData(VBO, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glNamedBufferData(EBO, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexArrayAttrib(VAO, 0);
	glVertexArrayAttribBinding(VAO, 0, 0);
	glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);

	glEnableVertexArrayAttrib(VAO, 1);
	glVertexArrayAttribBinding(VAO, 1, 0);
	glVertexArrayAttribFormat(VAO, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat));

	glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 5 * sizeof(GLfloat));
	glVertexArrayElementBuffer(VAO, EBO);

    GLuint screenTexOld;
    glGenTextures(1, &screenTexOld);
    glBindTexture(GL_TEXTURE_2D, screenTexOld);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // load and generate the texture
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char *data = stbi_load("glider_gun.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        assert(nrChannels == 3);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
        return -1;
    }
    stbi_image_free(data);
	glBindImageTexture(1, screenTexOld, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

	GLuint screenTex;
	glCreateTextures(GL_TEXTURE_2D, 1, &screenTex);
	glTextureParameteri(screenTex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(screenTex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(screenTex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(screenTex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureStorage2D(screenTex, 1, GL_RGBA32F, width, height);
	glBindImageTexture(0, screenTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	GLuint screenVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(screenVertexShader, 1, &screenVertexShaderSource, NULL);
	glCompileShader(screenVertexShader);
	GLuint screenFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(screenFragmentShader, 1, &screenFragmentShaderSource, NULL);
	glCompileShader(screenFragmentShader);

	GLuint screenShaderProgram = glCreateProgram();
	glAttachShader(screenShaderProgram, screenVertexShader);
	glAttachShader(screenShaderProgram, screenFragmentShader);
	glLinkProgram(screenShaderProgram);

	glDeleteShader(screenVertexShader);
	glDeleteShader(screenFragmentShader);

	GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(computeShader, 1, &screenComputeShaderSource, NULL);
	glCompileShader(computeShader);

	GLuint computeProgram = glCreateProgram();
	glAttachShader(computeProgram, computeShader);
	glLinkProgram(computeProgram);

	int work_grp_cnt[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);
	std::cout << "Max work groups per compute shader" << 
		" x:" << work_grp_cnt[0] <<
		" y:" << work_grp_cnt[1] <<
		" z:" << work_grp_cnt[2] << "\n";

	int work_grp_size[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);
	std::cout << "Max work group sizes" <<
		" x:" << work_grp_size[0] <<
		" y:" << work_grp_size[1] <<
		" z:" << work_grp_size[2] << "\n";

	int work_grp_inv;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
	std::cout << "Max invocations count per work group: " << work_grp_inv << "\n";

    float lastTimeSim = 0.0f;
    float lastTimeReal = 0.0f;
    float currentTime = 0.0f;
    float deltaTime = 0.0f;
    float targetFrameRate = 144.0f;
    float targetFrameRateSim = 25.0f;

    Camera camera(0.0f, SCREEN_WIDTH, 0.0f, SCREEN_HEIGHT, -1.0f, 1.0f);

	while (!glfwWindowShouldClose(window))
	{
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTimeReal;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f );
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if(currentTime - lastTimeSim >= 1.0/ targetFrameRateSim)
        {
            glUseProgram(computeProgram);
            glDispatchCompute(ceil(width / 8), ceil(height / 4), 1);
            glMemoryBarrier(GL_ALL_BARRIER_BITS);

            glCopyImageSubData(screenTex,    GL_TEXTURE_2D, 0, 0, 0, 0,
                               screenTexOld, GL_TEXTURE_2D, 0, 0, 0, 0,
                               width, height, 1);

            lastTimeSim = currentTime;
        }

        if(deltaTime >= 1.0 / targetFrameRate)
        {
            glUseProgram(screenShaderProgram);

            glBindTextureUnit(0, screenTex);

            glUniform1i(glGetUniformLocation(screenShaderProgram, "screen"), 0);

            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);

		    glfwSwapBuffers(window);
            lastTimeReal = currentTime;
        }

        camera.update(window, deltaTime);
        camera.matrix(screenShaderProgram, "u_ViewProj");
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
    return 0;
}
