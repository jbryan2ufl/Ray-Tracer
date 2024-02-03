#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include <utility>
#include <numbers>

#include <glm/glm.hpp>

struct animation
{
	std::vector<std::pair<float, glm::vec3>> keyframes;

	glm::vec3 get_keyframe(float t)
	{
		float totalTime{keyframes.back().first};
		t=std::fmod(t, totalTime);

		if (t > totalTime)
		{
			return keyframes.back().second;
		}

		glm::vec3 pos1{};
		glm::vec3 pos2{};
		float interpolate{};
		for (int i{}; i < keyframes.size(); i++)
		{
			if (t > keyframes[i].first && t < keyframes[i+1].first)
			{
				pos1=keyframes[i].second;
				pos2=keyframes[i+1].second;
				interpolate = (t-keyframes[i].first) / (keyframes[i+1].first - keyframes[i].first);
				return customMix(pos1, pos2, interpolate);
				// return glm::mix(pos1, pos2, interpolate);
			}
		}

		return glm::vec3{};

	}

	glm::vec3 customMix(glm::vec3 a, glm::vec3 b, float t)
	{
		glm::vec3 pos{};
		float range{};

		// x
		range=b.x-a.x;
		pos.x=range*easeInOutSine(t)+a.x;
		// y
		range=b.y-a.y;
		pos.y=range*easeInOutSine(t)+a.y;
		// z
		range=b.z-a.z;
		pos.z=range*easeInOutSine(t)+a.z;

		return pos;
	}

	float easeInOutSine(float x)
	{
		return -(std::cos(std::numbers::pi*x)-1)/2;
	}



};

#endif