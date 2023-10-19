#include "Renderer.h"

#include "Walnut/Random.h"

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (m_FinalImage)
	{
		//No resize necessary
		if (m_FinalImage->GetWidth() != width || m_FinalImage->GetHeight() != height)
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

void Renderer::Render()
{
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{
			glm::vec2 coord = { x / (float)m_FinalImage->GetWidth(),  y / (float)m_FinalImage->GetHeight()};
			coord = coord * 2.0f - 1.0f;
			m_ImageData[y * m_FinalImage->GetWidth() + x] = PerPixel({ coord });
		}
	}
	m_FinalImage->SetData(m_ImageData);
}

uint32_t Renderer::PerPixel(glm::vec2 coord)
{
	uint8_t r = (uint8_t)(coord.x * 255.0f);
	uint8_t g = (uint8_t)(coord.y * 255.0f);

	glm::vec3 rayOrigin = { 0.0f, 0.0f, 2.0f };
	glm::vec3 rayDirection = { coord.x, coord.y, -1.0f };
	rayDirection = glm::normalize(rayDirection);
	float radius = 0.5f;

	//(bx^2 + by^2)t^2 + 2(abxb + ayby)t + (ax^2 + ay^2 - r^2) = 0
	//a = (bx^2 + by^2) = Ray Origin
	//b = (ax^2 + ay^2 - r^2) = Ray Direction
	//r = radius
	//t = (-b +- sqrt(b^2 - 4ac)) / 2a = Intersection or Hit Distance
	float a = glm::dot(rayDirection, rayDirection);
	float b = 2.0f * glm::dot(rayOrigin, rayDirection);
	float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

	// Discriminant = b^2 - 4ac
	float discriminant = b * b - 4.0f * a * c;
	if (discriminant >= 0)
		return 0xFF000000 | (r << 16) | (g << 8) | 0xFF;
	return 0xFF000000;
}
