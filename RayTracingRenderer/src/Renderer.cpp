#include "Renderer.h"

#include "Walnut/Random.h"

#include <execution>

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

	delete[] m_AccumulationData;
	m_AccumulationData = new glm::vec4[width * height];

	m_ImageHorizontalIt.resize(width);
	m_ImageVerticalIt.resize(height);
	for (uint32_t i = 0; i < width; i++)
		m_ImageHorizontalIt[i] = i;
	for (uint32_t i = 0; i < height; i++)
		m_ImageVerticalIt[i] = i;
}

void Renderer::Render(const Scene& scene, const Camera& camera)
{
	m_ActiveCamera = &camera;
	m_ActiveScene = &scene;

	if (m_FrameIndex == 1)
		memset(m_AccumulationData, 0, m_FinalImage->GetWidth() * m_FinalImage->GetHeight() * sizeof(glm::vec4));

#define MT 1
#if MT

	std::for_each(std::execution::par, m_ImageVerticalIt.begin(), m_ImageVerticalIt.end(),
		[this](uint32_t y)
		{
			for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
			{
				glm::vec4 color = PerPixel(x, y);
				m_AccumulationData[y * m_FinalImage->GetWidth() + x] += color;

				glm::vec4 accumulatedColor = m_AccumulationData[y * m_FinalImage->GetWidth() + x];
				accumulatedColor /= (float)m_FrameIndex;

				accumulatedColor = glm::clamp(accumulatedColor, 0.0f, 1.0f);
				m_ImageData[y * m_FinalImage->GetWidth() + x] = Utils::ConvertToRGBA(accumulatedColor);
			}
	#if 0 //0 for assigning threads to rows, 1 for assigning threads to individual pixels
			std::for_each(std::execution::par, m_ImageHorizontalIt.begin(), m_ImageHorizontalIt.end(),
			[this, y](uint32_t x)
				{
					glm::vec4 color = PerPixel(x, y);
					m_AccumulationData[y * m_FinalImage->GetWidth() + x] += color;

					glm::vec4 accumulatedColor = m_AccumulationData[y * m_FinalImage->GetWidth() + x];
					accumulatedColor /= (float)m_FrameIndex;

					accumulatedColor = glm::clamp(accumulatedColor, 0.0f, 1.0f);
					m_ImageData[y * m_FinalImage->GetWidth() + x] = Utils::ConvertToRGBA(accumulatedColor);
				});
	#endif
		});
#else
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{
			glm::vec4 color = PerPixel(x, y);
			m_AccumulationData[y * m_FinalImage->GetWidth() + x] += color;

			glm::vec4 accumulatedColor = m_AccumulationData[y * m_FinalImage->GetWidth() + x];
			accumulatedColor /= (float)m_FrameIndex;

			accumulatedColor = glm::clamp(accumulatedColor, 0.0f, 1.0f);
			m_ImageData[y * m_FinalImage->GetWidth() + x] = Utils::ConvertToRGBA(accumulatedColor);
		}
	}
#endif
	m_FinalImage->SetData(m_ImageData);

	if(m_Settings.Accumulate)
		m_FrameIndex++;
	else
		m_FrameIndex = 1;
}

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
	Ray ray;
	ray.Origin = m_ActiveCamera->GetPosition();
	size_t index = x + y * m_FinalImage->GetWidth();
	if (m_ActiveCamera->GetRayDirections().size() <= index)
		return glm::vec4(0, 0, 0, 1);
	ray.Direction = m_ActiveCamera->GetRayDirections()[index];
	
	glm::vec3 color(0.0f);
	float multiplier = 1.0f;
	for (int i = 0; i < m_MaxBounces; i++)
	{
		HitPayLoad payload = TraceRay(ray);
		if (payload.Hitdistance < 0.0f)
		{
			glm::vec3 skyColor = glm::vec3(0.166f, 0.233f, 0.333f);
			color += skyColor * multiplier;
			break;
		}

		glm::vec3 lightDir = glm::normalize(glm::vec3(-2.0f, -2.0f, 2.0f));
		float intensity = glm::max(glm::dot(payload.Normal, -lightDir), 0.0f);

		const Sphere& sphere = m_ActiveScene->Spheres[payload.ObjectIndex];
		const Material& material = m_ActiveScene->Materials[sphere.MaterialIndex];
		glm::vec3 objectColor = material.Albedo;
		objectColor *= intensity;
		color += objectColor * multiplier;
		multiplier *= 0.6f;

		ray.Origin =payload.Position + payload.Normal * 0.0001f;
		ray.Direction = glm::reflect(ray.Direction,
			payload.Normal + material.Roughness * Walnut::Random::Vec3(-0.5f, 0.5f));
	}

	return glm::vec4(color, 1.0f);
}

Renderer::HitPayLoad  Renderer::TraceRay(const Ray& ray)
{
	//(bx^2 + by^2)t^2 + 2(abxb + ayby)t + (ax^2 + ay^2 - r^2) = 0
	//a = (bx^2 + by^2) = Ray Origin
	//b = (ax^2 + ay^2 - r^2) = Ray Direction
	//r = radius
	//t = (-b +- sqrt(b^2 - 4ac)) / 2a = Intersection or Hit Distance
	int closestSphere = -1;
	float hitDistance = std::numeric_limits<float>::max();
	for (size_t i = 0; i < m_ActiveScene->Spheres.size(); i++)
	{
		const Sphere& sphere = m_ActiveScene->Spheres[i];
		glm::vec3 origin = ray.Origin - sphere.Position;

		float a = glm::dot(ray.Direction, ray.Direction);
		float b = 2.0f * glm::dot(origin, ray.Direction);
		float c = glm::dot(origin, origin) - sphere.Radius * sphere.Radius;

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
		float closesT = std::min(t[0], t[1]);
		//for (int j = 0; j < 1; j++)
		{
			if (closesT > 0.0f && closesT < hitDistance)
			{
				hitDistance = closesT;
				closestSphere = (int)i;
			}
		}
	}
	if (closestSphere < 0)
		return Miss(ray);

	return ClosesRayHit(ray, hitDistance, closestSphere);
}

Renderer::HitPayLoad  Renderer::ClosesRayHit(const Ray& ray, float hitDistance, int objectIndex)
{
	Renderer::HitPayLoad payload;
	payload.Hitdistance = hitDistance;
	payload.ObjectIndex = objectIndex;

	const Sphere& closestSphere = m_ActiveScene->Spheres[objectIndex];

	payload.Position = ray.Origin - closestSphere.Position + ray.Direction * hitDistance;
	payload.Normal = glm::normalize(payload.Position);

	payload.Position += closestSphere.Position;
	return payload;
}

Renderer::HitPayLoad  Renderer::Miss(const Ray& ray)
{
	Renderer::HitPayLoad payload;
	payload.Hitdistance = -1.0f;
	return payload;
}
