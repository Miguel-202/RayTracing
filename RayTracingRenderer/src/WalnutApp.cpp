#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "Renderer.h"
#include "Camera.h"
#include "Scene.h"

#include <glm/gtc/type_ptr.hpp>
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	IF YOU ARE SEEING THIS, THIS IS NOT THE LATES VERSION, GO TO https://github.com/Miguel-202/RayTracing FOR THE LATEST VERSION//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer()
		: m_Camera(45.0f, 0.1f, 100.0f) 
	{
		Material& pinkSphere = m_Scene.Materials.emplace_back();
		pinkSphere.Albedo = { 1.0f, 0.0f, 1.0f };
		pinkSphere.Roughness = 0.0f;

		Material& blueSphere = m_Scene.Materials.emplace_back();
		blueSphere.Albedo = { 0.2f, 0.3f, 1.0f };
		blueSphere.Roughness = 0.1f;

		Material& orangeSphere = m_Scene.Materials.emplace_back();
		orangeSphere.Albedo = { 0.8f, 0.5f, 0.2f };
		orangeSphere.Roughness = 0.1f;
		orangeSphere.EmmisionColor = { 1.0f, 0.5f, 0.2f };
		orangeSphere.EmmisionPower = 1.0f;
		{
			// Purple sphere in the middle
			Sphere sphere;
			sphere.Position = { 0.0f, 0.0f, 0.0f };
			sphere.Radius = 1.0f;
			sphere.MaterialIndex = 0;
			m_Scene.Spheres.push_back(sphere);
		}
		{
			//Blue sphere on the back right
			Sphere sphere;
			sphere.Position = { 0.0f, -101.0f, 0.0f };
			sphere.Radius = 100.0f;
			sphere.MaterialIndex = 1;
			m_Scene.Spheres.push_back(sphere);
		}
		{
			//Orange sphere on the Middle right
			Sphere sphere;
			sphere.Position = { 2.0f, 0.0f, 0.0f };
			sphere.Radius = 1.0f;
			sphere.MaterialIndex = 2;
			m_Scene.Spheres.push_back(sphere);
		}
	}
	
	virtual void OnUpdate(float ts) override
	{
		if(m_Camera.OnUpdate(ts))
			m_Renderer.ResetFrameIndex();
	}
	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("FPS: %.0f", 1000.0f/m_lastRenderTimer);
		if (ImGui::Button("Render"))
		{
			Render();
		}
		ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulate);
		if (ImGui::Button("Reset"))
		{
			m_Renderer.ResetFrameIndex();
		}
		ImGui::End();

		ImGui::Begin("Scene");
		for (size_t i = 0; i < m_Scene.Spheres.size(); i++)
		{
			ImGui::PushID(i);
			Sphere& sphere = m_Scene.Spheres[i];
			ImGui::DragFloat3("Position", glm::value_ptr(sphere.Position), 0.1f);
			ImGui::DragFloat("Radious", &sphere.Radius, 0.01f);
			ImGui::DragInt("Material Index", &sphere.MaterialIndex, 1.0f, 0, m_Scene.Materials.size() - 1);
			ImGui::Separator();
			ImGui::PopID();
		}

		for (size_t i = 0; i < m_Scene.Materials.size(); i++)
		{
			ImGui::PushID(i);
			Material& mat = m_Scene.Materials[i];
			ImGui::ColorEdit3("Albedo", glm::value_ptr(mat.Albedo));
			ImGui::ColorEdit3("Emmisive Color", glm::value_ptr(mat.EmmisionColor));
			ImGui::DragFloat("Roughnes", &mat.Roughness, 0.05f, 0.0f, 1.0f);
			ImGui::DragFloat("Metalic", &mat.Metallic, 0.05f, 0.0f, 1.0f);
			ImGui::DragFloat("Emmision Power", &mat.EmmisionPower, 0.05f, 0.0f, 1000.0f);
			ImGui::Separator();
			ImGui::PopID();
		}

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");
		m_ViewportWidth = ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = ImGui::GetContentRegionAvail().y; 

		auto image = m_Renderer.GetFinalImage();
		if (image)
		{
			ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() },
				ImVec2(0,1), ImVec2(1,0));
		}
		ImGui::End();
		ImGui::PopStyleVar();
		Render();
	}

	void Render()
	{
		Walnut::Timer timer;

		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Renderer.Render(m_Scene, m_Camera);
		
		m_lastRenderTimer = timer.ElapsedMillis();
	}
private:
	Renderer m_Renderer;
	Camera m_Camera;
	Scene m_Scene;
	uint32_t m_ViewportWidth = 500, m_ViewportHeight = 500;

	float m_lastRenderTimer = 0.0f;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracing Renderer";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}