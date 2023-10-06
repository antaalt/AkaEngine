#pragma once

#include <Aka/Core/Layer.h>
#include <Aka/Graphic/GraphicDevice.h>

#include "EditorLayer.hpp"

namespace app {

class AssetLibrary;

class InfoEditorLayer : public aka::Layer {
public:
	InfoEditorLayer();

	void onLayerCreate(aka::gfx::GraphicDevice* _device) override;
	void onLayerDestroy(aka::gfx::GraphicDevice* _device) override;

	void onLayerPreRender() override;
	void onLayerRender(aka::gfx::GraphicDevice* _device, aka::gfx::FrameHandle frame) override;
	void onLayerPostRender() override;
	void onLayerResize(uint32_t width, uint32_t height) override;
	
	void setEditorLayer(EditorLayerType type, EditorLayer* layer);
private:
	EditorLayer* m_editorLayer[aka::EnumCount<EditorLayerType>()] = { nullptr };
	uint32_t m_width;
	uint32_t m_height;
};

};