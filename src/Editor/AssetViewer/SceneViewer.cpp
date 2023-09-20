#include "TextureViewer.hpp"

#include <Aka/Layer/ImGuiLayer.h>
#include "SceneViewer.hpp"

namespace app {
app::SceneViewer::SceneViewer(ResourceID _resourceID, ResourceHandle<Scene> _resourceHandle) :
	AssetViewer(ResourceType::Scene, _resourceID, _resourceHandle)
{
}
};