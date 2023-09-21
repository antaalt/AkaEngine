#pragma once

#include <Aka/Resource/Resource/Scene.hpp>
#include "AssetViewer.hpp"

namespace app {

class SceneViewer : public AssetViewer<aka::SceneAvecUnNomChelou>
{
public:
	SceneViewer(aka::ResourceID _resourceID, aka::ResourceHandle<aka::SceneAvecUnNomChelou> _resourceHandle);
	~SceneViewer() {}

	void onCreate(aka::gfx::GraphicDevice* _device) override {}
	void onDestroy(aka::gfx::GraphicDevice* _device) override {}
	void onUpdate(aka::Time deltaTime) override {}
	void onRender(aka::gfx::GraphicDevice* _device, aka::gfx::Frame* frame) override {}
protected:
	void drawUIResource(const aka::SceneAvecUnNomChelou& scene) override {}
private:
};

};