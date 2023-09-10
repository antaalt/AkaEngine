#pragma once

#include <Aka/Core/Layer.h>
#include <Aka/Graphic/GraphicDevice.h>

namespace app {

class AssetLibrary;

class InfoEditorLayer : public aka::Layer {
public:
	InfoEditorLayer();

	void onLayerCreate() override;
	void onLayerDestroy() override;

	void onLayerFrame() override;
	void onLayerRender(aka::gfx::Frame* frame) override;
	void onLayerPresent() override;
	void onLayerResize(uint32_t width, uint32_t height) override;
private:
	uint32_t m_width;
	uint32_t m_height;
};

};