#pragma once

#include <imgui.h>

#include <functional>

#include <Aka/Core/Container/String.h>
#include <Aka/Scene/Camera.h>

#include "../Asset/Resource/Resource.hpp"
#include "../Asset/Resource/StaticMesh.hpp"
#include "../Asset/Resource/Scene.hpp"

namespace app {

struct MenuEntry
{
	aka::String name;
	std::function<void(void)> callback;
};

class AssetViewerBase
{
public:
	AssetViewerBase() {}
	virtual ~AssetViewerBase() {}

	virtual void create() {}
	virtual void destroy() {}
	virtual void update(Time deltaTime) {}
	virtual void render(aka::gfx::Frame* frame) {}
};

template <typename T>
class AssetViewer : public AssetViewerBase
{
public:
	AssetViewer(const char* type);
	virtual ~AssetViewer() {}

	void render(aka::gfx::Frame* frame) override;
	// Set the resource for the viewer.
	void set(ResourceID id, const ResourceHandle<T>& resource) { m_opened = true; m_id = id; m_resource = resource; onResourceChange(); }
protected:
	virtual void draw(const aka::String& name, ResourceHandle<T>& resource) = 0;
	virtual void onResourceChange() {}
protected:
	const char* m_type;
	bool m_opened;
	ResourceID m_id;
	ResourceHandle<T> m_resource;
};

class MeshViewer : public AssetViewer<app::StaticMesh>
{
public:
	MeshViewer();
	void create() override;
	void destroy() override;
	void update(aka::Time deltaTime) override;
protected:
	void draw(const aka::String& name, app::ResourceHandle<StaticMesh>& resource) override;
	void onResourceChange() override;
	void drawMesh(const StaticMesh* mesh);
private:
	const uint32_t m_width = 512;
	const uint32_t m_height = 512;
	aka::mat4f m_projection;
	aka::gfx::TextureHandle m_renderTarget;
	aka::gfx::TextureHandle m_depthTarget;
	aka::gfx::FramebufferHandle m_target;
	aka::gfx::RenderPassHandle m_renderPass;
	aka::gfx::DescriptorSetHandle m_descriptorSet;
	aka::gfx::BufferHandle m_uniform;
	aka::CameraArcball m_arcball;
};

class TextureViewer : public AssetViewer<app::Texture>
{
public:
	TextureViewer();
	void create() override;
	void destroy() override;
	void update(aka::Time deltaTime) override;
protected:
	void draw(const aka::String& name, app::ResourceHandle<app::Texture>& resource) override;
	void onResourceChange() override;
private:
	gfx::DescriptorSetHandle m_descriptorSet;
	gfx::SamplerHandle m_sampler;
	bool m_needUpdate;
};

template<typename T>
inline AssetViewer<T>::AssetViewer(const char* type) :
	m_type(type),
	m_opened(false),
	m_id(ResourceID::Invalid),
	m_resource{}
{
}

template<typename T>
inline void AssetViewer<T>::render(aka::gfx::Frame* frame)
{
	Application* app = Application::app();
	if (m_opened)
	{
		// TODO add tabs when multiple resources opened
		if (ImGui::Begin(m_type, &m_opened, ImGuiWindowFlags_MenuBar))
		{
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					// TODO check if dirty
					if (ImGui::MenuItem("Save"))
					{
						AKA_NOT_IMPLEMENTED;
					}
					if (ImGui::MenuItem("Reload"))
					{
						AKA_NOT_IMPLEMENTED;
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}
			if (m_resource.isLoaded())
			{
				draw(m_resource.get().getName(), m_resource);
			}
			else
			{
				ImGui::Text("Resource is not loaded");
			}
			ImGui::Separator();
			if (ImGui::TreeNode("Asset"))
			{
				/*m_library-> m_resource.get()->getID()
				ImGui::Text("Path : %s", m_resource.path.cstr());*/
				ImGui::TreePop();
			}
		}
		ImGui::End();
	}
}

};