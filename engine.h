#ifndef ENGINE_H
#define ENGINE_H

#include <glm/glm.hpp>
#include <utility>
#include <iostream>

struct ray
{
	glm::vec3 p{}; // position
	glm::vec3 d{}; // direction
	glm::vec3 evaluate(float t)
	{
		return p + t * d;
	}
};

struct camera
{
	glm::vec3 e{0, 0, -5}; // viewpoint

	glm::vec3 u{1, 0, 0}; // right basis vector
	glm::vec3 v{0, 1, 0}; // up basis vector
	glm::vec3 w{0, 0, -1}; // back basis vector

	int nx{}; // x resolution
	int ny{}; // y resolution

	int l{-1}; // left bound
	int r{1}; // right bound
	int b{-1}; // bottom bound
	int t{1}; // top bound
};

struct orthographic_camera : public camera
{
	ray generate_ray(int i, int j)
	{
		ray viewing_ray{};

		float coord_u = l + (r - l) * (i + 0.5) / nx;
		float coord_v = b + (t - b) * (j + 0.5) / ny;

		viewing_ray.p = e + (u * coord_u) + (coord_v * v);
		viewing_ray.d = -w;

		return viewing_ray;
	}
};

struct perspective_camera : public camera
{
	float d{3.0f}; // depth

	ray generate_ray(int i, int j)
	{
		ray viewing_ray{};

		float coord_u = l + (r - l) * (i + 0.5) / nx;
		float coord_v = b + (t - b) * (j + 0.5) / ny;

		viewing_ray.p = e;
		viewing_ray.d = -d * w + (u * coord_u) + (coord_v * v);

		return viewing_ray;
	}
};

struct intersection
{
	int hits{};
	glm::vec3 first_hit{};
	int first_hit_t{};
	int second_hit_t{};
	glm::vec3 normal{};

};

struct sphere
{
	glm::vec3 c{};
	float r{1.0f};

	intersection intersect(ray& view_ray)
	{
		intersection i{};
		glm::vec3 ec{view_ray.p-c};
		float dd{glm::dot(view_ray.d, view_ray.d)};
		float discriminant = glm::pow(glm::dot(view_ray.d, ec), 2) - dd * (glm::dot(ec, ec) - glm::pow(r, 2));
		if (discriminant >= 0) // two solutions
		{
			// ray goes through object
			i.first_hit_t = (glm::dot(-view_ray.d, ec) + glm::sqrt(discriminant)) / dd;
			i.first_hit = view_ray.evaluate(i.first_hit_t);
			i.normal = 2.0f * (i.first_hit - c);
			i.hits = 2;

			if (discriminant == 0) // one solution
			{
				// ray is tangent to object
				i.second_hit_t = (glm::dot(-view_ray.d, ec) - glm::sqrt(discriminant)) / dd;
				i.hits = 1;
			}
		}

		// if ray misses object, do nothing

		return i;
	}
};

struct application
{
	// Create the image (RGB Array) to be displayed
	perspective_camera cam{};
	const int width  = 128; // keep it in powers of 2!
	const int height = 128; // keep it in powers of 2!
	unsigned char* image{new unsigned char[width*height*3]};
	std::vector<sphere> scene{};

	application()
	{
		cam.nx=width;
		cam.ny=height;

		scene.push_back(sphere{});
		scene.push_back(sphere{glm::vec3{2, 0, 1}});

		update_image();
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

	void loop()
	{
		
	}

	~application()
	{
		delete[] image;
	}
};

#endif