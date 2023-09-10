#pragma once

#include <Aka/Core/Layer.h>
#include <Aka/Graphic/GraphicDevice.h>

#include "../Asset/Resource/Resource.hpp"

namespace app {

class Scene;

class SceneEditorLayer : public aka::Layer {
public:
	SceneEditorLayer();

	void onLayerCreate() override;
	void onLayerDestroy() override;

	void onLayerFrame() override;
	void onLayerRender(aka::gfx::Frame* frame) override;
	void onLayerPresent() override;
	void onLayerResize(uint32_t width, uint32_t height) override;
public:
	void setCurrentScene(ResourceHandle<Scene>* _scene);
private:
	ResourceHandle<Scene>* m_scene;
};

};