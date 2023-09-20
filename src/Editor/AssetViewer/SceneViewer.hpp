#pragma once

#include "AssetViewer.hpp"
#include "../../Asset/Resource/Scene.hpp"

namespace app {

class SceneViewer : public AssetViewer<Scene>
{
public:
	SceneViewer(ResourceID _resourceID, ResourceHandle<Scene> _resourceHandle);
	~SceneViewer() {}

	void onCreate(gfx::GraphicDevice* _device) override {}
	void onDestroy(gfx::GraphicDevice* _device) override {}
	void onUpdate(aka::Time deltaTime) override {}
	void onRender(gfx::GraphicDevice* _device, aka::gfx::Frame* frame) override {}
protected:
	void drawUIResource(const Scene& scene) override {}
private:
};

};