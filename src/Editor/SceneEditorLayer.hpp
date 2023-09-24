#pragma once

#include <Aka/Core/Layer.h>
#include <Aka/Graphic/GraphicDevice.h>
#include <Aka/Resource/Resource/Resource.hpp>
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

	void onFrame() override;
	void onRender(aka::gfx::GraphicDevice* _device, aka::gfx::Frame* frame) override;
	void onDrawUI() override;
	void onPresent() override;
	void onResize(uint32_t width, uint32_t height) override;
	void onReceive(const aka::ResourceLoadedEvent& event);
public:
	void setLibrary(aka::AssetLibrary* library);
	void setCurrentScene(aka::ResourceHandle<aka::Scene> _scene);
	void setCurrentCameraController(aka::CameraController* _camera) { m_cameraController = _camera; }
	void setCurrentCameraProjection(aka::CameraProjection* _camera) { m_cameraProjection = _camera; }
private:
	aka::AssetLibrary* m_library;
	ImGuizmo::OPERATION m_gizmoOperation = ImGuizmo::TRANSLATE;
	aka::Node* m_nodeToDestroy = nullptr;
	aka::Node* m_currentNode = nullptr;
	aka::ResourceHandle<aka::Scene> m_scene;
	aka::CameraController* m_cameraController = nullptr;
	aka::CameraProjection* m_cameraProjection = nullptr;
};

};