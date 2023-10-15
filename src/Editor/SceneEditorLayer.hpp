#pragma once

#include <Aka/Core/Layer.h>
#include <Aka/Graphic/GraphicDevice.h>
#include <Aka/Resource/Resource/Resource.hpp>
#include <Aka/Resource/Asset.hpp>
#include <Aka/Core/Event.h>
#include <Aka/Scene/Node.hpp>
#include <Aka/Scene/Component/CameraComponent.hpp>
#include <Aka/Layer/ImGuiLayer.h>

#include "EditorLayer.hpp"


namespace app {

class SceneEditorLayer : public EditorLayer, aka::EventListener<aka::ResourceLoadedEvent>
{
public:
	SceneEditorLayer();

	void onCreate(aka::gfx::GraphicDevice* _device) override;
	void onDestroy(aka::gfx::GraphicDevice* _device) override;

	void onPreRender() override;
	void onRender(aka::gfx::GraphicDevice* _device, aka::gfx::FrameHandle frame) override;
	void onDrawUI() override;
	void onPostRender() override;
	void onResize(uint32_t width, uint32_t height) override;
	void onReceive(const aka::ResourceLoadedEvent& event);
public:
	void setLibrary(aka::AssetLibrary* library);
	void setCurrentScene(aka::ResourceHandle<aka::Scene> _scene);
private:
	aka::AssetLibrary* m_library;
	ImGuizmo::MODE m_gizmoMode = ImGuizmo::WORLD;
	ImGuizmo::OPERATION m_gizmoOperation = ImGuizmo::TRANSLATE;
	aka::Node* m_nodeToDestroy = nullptr;
	aka::Node* m_currentNode = nullptr;
	aka::AssetID m_assetToUnload = aka::AssetID::Invalid;
	aka::ResourceHandle<aka::Scene> m_scene;
};

};