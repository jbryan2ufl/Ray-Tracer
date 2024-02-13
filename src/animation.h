#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include <utility>
#include <numbers>

#include <glm/glm.hpp>


inline float easeInOutSine(float x)
{
	return -(std::cos(std::numbers::pi*x)-1)/2;
}

inline glm::vec3 customMix(glm::vec3 a, glm::vec3 b, float t)
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

inline float customMixF(float a, float b, float t)
{
	float pos{};
	float range{};

	// x
	range=b-a;
	pos=range*easeInOutSine(t)+a;

	return pos;
}

struct keyframe
{
	float time{};
	glm::vec3 position{};
	glm::vec3 lookat{};
	float depth{};
};

struct animation_manager
{
	// std::vector<std::pair<float, glm::vec3>> keyframes;
	std::vector<keyframe> keyframes;

	keyframe get_keyframe(float t)
	{
		float totalTime{keyframes.back().time};
		t=std::fmod(t, totalTime);
		keyframe k{};

		// if (t > totalTime)
		// {
		// 	k=keyframes.back();
		// 	return k;
		// }

		glm::vec3 pos1{};
		glm::vec3 pos2{};
		
		float interpolate{};
		for (int i{}; i < keyframes.size(); i++)
		{
			if (t > keyframes[i].time && t < keyframes[i+1].time)
			{
				pos1=keyframes[i].position;
				pos2=keyframes[i+1].position;
				interpolate = (t-keyframes[i].time) / (keyframes[i+1].time - keyframes[i].time);
				k.position=customMix(pos1, pos2, interpolate);
				k.lookat=customMix(keyframes[i].lookat, keyframes[i+1].lookat, interpolate);
				k.depth=customMixF(keyframes[i].depth, keyframes[i+1].depth, interpolate);
				return k;
			}
		}
		return k;

	}
};

#endif