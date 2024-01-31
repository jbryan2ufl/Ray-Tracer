#ifndef ENGINE_H
#define ENGINE_H

#include <glm/glm.hpp>
#include <utility>
#include <iostream>

#define INF 999999

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
	bool active{false};

	glm::vec3 e{0, 0, -5}; // viewpoint

	glm::vec3 u{1, 0, 0}; // right basis vector
	glm::vec3 v{0, 1, 0}; // up basis vector
	glm::vec3 w{0, 0, -1}; // back basis vector

	glm::vec3 world_up{v};

	int nx{}; // x resolution
	int ny{}; // y resolution

	int l{-1}; // left bound
	int r{1}; // right bound
	int b{-1}; // bottom bound
	int t{1}; // top bound

	float d{1.3f};

	ray viewing_ray{};

	bool ortho{false};

	ray& generate_ray(int i, int j)
	{
		if (ortho)
		{
			generate_ray_orthographic(i, j);
		}
		else
		{
			generate_ray_perspective(i, j);
		}
		return viewing_ray;
	}

	void generate_ray_orthographic(int i, int j)
	{
		float coord_u = l + (r - l) * (i + 0.5) / nx;
		float coord_v = b + (t - b) * (j + 0.5) / ny;

		viewing_ray.p = e + (u * coord_u) + (coord_v * v);
		viewing_ray.d = -w;
	}

	void generate_ray_perspective(int i, int j)
	{
		float coord_u = l + (r - l) * (i + 0.5) / nx;
		float coord_v = b + (t - b) * (j + 0.5) / ny;

		viewing_ray.p = e;
		viewing_ray.d = glm::normalize(-d * w + (u * coord_u) + (coord_v * v));
	}

	void toggle_cam()
	{
		ortho = !ortho;
	}
};

struct hit_information;

struct material
{
	glm::vec3 k_a{0.05};
	glm::vec3 k_d{1.0};
	glm::vec3 k_s{0.3};
	int p{8};
};

struct surface
{
	virtual hit_information intersect(ray& view_ray) = 0;
	glm::vec3 color{1.0f, 0.0f, 0.0f};
	material m{};
};

struct hit_information
{
	surface* s{nullptr};
	int hits{};

	float t{INF};
	glm::vec3 normal{};

};

struct sphere : public surface
{
	glm::vec3 c{0.0f, 0.0f, 0.0f};
	float r{1.0f};

	sphere()
	{
	}

	sphere(glm::vec3 c, float r, glm::vec3 col, material mat)
		: c{c}
		, r{r}
	{
		color=col;
		m=mat;
	}

	hit_information intersect(ray& view_ray)
	{
		hit_information i{};
		i.s = this;
		glm::vec3 ec{view_ray.p-c};
		float dd{glm::dot(view_ray.d, view_ray.d)};
		float discriminant = glm::pow(glm::dot(view_ray.d, ec), 2) - dd * (glm::dot(ec, ec) - glm::pow(r, 2));
		if (discriminant >= 0) // at least one solutions
		{
			// ray goes through object
			i.t = glm::min((glm::dot(-view_ray.d, ec) - glm::sqrt(discriminant)) / dd, (glm::dot(-view_ray.d, ec) + glm::sqrt(discriminant)) / dd);
			if (i.t < 0)
			{
				return i;
			}
			i.normal = (view_ray.evaluate(i.t)-c)/r;
			i.hits = 2;

			if (discriminant == 0) // one solution
			{
				// ray is tangent to object
				i.hits = 1;
			}
		}

		// if ray misses object, do nothing

		return i;
	}
};

struct light
{
	glm::vec3 color{1.0f, 1.0f, 1.0f};
	// virtual glm::vec3 illuminate(ray& r, hit_information& hit);
};

struct ambient_light : public light
{
	glm::vec3 illuminate(ray& r, hit_information& hit)
	{
		return hit.s->m.k_a * hit.s->color * color;
	}
};

struct point_light : public light
{
	glm::vec3 p{}; // light position

	point_light(glm::vec3 pos)
		: p{pos}
	{
	}

	glm::vec3 illuminate(ray& r, hit_information& hit, std::vector<sphere>& scene)
	{
		glm::vec3 x{r.evaluate(hit.t)};
		float dist{glm::length(p - x)};
		glm::vec3 l{(p-x)/dist}; // normalized ray pointing to light
		ray light_ray{x+0.1f*l, l};
		hit_information h;
		for (auto& obj : scene)
		{
			if (hit.s == &obj)
			{
				continue;
			}
			h = obj.intersect(light_ray);
			if (h.hits != 0)
			{
				return glm::vec3{0, 0, 0};
			}
		}
		glm::vec3 E{glm::max(0.0f,glm::dot(hit.normal,l)) * color /(float)glm::pow(dist,2)}; // /(float)glm::pow(dist,2)
		glm::vec3 h2{glm::normalize(l-r.d)};
		return hit.s->m.k_s*(float)glm::pow(glm::max(0.0f,glm::dot(hit.normal,h2)), hit.s->m.p)*E*color+hit.s->m.k_d*hit.s->color*E;
	}
};

#endif