#include "TextureViewer.hpp"

#include <Aka/Layer/ImGuiLayer.h>
#include "SceneViewer.hpp"

namespace app {

using namespace aka;

SceneViewer::SceneViewer(AssetID _assetID, ResourceHandle<Scene> _resourceHandle) :
	AssetViewer(ResourceType::Scene, _assetID, _resourceHandle)
{
}

void SceneViewer::drawUIResource(const aka::Scene& scene)
{
#if defined(AKA_USE_IMGUI_LAYER)
	ImGui::Text("%s", scene.getName().cstr());
	aabbox<> b = scene.getBounds();
	ImGui::InputFloat3("Min", b.min.data);
	ImGui::InputFloat3("Max", b.max.data);
#endif
}

};