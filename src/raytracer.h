#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <GL/glew.h>
#include <vector>
#include <list>

#include <stb_image_write.h>

#include "engine.h"

struct ray_tracer
{
	// Create the image (RGB Array) to be displayed
	camera cam{};

	int res_pow{7};
	int export_res_pow{6};

	int width  = glm::pow(2, res_pow); // keep it in powers of 2!
	int height = width; // keep it in powers of 2!
	unsigned char* image{new unsigned char[width*height*3]};
	std::vector<surface*> scene{};
	std::vector<ambient_light> ambient_lights{};
	std::vector<point_light> point_lights{};

	ray_tracer()
	{
		cam.nx=width;
		cam.ny=height;

		scene.push_back(new sphere{});
		scene.push_back(new triangle{});

		ambient_lights.push_back(ambient_light{});
		point_lights.push_back(point_light{});
	}

	hit_information calculate_hit(ray& r)
	{
		hit_information h{};
		for (auto& obj : scene)
		{
			if (obj->visible == false)
			{
				continue;
			}
			hit_information hit{obj->intersect(r)};
			if (hit.hits != 0)
			{
				if (hit.t < h.t)
				{
					h = hit;
				}
			}
		}
		return h;
	}

	void lookat(glm::vec3 point)
	{
		glm::vec3 d{glm::normalize(cam.e-point)};
		cam.w = d;
		cam.u = glm::normalize(glm::cross(cam.w, cam.world_up));
		cam.v = glm::normalize(glm::cross(cam.u, cam.w));
	}

	void update_image()
	{
		for(int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				int idx = (i * width + j) * 3;

				ray r{cam.generate_ray(j, i)};

				// for (auto& l : point_lights)
				// {
				// 	if (l.sph.intersect(r))
				// 	{
				// 		image[idx+0] = l.color.r * 255;
				// 		image[idx+1] = l.color.g * 255;
				// 		image[idx+2] = l.color.b * 255;
				// 		return;
				// 	}
				// }

				hit_information closest_hit{calculate_hit(r)};
				
				// glm::vec3 color{get_hit_color(r, closest_hit)};
				glm::vec3 color{};
				int depth{};
				if (closest_hit.hits != 0)
				{
					color=shader(r, closest_hit, depth);
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

	void export_image(std::string s)
	{
		int res{glm::pow(2,export_res_pow)};
		width  = res;
		height = res;
		resize(true);
		update_image();

		stbi_flip_vertically_on_write(true);
		stbi_write_jpg(("images/"+s).c_str(), res, res, 3, image, 100);

		res = glm::pow(2,res_pow);
		width  = res;
		height = res;
		resize();
		update_image();
		std::cout << "ATTEMPTING TO WRITE IMAGE\n";

	}

	void resize(bool exporting=false)
	{
		delete[] image;

		exporting ? width = glm::pow(2, export_res_pow) : width = glm::pow(2, res_pow);
		// width = glm::pow(2, res_pow);
		height = width;
		cam.nx = width;
		cam.ny=height;
		image = new unsigned char[width*height*3];
	}

	glm::vec3 shader(ray& r, hit_information& hit, int& depth)
	{
		glm::vec3 color{};
		for (auto& l : point_lights)
		{
			if (l.visible == false)
			{
				continue;
			}
			color += l.illuminate(r, hit, scene);
			// color += l.specular(r, hit);
		}
		for (auto& l : ambient_lights)
		{
			if (l.visible == false)
			{
				continue;
			}
			color += l.illuminate(r, hit);
		}


		if (depth > 1)
		{
			return color;
		}
		depth++;

		// mirror reflection
		if (hit.s->m.glazed == true)
		{
			glm::vec3 l{glm::normalize(r.d-2.0f*hit.normal*glm::dot(r.d, hit.normal))};
			ray reflection{r.evaluate(hit.t)+0.1f*l, l};
			hit_information reflection_hit{calculate_hit(reflection)};
			float dist{glm::length(r.p - reflection.p)};
			if (reflection_hit.hits != 0)
			{
				color += shader(reflection, reflection_hit, depth)*reflection_hit.s->m.k_s;
			}
		}
		return color;
	}

	void move()
	{
		cam.e.x+=1;
	}

	~ray_tracer()
	{
		delete[] image;
	}
};

#endif