#pragma once

#include <Aka/Core/Layer.h>
#include <Aka/Core/Container/String.h>
#include <Aka/Renderer/DebugDraw/DebugDrawList.hpp>

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
	virtual void onCreate(aka::Renderer* _renderer) {}
	virtual void onDestroy(aka::Renderer* _renderer) {}
	virtual void onUpdate(aka::Time deltaTime) {}
	virtual void onFixedUpdate(aka::Time deltaTime) {}
	virtual void onPreRender() {}
	virtual void onRender(aka::Renderer* _renderer, aka::gfx::FrameHandle frame) {}
	virtual void onDrawUI(aka::DebugDrawList& debugDrawList) {}
	virtual void onPostRender() {}
	virtual void onResize(uint32_t width, uint32_t height) {}
private:
	void onLayerCreate(aka::Renderer* _renderer) final;
	void onLayerDestroy(aka::Renderer* _renderer) final;

	void onLayerUpdate(aka::Time deltaTime) final;
	void onLayerFixedUpdate(aka::Time deltaTime) final;
	void onLayerPreRender() final;
	void onLayerRender(aka::Renderer* _renderer, aka::gfx::FrameHandle frame) final;
	void onLayerPostRender() final;

	void onLayerResize(uint32_t width, uint32_t height) final;
private:
	aka::String m_name;
	bool m_visible;
	bool m_enabled;
};

};