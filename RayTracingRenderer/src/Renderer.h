#pragma once
#include "Walnut/Image.h"

#include <memory>
#include <glm/glm.hpp>
#include "Camera.h"
#include "Ray.h"
#include "Scene.h"

class Renderer
{
public:
	struct Settings
	{
		bool Accumulate = true;
	};
public:
	Renderer() = default;
	~Renderer() = default;

	void OnResize(uint32_t width, uint32_t height);
	void Render(const Scene& scene, const Camera& camera); 

	std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }

	void ResetFrameIndex() { m_FrameIndex = 1; }
	Settings& GetSettings() { return m_Settings; }
private:
	struct HitPayLoad
	{
		float Hitdistance;
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec3 Albedo;

		uint32_t ObjectIndex;
	};
	glm::vec4 PerPixel(uint32_t x, uint32_t y); //RayGen
	HitPayLoad TraceRay(const Ray& ray);
	HitPayLoad ClosesRayHit(const Ray& ray, float hitDistance, int objectIndex);
	HitPayLoad Miss(const Ray& ray);
private:
	std::shared_ptr<Walnut::Image> m_FinalImage;
	Settings m_Settings;

	const Camera* m_ActiveCamera = nullptr;
	const Scene* m_ActiveScene = nullptr;

	uint32_t* m_ImageData = nullptr;
	glm::vec4* m_AccumulationData = nullptr;

	uint32_t m_MaxBounces = 5;
	uint32_t m_FrameIndex = 1;
};