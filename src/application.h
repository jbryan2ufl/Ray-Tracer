#ifndef APPLICATION_H
#define APPLICATION_H

// Based on templates from learnopengl.com
#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include "engine.h"
#include "raytracer.h"

class application
{
private:
	void processInput(GLFWwindow *window);
	
	// settings
	const unsigned int SCR_WIDTH =  1080;
	const unsigned int SCR_HEIGHT = 1080;

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

	ray_tracer* rt;

public:
	application();
	void init();
	void loop();
	void close();
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
};

static application* callback;

#endif