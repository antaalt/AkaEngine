#include "TextureViewer.hpp"

#include <Aka/Layer/ImGuiLayer.h>

namespace app {

using namespace aka;

const char* toString(gfx::TextureFormat format)
{
	switch (format)
	{
	case gfx::TextureFormat::R8:
		return "R8";
	case gfx::TextureFormat::R8U:
		return "R8U";
	case gfx::TextureFormat::R16:
		return "R16";
	case gfx::TextureFormat::R16U:
		return "R16U";
	case gfx::TextureFormat::R16F:
		return "R16F";
	case gfx::TextureFormat::R32F:
		return "R32F";
	case gfx::TextureFormat::RG8:
		return "RG8";
	case gfx::TextureFormat::RG8U:
		return "RG8U";
	case gfx::TextureFormat::RG16U:
		return "RG16U";
	case gfx::TextureFormat::RG16:
		return "RG16";
	case gfx::TextureFormat::RG16F:
		return "RG16F";
	case gfx::TextureFormat::RG32F:
		return "RG32F";
	case gfx::TextureFormat::RGB8:
		return "RGB8";
	case gfx::TextureFormat::RGB8U:
		return "RGB8U";
	case gfx::TextureFormat::RGB16:
		return "RGB16";
	case gfx::TextureFormat::RGB16U:
		return "RGB16U";
	case gfx::TextureFormat::RGB16F:
		return "RGB16F";
	case gfx::TextureFormat::RGB32F:
		return "RGB32F";
	case gfx::TextureFormat::RGBA8:
		return "RGBA8";
	case gfx::TextureFormat::RGBA8U:
		return "RGBA8U";
	case gfx::TextureFormat::RGBA16:
		return "RGBA16";
	case gfx::TextureFormat::RGBA16U:
		return "RGBA16U";
	case gfx::TextureFormat::RGBA16F:
		return "RGBA16F";
	case gfx::TextureFormat::RGBA32F:
		return "RGBA32F";
	case gfx::TextureFormat::Depth:
		return "Depth";
	case gfx::TextureFormat::Depth16:
		return "Depth16";
	case gfx::TextureFormat::Depth24:
		return "Depth24";
	case gfx::TextureFormat::Depth32:
		return "Depth32";
	case gfx::TextureFormat::Depth32F:
		return "Depth32F";
	case gfx::TextureFormat::DepthStencil:
		return "DepthStencil";
	case gfx::TextureFormat::Depth0Stencil8:
		return "Depth0Stencil8";
	case gfx::TextureFormat::Depth24Stencil8:
		return "Depth24Stencil8";
	case gfx::TextureFormat::Depth32FStencil8:
		return "Depth32FStencil8";
	default:
		return "Unknown";
	}
}

const char* toString(gfx::TextureType type)
{
	switch (type)
	{
	case gfx::TextureType::Texture2D:
		return "Texture2D";
	case gfx::TextureType::Texture2DMultisample:
		return "Texture2DMultisample";
	case gfx::TextureType::TextureCubeMap:
		return "TextureCubemap";
	default:
		return "Unknown";
	}
}

TextureViewer::TextureViewer(AssetID _assetID, ResourceHandle<Texture> _resourceHandle) :
	AssetViewer(ResourceType::Texture, _assetID, _resourceHandle),
	m_descriptorSet(gfx::DescriptorSetHandle::null),
	m_sampler(gfx::SamplerHandle::null),
	m_needUpdate(false),
	m_layerSelected(0),
	m_mipSelected(0)
{
}

TextureViewer::~TextureViewer()
{
}

void TextureViewer::onCreate(gfx::GraphicDevice* _device)
{
	gfx::ShaderBindingState state{};
	state.add(gfx::ShaderBindingType::SampledImage, gfx::ShaderMask::Fragment);

	// TODO should share descriptor pool between texture viewer.
	m_descriptorPool = _device->createDescriptorPool("ImGuiTextureViewerDescriptorPool", state, 1);
	m_descriptorSet = _device->allocateDescriptorSet("ImGuiTextureViewerDescriptorSet", state, m_descriptorPool);
	m_sampler = _device->createSampler("ImGuiTextureViewerSampler",
		gfx::Filter::Nearest, gfx::Filter::Nearest,
		gfx::SamplerMipMapMode::None,
		gfx::SamplerAddressMode::Repeat, gfx::SamplerAddressMode::Repeat, gfx::SamplerAddressMode::Repeat,
		1.0);
	// Should load a dummy texture waiting for main to be loaded
	if (m_resource.isLoaded())
	{
		gfx::DescriptorSetData data;
		data.addSampledTexture2D(m_resource.get().getGfxHandle(), m_sampler, m_layerSelected, m_mipSelected);
		_device->update(m_descriptorSet, data);
		m_needUpdate = false;
	}
	else
	{
		m_needUpdate = true; // wait for a texture to be available
	}
}

void TextureViewer::onDestroy(gfx::GraphicDevice* _device)
{
	_device->free(m_descriptorSet);
	_device->destroy(m_descriptorPool);
	_device->destroy(m_sampler);
}

void TextureViewer::onRender(gfx::GraphicDevice* _device, aka::gfx::Frame* frame)
{
	if (m_resource.isLoaded() && m_needUpdate)
	{
		_device->wait();
		gfx::DescriptorSetData data;
		data.addSampledTexture2D(m_resource.get().getGfxHandle(), m_sampler, m_layerSelected, m_mipSelected);
		_device->update(m_descriptorSet, data);
		m_needUpdate = false;
	}
}

void TextureViewer::onLoad(const Texture& texture)
{
}

void TextureViewer::onUpdate(aka::Time deltaTime)
{
}

void TextureViewer::drawUIResource(const app::Texture& texture)
{
	if (m_needUpdate)
	{
		ImGui::Text("Texture need update");
		return;
	}
	gfx::TextureHandle handle = texture.getGfxHandle();
	bool isRenderTarget = (texture.getTextureUsage() & gfx::TextureUsage::RenderTarget) == gfx::TextureUsage::RenderTarget;
	bool isShaderResource = (texture.getTextureUsage() & gfx::TextureUsage::ShaderResource) == gfx::TextureUsage::ShaderResource;
	bool hasMips = (texture.getTextureUsage() & gfx::TextureUsage::GenerateMips) == gfx::TextureUsage::GenerateMips;
	ImGui::Text("%s[%d] - %u x %u (%s)", toString(texture.getTextureType()), m_mipSelected, texture.getWidth() >> m_mipSelected, texture.getHeight() >> m_mipSelected, toString(texture.getTextureFormat()));
	ImGui::Checkbox("Render target", &isRenderTarget); ImGui::SameLine();
	ImGui::Checkbox("Shader resource", &isShaderResource); ImGui::SameLine();
	ImGui::Checkbox("Mips", &hasMips);

	ImGui::Separator();

	constexpr float minZoom = 0.1f;
	constexpr float maxZoom = 5.f;
	constexpr float zoomScale = 0.1f;
	bool red = asBool(m_mask & gfx::ColorMask::Red);
	bool green = asBool(m_mask & gfx::ColorMask::Green);
	bool blue = asBool(m_mask & gfx::ColorMask::Blue);
	bool alpha = asBool(m_mask & gfx::ColorMask::Alpha);
	ImGui::Checkbox("R", &red); ImGui::SameLine();
	ImGui::Checkbox("G", &green); ImGui::SameLine();
	ImGui::Checkbox("B", &blue); ImGui::SameLine();
	ImGui::Checkbox("A", &alpha); ImGui::SameLine();

	m_mask = gfx::ColorMask::None;
	if (red)   m_mask |= gfx::ColorMask::Red;
	if (green) m_mask |= gfx::ColorMask::Green;
	if (blue)  m_mask |= gfx::ColorMask::Blue;
	if (alpha) m_mask |= gfx::ColorMask::Alpha;

	bool zoomChanged = false;
	float zoomPercent = m_zoom * 100.f;
	if (ImGui::DragFloat("##Zoom", &zoomPercent, 0.1f, minZoom * 100.f, maxZoom * 100.f, "%.0f %%"))
	{
		zoomChanged = true;
		m_zoom = zoomPercent / 100.f;
	}

	ImGui::Separator();
	m_needUpdate |= ImGui::SliderInt("Layer", &m_layerSelected, 0, texture.getLayerCount() - 1);
	m_needUpdate |= ImGui::SliderInt("Mip", &m_mipSelected, 0, texture.getMipCount() - 1);

	// Image explorer
	gfx::GraphicDevice* device = Application::app()->graphic();
	ImTextureID textureID = ImGuiLayer::getTextureID(device, m_descriptorSet);
	ImVec4 mask = ImVec4(
		red ? 1.f : 0.f,
		green ? 1.f : 0.f,
		blue ? 1.f : 0.f,
		alpha ? 1.f : 0.f
	);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	if (ImGui::BeginChild("TextureDisplayChild", ImVec2(0, 0), true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar))
	{
		if (ImGui::IsWindowHovered())
		{
			const ImGuiIO& io = ImGui::GetIO();
			ImVec2 scrollPosition = ImVec2(0.f, 0.f);
			scrollPosition.x = ImGui::GetScrollX();
			scrollPosition.y = ImGui::GetScrollY();
			if (zoomChanged || io.MouseWheel != 0.f && !ImGui::IsAnyItemActive())
				m_zoom = clamp(m_zoom + io.MouseWheel * zoomScale, minZoom, maxZoom);

			if (ImGui::IsMouseDragging(0, 0.0f) || ImGui::IsMouseDragging(1, 0.0f))
			{
				scrollPosition.x = clamp(scrollPosition.x - io.MouseDelta.x, 0.f, ImGui::GetScrollMaxX());
				scrollPosition.y = clamp(scrollPosition.y - io.MouseDelta.y, 0.f, ImGui::GetScrollMaxY());
			}
			ImGui::SetScrollX(scrollPosition.x);
			ImGui::SetScrollY(scrollPosition.y);
		}
		ImGui::Image(textureID, ImVec2(m_zoom * texture.getWidth(), m_zoom * texture.getHeight()), ImVec2(0, 0), ImVec2(1, 1), mask);
	}
	ImGui::EndChild();
	ImGui::PopStyleVar();
}


};