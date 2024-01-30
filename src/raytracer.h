#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <GL/glew.h>
#include <vector>

#include "engine.h"

struct ray_tracer
{
	// Create the image (RGB Array) to be displayed
	perspective_camera cam{};
	const int width  = 512; // keep it in powers of 2!
	const int height = 512; // keep it in powers of 2!
	unsigned char* image{new unsigned char[width*height*3]};
	std::vector<sphere> scene{};

	ray_tracer()
	{
		cam.nx=width;
		cam.ny=height;

		scene.push_back(sphere{});
		scene.push_back(sphere{glm::vec3{2, 0, 1}});
	}

	void update_image()
	{
		for(int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				int idx = (i * width + j) * 3;
				
				// image[idx] = (unsigned char) (255 * i*j/height/width)  ; //((i+j) % 2) * 255;
				image[idx] = 0;
				image[idx+1] = 0;
				image[idx+2] = 0;

				ray r{cam.generate_ray(j, i)};

				for (auto& obj : scene)
				{
					intersection p{obj.intersect(r)};
					if (p.hits != 0)
					{
						image[idx] = 255;
						image[idx+1] = 255;
						image[idx+2] = 255;
					}
				}
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

	~ray_tracer()
	{
		delete[] image;
	}
};

#endif