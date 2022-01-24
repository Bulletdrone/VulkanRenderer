#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Transform
{
public:
	Transform() 
		: Position(glm::vec3(0)), Rotation(glm::vec3(0)), Scale(glm::vec3(1)) {}
	
	Transform(glm::vec3 a_Pos, float a_Scale)
		: Position(a_Pos), Rotation(0), Scale(glm::vec3(a_Scale)) {}

	Transform(glm::vec3 a_Pos, glm::vec3 a_Scale)
		: Position(a_Pos), Rotation(0), Scale(a_Scale) {}

	Transform(glm::vec3 a_Pos, glm::vec3 a_Rot, glm::vec3 a_Scale)
		: Position(a_Pos), Rotation(a_Rot), Scale(a_Scale) {}

	~Transform() {}

	//Standard Positions.
	glm::vec3 Position;
	glm::vec3 Rotation;
	glm::vec3 Scale;
};