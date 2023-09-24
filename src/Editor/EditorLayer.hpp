#pragma once

#include <Aka/Core/Layer.h>
#include <Aka/Core/Container/String.h>

namespace app {

enum class EditorLayerType
{
	Unknown,

	AssetViewer,
	AssetBrowser,
	SceneEditor,

	First = AssetViewer,
	Last = SceneEditor,
};

class EditorLayer : public aka::Layer
{
public:
	EditorLayer(const char* name) : m_name(name), m_visible(true), m_enabled(true) {}
	virtual ~EditorLayer() {}

	void setVisible(bool visible) { m_visible = visible; }
	bool isVisible() const { return m_visible; }
	void setEnabled(bool enable) { m_enabled = enable; }
	bool isEnabled() const { return m_enabled; }
	const char* getName() const { return m_name.cstr(); }
protected:
	virtual void onCreate(aka::gfx::GraphicDevice* _device) {}
	virtual void onDestroy(aka::gfx::GraphicDevice* _device) {}
	virtual void onUpdate(aka::Time deltaTime) {}
	virtual void onFixedUpdate(aka::Time deltaTime) {}
	virtual void onFrame() {}
	virtual void onRender(aka::gfx::GraphicDevice* _device, aka::gfx::Frame* frame) {}
	virtual void onDrawUI() {}
	virtual void onPresent() {}
	virtual void onResize(uint32_t width, uint32_t height) {}
private:
	void onLayerCreate(aka::gfx::GraphicDevice* _device) final;
	void onLayerDestroy(aka::gfx::GraphicDevice* _device) final;

	void onLayerUpdate(aka::Time deltaTime) final;
	void onLayerFixedUpdate(aka::Time deltaTime) final;
	void onLayerFrame() final;
	void onLayerRender(aka::gfx::GraphicDevice* _device, aka::gfx::Frame* frame) final;
	void onLayerPresent() final;

	void onLayerResize(uint32_t width, uint32_t height) final;
private:
	aka::String m_name;
	bool m_visible;
	bool m_enabled;
};

};