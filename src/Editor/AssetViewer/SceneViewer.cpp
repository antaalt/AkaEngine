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
	ImGui::Text("%s", scene.getName().cstr());
	aabbox<> b = scene.getBounds();
	ImGui::InputFloat3("Min", b.min.data);
	ImGui::InputFloat3("Max", b.max.data);
}

};