#pragma once

#include <Aka/Core/Layer.h>
#include <Aka/Layer/ImGuiLayer.h>

#include "EditorLayer.hpp"


namespace app {

class PerformanceEditorLayer : public EditorLayer
{
public:
	PerformanceEditorLayer();

	void onCreate(aka::Renderer* _renderer) override;
	void onDestroy(aka::Renderer* _renderer) override;

	void onDrawUI(aka::DebugDrawList& debugDrawList) override;
private:
};

};