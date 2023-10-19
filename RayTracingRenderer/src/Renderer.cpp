#include "Renderer.h"

#include "Walnut/Random.h"

namespace Utils
{
	static uint32_t ConvertToRGBA(const glm::vec4& color)
	{
		uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);

		return (a << 24) | (b << 16) | (g << 8) | r;
	}
}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (m_FinalImage)
	{
		//No resize necessary
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
			return; 
		m_FinalImage->Resize(width, height);
	}
	else
	{
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];
}

void Renderer::Render(const Scene& scene, const Camera& camera)
{
	Ray ray;
	ray.Origin = camera.GetPosition();;

	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{
			size_t index = x + y * m_FinalImage->GetWidth();
			if (camera.GetRayDirections().size() <= index)
				continue;
			ray.Direction =  camera.GetRayDirections()[index];

			glm::vec4 color = TraceRay(scene, ray);
			color = glm::clamp(color, 0.0f, 1.0f);
			m_ImageData[y * m_FinalImage->GetWidth() + x] = Utils::ConvertToRGBA(color);
		}
	}
	m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::TraceRay(const Scene& scene, const Ray& ray)
{
	//(bx^2 + by^2)t^2 + 2(abxb + ayby)t + (ax^2 + ay^2 - r^2) = 0
	//a = (bx^2 + by^2) = Ray Origin
	//b = (ax^2 + ay^2 - r^2) = Ray Direction
	//r = radius
	//t = (-b +- sqrt(b^2 - 4ac)) / 2a = Intersection or Hit Distance
	if (scene.Spheres.size() <= 0)
		return glm::vec4(0, 0, 0, 1);
	const Sphere* closestSphere = nullptr;
	float hitDistance = FLT_MAX;
	for each (const Sphere & sphere in scene.Spheres)
	{
		glm::vec3 origin = ray.Origin - sphere.Position;

		float a = glm::dot(ray.Direction, ray.Direction);
		float b = 2.0f * glm::dot(origin, ray.Direction);
		float c = glm::dot(origin, origin) - scene.Spheres[0].Radius * scene.Spheres[0].Radius;

		// Discriminant = b^2 - 4ac
		float discriminant = b * b - 4.0f * a * c;
		if (discriminant < 0.0f)
		{
			continue;
		}
		float t[]{
			(-b - sqrt(discriminant)) / (2.0f * a),
			(-b + sqrt(discriminant)) / (2.0f * a)
		};

		for (int i = 0; i < 1; i++)
		{
			if (t[i] > 0.0f)
			{
				if (t[i] < hitDistance)
				{
					hitDistance = t[i];
					closestSphere = &sphere;
				}
			}
		}
	}
	if (nullptr == closestSphere)
		return glm::vec4(0, 0, 0, 1);

	glm::vec3 hitPoint = ray.Origin - closestSphere->Position + ray.Direction * hitDistance;
	glm::vec3 normal = glm::normalize(hitPoint);

	glm::vec3 lightDir = glm::normalize(glm::vec3(-2.0f, -2.0f, 2.0f));

	float intensity = glm::dot(normal, -lightDir);
	intensity = glm::max(intensity, 0.0f);

	glm::vec3 color = closestSphere->Albedo;
	color *= intensity;
	return glm::vec4(color, 1.0f);
}
