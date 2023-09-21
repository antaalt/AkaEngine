#pragma once

#include <Aka/Core/Layer.h>
#include <Aka/Graphic/GraphicDevice.h>

#include <Aka/Resource/Resource/Resource.hpp>

namespace app {

class SceneEditorLayer : public aka::Layer {
public:
	SceneEditorLayer();

	void onLayerCreate(aka::gfx::GraphicDevice* _device) override;
	void onLayerDestroy(aka::gfx::GraphicDevice* _device) override;

	void onLayerFrame() override;
	void onLayerRender(aka::gfx::Frame* frame) override;
	void onLayerPresent() override;
	void onLayerResize(uint32_t width, uint32_t height) override;
public:
	void setCurrentScene(aka::ResourceHandle<aka::Scene>* _scene);
private:
	aka::ResourceHandle<aka::Scene>* m_scene;
};

};