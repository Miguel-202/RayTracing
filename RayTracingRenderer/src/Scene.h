#pragma once

#include <glm/glm.hpp>
#include <vector>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	IF YOU ARE SEEING THIS, THIS IS NOT THE LATES VERSION, GO TO https://github.com/Miguel-202/RayTracing FOR THE LATEST VERSION//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct Material
{
	glm::vec3 Albedo{ 1.0f };
	float Roughness = 1.0f;
	float Metallic = 0.0f;
	float EmmisionPower = 0.0f;
	glm::vec3 EmmisionColor{ 1.0f };

	glm::vec3 GetEmmision() const { return EmmisionPower * EmmisionColor; }
};

struct Sphere
{
	glm::vec3 Position{0.0f};
	float Radius = 0.5f;

	int MaterialIndex = 0;
};

struct Scene
{
	std::vector<Sphere> Spheres;
	std::vector<Material> Materials;
};