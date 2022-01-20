#pragma once
#include <glm/glm.hpp>

namespace Engine
{
	struct SceneData
	{
		glm::vec4 fogColor; // w is for exponent
		glm::vec4 fogDistance; //x for min, y for max, zw unused.

		glm::vec4 ambientColor;

		glm::vec4 sunlightDirection; //w for sun power
		glm::vec4 sunlightColor;
	};
}