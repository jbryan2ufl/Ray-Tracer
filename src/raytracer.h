#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <GL/glew.h>
#include <vector>

#include "engine.h"

struct ray_tracer
{
	// Create the image (RGB Array) to be displayed
	camera cam{};
	bool ortho{false};

	const int width  = 128; // keep it in powers of 2!
	const int height = 128; // keep it in powers of 2!
	unsigned char* image{new unsigned char[width*height*3]};
	std::vector<sphere> scene{};
	std::vector<ambient_light> ambient_lights{};
	std::vector<point_light> point_lights{};

	ray_tracer()
	{
		cam.nx=width;
		cam.ny=height;

		material matte{};
		matte.k_s=glm::vec3{0.3};

		material shiny{};
		shiny.k_s=glm::vec3{1.0};
		shiny.p=128;

		scene.push_back(sphere{});
		scene.push_back(sphere{glm::vec3{0, 0, -2}, 0.25f, glm::vec3{0.0f, 0.0f, 1.0f}, shiny});
		scene.push_back(sphere{glm::vec3{2, -1, 2}, 1.5f, glm::vec3{0.0f, 1.0f, 0.0f}, shiny});

		ambient_lights.push_back(ambient_light{});
		point_lights.push_back(point_light{glm::vec3{0.0f, 2.5f, -1.0f}});
		point_lights.push_back(point_light{glm::vec3{0.0f, 0.5f, -3.0f}});
		point_lights.push_back(point_light{glm::vec3{2, 2, 2}});
	}

	void update_image()
	{
		for(int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				int idx = (i * width + j) * 3;

				ray r{cam.generate_ray(j, i)};

				hit_information closest_hit{};
				for (auto& obj : scene)
				{
					hit_information hit{obj.intersect(r)};
					if (hit.hits != 0)
					{
						if (hit.t < closest_hit.t)
						{
							closest_hit = hit;
						}
					}
				}
				glm::vec3 color{};
				if (closest_hit.hits != 0)
				{
					color=shader(r, closest_hit);
				}
				else
				{
					color=glm::vec3{0, 0, 0}; // background color
				}
				color = glm::clamp(color, 0.0f, 1.0f);
				image[idx+0] = color.r * 255;
				image[idx+1] = color.g * 255;
				image[idx+2] = color.b * 255;
			}
		}

		if (image)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
	}

	glm::vec3 shader(ray& r, hit_information& hit)
	{
		glm::vec3 color{};
		for (auto& l : point_lights)
		{
			color += l.illuminate(r, hit, scene);
			// color += l.specular(r, hit);
		}
		for (auto& l : ambient_lights)
		{
			color += l.illuminate(r, hit);
		}
		return color;
	}

	~ray_tracer()
	{
		delete[] image;
	}
};

#endif