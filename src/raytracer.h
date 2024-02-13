#ifndef RAYTRACER_H
#define RAYTRACER_H

#include <GL/glew.h>
#include <vector>
#include <list>

#include <stb_image_write.h>

#include "engine.h"
#include "animation.h"

struct ray_tracer
{
	// Create the image (RGB Array) to be displayed
	camera cam{};

	int res_pow{6};
	int export_res_pow{10};

	int width  = glm::pow(2, res_pow); // keep it in powers of 2!
	int height = width; // keep it in powers of 2!
	unsigned char* image{new unsigned char[width*height*3]};
	std::vector<surface*> scene{};
	std::vector<ambient_light> ambient_lights{};
	std::vector<point_light> point_lights{};
	std::vector<material*> materials{};

	bool blinn_phong{false};
	int bounce_count{1};

	ray_tracer()
	{
		cam.nx=width;
		cam.ny=height;

		materials.push_back(new material{0.5, 0.4, 0.8, 32, true});
		materials.push_back(new material{0.25, 0.4, 0.6, 100, true});
		materials.push_back(new material{0.4, 0.4, 0.25, 16});
		materials.push_back(new material{0.5, 0.2, 0.2, 8});

		glm::vec3 points[4]=
		{
			glm::vec3(0, 3, 0),
			glm::vec3(1, 1, -1),
			glm::vec3(-1, 1, -1),
			glm::vec3(0, 1, 1)
		};
		scene.push_back(new triangle{points[1], points[2], points[3]});
		scene.back()->m=materials[0];
		scene.push_back(new triangle{points[0], points[1], points[2]});
		scene.back()->m=materials[0];
		scene.push_back(new triangle{points[0], points[2], points[3]});
		scene.back()->m=materials[0];
		scene.push_back(new triangle{points[0], points[3], points[1]});
		scene.back()->m=materials[0];

		scene.push_back(new sphere{});
		scene.back()->m=materials[1];
		scene.back()->color=glm::vec3{75.0f/255, 255.0f/255, 0.0f/255};
		scene.back()->r=1.5f;
		scene.back()->center=glm::vec3{-3.0, 2.0, 0};

		scene.push_back(new sphere{});
		scene.back()->m=materials[2];
		scene.back()->color=glm::vec3{0.0f/255, 210.0f/255, 255.0f/255};
		scene.back()->r=0.5f;
		scene.back()->center=glm::vec3{-5, 2.5, 2.0};
		// scene.back()->center=glm::vec3{0, 2.0, -2.0};

		scene.push_back(new triangle{});
		scene.back()->m=materials[3];
		scene.back()->color=glm::vec3{200.0f/255, 200.0f/255, 200.0f/255};
		dynamic_cast<triangle*>(scene.back())->plane=true;


		ambient_lights.push_back(ambient_light{});

		point_lights.push_back(point_light{glm::vec3{-2.0, 4.0, -3.0}, glm::vec3{0, 1, 1}, glm::vec3{1, 0, 1}, 1.0f});
		point_lights.push_back(point_light{glm::vec3{-0.5, 3.5, 0.5}, glm::vec3{1, 1, 0}, glm::vec3{1, 0, 0.5}, 1.5f});

		cam.e=glm::vec3{1.5f, 4.0f, -2.0f};

		lookat(glm::vec3{-0.5, 1.5, 0.0});
	}

	void addSphere()
	{
		scene.push_back(new sphere{});
		scene.back()->m=materials[0];
	}

	void addTriangle()
	{
		scene.push_back(new triangle{});
		scene.back()->m=materials[0];
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

				hit_information closest_hit{calculate_hit(r)};
				
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
	}

	void lightAnimation(float time)
	{
		for (auto& l : point_lights)
		{
			float animationTime{time/l.period}; // between 0 and 1
			if (animationTime < 0.5)
			{
				l.color=customMix(l.animationEndColor, l.animationStartColor, animationTime);
			}
			else
			{
				l.color=customMix(l.animationStartColor, l.animationEndColor, animationTime);
			}
		}
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
			color += l.illuminate(r, hit, scene, blinn_phong);
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


		if (depth > bounce_count)
		{
			return color;
		}
		depth++;

		// mirror reflection
		if (hit.s->m->glazed == true)
		{
			glm::vec3 l{glm::normalize(r.d-2.0f*hit.normal*glm::dot(r.d, hit.normal))};
			ray reflection{r.evaluate(hit.t)+0.1f*l, l};
			hit_information reflection_hit{calculate_hit(reflection)};
			float dist{glm::length(r.p - reflection.p)};
			if (reflection_hit.hits != 0)
			{
				color += shader(reflection, reflection_hit, depth)*hit.s->m->k_s;
			}
		}
		return color;
	}

	~ray_tracer()
	{
		delete[] image;

		for (auto& obj : scene)
		{
			delete obj;
		}
		for (auto& obj : materials)
		{
			delete obj;
		}
	}
};

#endif