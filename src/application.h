#ifndef APPLICATION_H
#define APPLICATION_H

// Based on templates from learnopengl.com
#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <utility>

#include <string>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "engine.h"
#include "raytracer.h"
#include "animation.h"

class application
{
private:
	void processInput(GLFWwindow *window);
	
	// settings
	const unsigned int SCR_WIDTH =  1364;
	const unsigned int SCR_HEIGHT = 1024;

	const char *vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"layout (location = 1) in vec3 aColor;\n"
		"layout (location = 2) in vec2 aTexCoord;\n"
		"out vec3 ourColor;\n"
		"out vec2 TexCoord;\n"
		"void main()\n"
		"{\n"
		"gl_Position = vec4(aPos, 1.0);\n"
		"ourColor = aColor;\n"
		"TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
		"}\0";

	const char *fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"in vec3 ourColor;\n"
		"in vec2 TexCoord;\n"
		"uniform sampler2D texture1;\n"
		"void main()\n"
		"{\n"
		"   FragColor = texture(texture1, TexCoord);\n"
		"}\0";

	GLFWwindow* window;
	unsigned int shaderProgram;
	unsigned int texture;
	unsigned int VBO, VAO, EBO;

	ray_tracer rt{};

	const float maxFPS = 60;
	const float maxPeriod = 1.0f / maxFPS;
	float lastTime = 0.0;
	float deltaTime{};
	float time{};
	float moveStartTime{};

	bool freemove{true};

	int keyframe_time{};
	int frameCount{};
	const float videoFPS=30;
	const float maxVideoPeriod=1.0f/videoFPS;
	float videoTime{};

	ImGuiIO* ioptr{};

	animation a{};


public:
	void process_keys(int key, int scancode, int action, int mods);
	void framebuffer_size(int width, int height);
	application();
	void init();
	void loop();
	void close();
};

#endif