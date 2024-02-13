#ifndef ENGINE_H
#define ENGINE_H

#include <glm/glm.hpp>
#include <utility>
#include <iostream>
#include <vector>
#include <cmath>

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
	glm::vec3 e{}; // viewpoint

	glm::vec3 u{1, 0, 0}; // right basis vector
	glm::vec3 v{0, 1, 0}; // up basis vector
	glm::vec3 w{0, 0, -1}; // back basis vector

	glm::vec3 world_up{v};

	int nx{}; // x resolution
	int ny{}; // y resolution

	float l{-5}; // left bound
	float r{5}; // right bound
	float b{-5}; // bottom bound
	float t{5}; // top bound

	float d{5.0f};

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

struct surface;

struct hit_information
{
	surface* s{nullptr};
	int hits{};

	float t{INF};
	glm::vec3 normal{};

};

struct material
{
	// float k_a{1.0};
	float k_a{0.1};
	float k_d{0.8};
	float k_s{0.3};
	int p{8};
	bool glazed{false};
};

struct surface
{
	bool visible{true};
	glm::vec3 color{1.0f, 0.0f, 0.0f};
	material* m{};
	int mat_idx{};
	glm::vec3 center{};
	float r{1.0f};

	virtual hit_information intersect(ray& view_ray) = 0;	
};

struct sphere : public surface
{
	sphere()
	{
	}

	hit_information intersect(ray& view_ray)
	{
		hit_information i{};
		i.s = this;
		glm::vec3 ec{view_ray.p-center};
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
			i.normal = (view_ray.evaluate(i.t)-center)/r;
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

struct triangle : public surface
{
	glm::vec3 p1{-1, 0, -1};
	glm::vec3 p2{-1, 0, 1};
	glm::vec3 p3{1, 0, 1};
	// glm::vec3 p1{-5, 0, -5};
	// glm::vec3 p2{-5, 0, 5};
	// glm::vec3 p3{5, 0, 5};

	bool plane{false};

	triangle()
	{
	}

	triangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
		: p1{p1}
		, p2{p2}
		, p3{p3}
	{
	}

	void move(glm::vec3 pos)
	{
		p1+=pos;
		p2+=pos;
		p3+=pos;
	}

	hit_information intersect(ray& view_ray)
	{
		hit_information h{};
		h.s=this;
		glm::vec3 normal{glm::normalize(glm::cross(p2-p1,p3-p1))};


		h.t=glm::dot(p1-view_ray.p, normal)/glm::dot(normal, view_ray.d);
		if (h.t < 0)
		{
			return h;
		}
		glm::vec3 x{view_ray.evaluate(h.t)};
		float e1{glm::dot(glm::cross(p2-p1, x-p1), normal)};
		float e2{glm::dot(glm::cross(p3-p2, x-p2), normal)};
		float e3{glm::dot(glm::cross(p1-p3, x-p3), normal)};

		// if (e1 <= 0 || e2 <= 0 || e3 <= 0)
		// {
		// 	return h;
		// }
		if ((e1 > 0 && e2 > 0 && e3 > 0) || plane)
		{
			h.normal=normal;
			h.hits=1;
		}

		return h;
	}
};

struct light
{
	glm::vec3 color{1.0f, 1.0f, 1.0f};
	bool visible{true};
	// virtual glm::vec3 illuminate(ray& r, hit_information& hit);
};

struct ambient_light : public light
{
	glm::vec3 illuminate(ray& r, hit_information& hit)
	{
		return hit.s->m->k_a * hit.s->color * color;
	}
};

struct point_light : public light
{
	// simple_sphere sph{&p, &color};

	glm::vec3 p{0, 2, 0}; // light position
	glm::vec3 animationStartColor{1, 1, 1};
	glm::vec3 animationEndColor{1, 1, 1};
	float period{};

	point_light()
	{
	}
	point_light(glm::vec3 pos)
		: p{pos}
	{
	}

	point_light(glm::vec3 pos, glm::vec3 startColor, glm::vec3 endColor, float period)
		: p{pos}
		, animationStartColor{startColor}
		, animationEndColor{endColor}
		, period{period}
	{
	}

	glm::vec3 illuminate(ray& r, hit_information& hit, std::vector<surface*>& scene, bool& blinn_phong)
	{
		if (!visible)
		{
			return glm::vec3{0, 0, 0};
		}
		glm::vec3 x{r.evaluate(hit.t)};
		float dist{glm::length(p - x)};
		glm::vec3 l{(p-x)/dist}; // normalized ray pointing to light
		ray light_ray{x+0.01f*l, l};
		hit_information h;
		for (auto& obj : scene)
		{
			if (hit.s == obj || obj->visible==false)
			{
				continue;
			}


			h = obj->intersect(light_ray);

			// shadows
			if (h.hits != 0)
			{
				return glm::vec3{0, 0, 0};
			}
		}
		glm::vec3 E{glm::max(0.0f,glm::dot(hit.normal,l)) * color}; // /(float)glm::pow(dist,2)

		glm::vec3 Ld = hit.s->m->k_d*hit.s->color*E;

		// phong model
		glm::vec3 Ls{};
		if (!blinn_phong)
		{
			glm::vec3 vR{-glm::normalize(2*glm::dot(hit.normal, l)*hit.normal-l)};
			glm::vec3 vE{r.d};
			Ls = hit.s->m->k_s*color*(float)glm::pow(glm::max(0.0f, glm::dot(vE, vR)), hit.s->m->p);
		}
		else
		{
			glm::vec3 v2{glm::normalize(l-r.d)};
			Ls = hit.s->m->k_s*(float)glm::pow(glm::max(0.0f,glm::dot(hit.normal,v2)), hit.s->m->p)*E*color;
		}
		return Ld+Ls;
	}
};

#endif