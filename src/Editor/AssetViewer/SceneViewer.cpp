#include "TextureViewer.hpp"

#include <Aka/Layer/ImGuiLayer.h>
#include "SceneViewer.hpp"

namespace app {

using namespace aka;

SceneViewer::SceneViewer(ResourceID _resourceID, ResourceHandle<SceneAvecUnNomChelou> _resourceHandle) :
	AssetViewer(ResourceType::Scene, _resourceID, _resourceHandle)
{
}
};