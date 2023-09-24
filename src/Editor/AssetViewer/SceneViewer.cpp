#include "TextureViewer.hpp"

#include <Aka/Layer/ImGuiLayer.h>
#include "SceneViewer.hpp"

namespace app {

using namespace aka;

SceneViewer::SceneViewer(AssetID _assetID, ResourceHandle<Scene> _resourceHandle) :
	AssetViewer(ResourceType::Scene, _assetID, _resourceHandle)
{
}
};