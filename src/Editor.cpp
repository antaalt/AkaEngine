#include "Editor.h"

#include <Aka/Core/Container/String.h>
#include <Aka/OS/OS.h>
#include <Aka/Layer/ImGuiLayer.h>
#include <Aka/Scene/Node.hpp>

#include <imgui.h>
#include <imguizmo.h>
#include <map>
#include <vector>

#include "Importer/AssimpImporter.hpp"
#include "Editor/AssetBrowserEditorLayer.hpp"
#include "Editor/SceneEditorLayer.hpp"
#include "Editor/InfoEditorLayer.hpp"
#include "Editor/PerformanceEditorLayer.hpp"
#include "Component/CustomComponent.hpp"
#include "Component/RotatorComponent.hpp"
#include <Aka/Scene/Component/RigidBodyComponent.hpp>

#include <Aka/Resource/Archive/ArchiveGeometry.hpp>
#include <Aka/Resource/Archive/ArchiveBatch.hpp>
#include <Aka/Scene/Component/StaticMeshComponent.hpp>

using namespace aka;

Editor::Editor(const Config& cfg) :
	Application(cfg)
{
	ImGuiLayer* imgui = getRoot().addLayer<ImGuiLayer>();
	// Editor are child of imgui to support frame
	app::SceneEditorLayer*			sceneEditor			= imgui->addLayer<app::SceneEditorLayer>();
	app::AssetViewerEditorLayer*	assetViewerEditor	= imgui->addLayer<app::AssetViewerEditorLayer>();
	app::AssetBrowserEditorLayer*	assetBrowserEditor	= imgui->addLayer<app::AssetBrowserEditorLayer>();
	app::InfoEditorLayer*			infoEditor			= imgui->addLayer<app::InfoEditorLayer>();
	app::PerformanceEditorLayer*	perfEditor			= imgui->addLayer<app::PerformanceEditorLayer>();
	// Set dependencies
	sceneEditor->setCurrentScene(m_scene);
	sceneEditor->setLibrary(assets());
	assetBrowserEditor->setLibrary(assets());
	assetBrowserEditor->setAssetViewer(assetViewerEditor);
	infoEditor->setEditorLayer(app::EditorLayerType::AssetBrowser, assetBrowserEditor);
	infoEditor->setEditorLayer(app::EditorLayerType::AssetViewer, assetViewerEditor);
	infoEditor->setEditorLayer(app::EditorLayerType::SceneEditor, sceneEditor);
	infoEditor->setEditorLayer(app::EditorLayerType::PerformanceEditor, perfEditor);
}

Editor::~Editor()
{
}


AssetID createPlaneMesh(AssetLibrary* _library, Renderer* _renderer, mat4f _transform)
{
	AssetPath meshPath = AssetPath("xpbd/plane/plane.smesh", AssetPathType::Cooked);
	AssetPath batchPath = AssetPath("xpbd/plane/plane.batch", AssetPathType::Cooked);
	AssetPath geoPath = AssetPath("xpbd/plane/plane.geo", AssetPathType::Cooked);
	AssetPath materialPath = AssetPath("xpbd/plane/plane.mat", AssetPathType::Cooked);
	AssetPath imageAlbedoPath = AssetPath("xpbd/plane/albedo.img", AssetPathType::Cooked);
	AssetPath imageNormalPath = AssetPath("xpbd/plane/normal.img", AssetPathType::Cooked);
	OS::Directory::create(AssetPath("xpbd/plane/", AssetPathType::Cooked).getAbsolutePath());

	// Add to library & load it.
	AssetID meshID = _library->registerAsset(meshPath, AssetType::StaticMesh, true);
	AssetID batchID = _library->registerAsset(batchPath, AssetType::Batch, true);
	AssetID geometryID = _library->registerAsset(geoPath, AssetType::Geometry, true);
	AssetID materialID = _library->registerAsset(materialPath, AssetType::Material, true);
	AssetID imageAlbedoID = _library->registerAsset(imageAlbedoPath, AssetType::Image, true);
	AssetID imageNormalID = _library->registerAsset(imageNormalPath, AssetType::Image, true);

	ArchiveGeometry geometry(geometryID);
	// indices
	geometry.indices.append(0);
	geometry.indices.append(1);
	geometry.indices.append(3);
	geometry.indices.append(3);
	geometry.indices.append(1);
	geometry.indices.append(2);
	// Vertices
	static const StaticVertex s_planeVertices[4] = {
		StaticVertex{_transform.multiplyPoint(point3f(-1.0f, -1.0f, 0.0f)),	norm3f(_transform.multiplyVector(vec3f(0.f, 0.f, 1.f))), uv2f(0.f, 0.f), color4f(1.f, 1.f, 1.f, 1.f)},
		StaticVertex{_transform.multiplyPoint(point3f(-1.0f,  1.0f, 0.0f)),	norm3f(_transform.multiplyVector(vec3f(0.f, 0.f, 1.f))), uv2f(0.f, 1.f), color4f(1.f, 1.f, 1.f, 1.f)},
		StaticVertex{_transform.multiplyPoint(point3f(1.0f,  1.0f, 0.0f)),	norm3f(_transform.multiplyVector(vec3f(0.f, 0.f, 1.f))), uv2f(1.f, 1.f), color4f(1.f, 1.f, 1.f, 1.f)},
		StaticVertex{_transform.multiplyPoint(point3f(1.0f, -1.0f, 0.0f)),	norm3f(_transform.multiplyVector(vec3f(0.f, 0.f, 1.f))), uv2f(1.f, 0.f), color4f(1.f, 1.f, 1.f, 1.f)},
	};
	geometry.staticVertices.resize(4);
	Memory::copy(geometry.staticVertices.data(), s_planeVertices, sizeof(s_planeVertices));
	geometry.bounds = aabbox(point3f(-1.f), point3f(1.f));

	ArchiveBatch batch(batchID);
	batch.geometry = geometryID;
	batch.material = materialID;

	// Material
	ArchiveMaterial material(materialID);
	material.color = color4f(1.0, 1.0, 1.0, 1.0);
	material.albedo = imageAlbedoID;
	material.normal = imageNormalID;

	ArchiveImage albedo(imageAlbedoID);
	albedo = ArchiveImage(imageAlbedoID);
	albedo.width = 1;
	albedo.height = 1;
	albedo.channels = 4;
	albedo.data.append(127);
	albedo.data.append(127);
	albedo.data.append(127);
	albedo.data.append(255);

	ArchiveImage normal(imageNormalID);
	normal = ArchiveImage(imageNormalID);
	normal.width = 1;
	normal.height = 1;
	normal.channels = 4;
	normal.data.append(127);
	normal.data.append(127);
	normal.data.append(255);
	normal.data.append(255);

	ArchiveStaticMesh cubeMesh(meshID);
	cubeMesh.batches.append(batchID);
	ArchiveSaveContext ctx(cubeMesh, _library);
	ArchiveParseResult res = cubeMesh.save(ctx);
	res = batch.save(ctx);
	res = geometry.save(ctx);
	res = material.save(ctx);
	res = albedo.save(ctx);
	res = normal.save(ctx);

	ArchiveLoadContext loadCtx(cubeMesh, _library);
	cubeMesh.load(loadCtx);
	_library->load<StaticMesh>(meshID, loadCtx, _renderer);
	return meshID;
}

void Editor::onCreate(int argc, char* argv[])
{
	assets()->parse();

	AssetID sceneID = assets()->registerAsset(AssetPath("xpbd/scene.sce"), AssetType::Scene);

	ArchiveScene archive(sceneID);
	archive.bounds = aabbox<>(point3f(-5.f), point3f(5.f));

	ArchiveSaveContext ctx(archive, assets());
	archive.save(ctx);
	ArchiveLoadContext loadCtx(archive, assets());
	archive.load(loadCtx);

	m_scene = assets()->load<Scene>(sceneID, renderer());


	if (m_scene.isLoaded())
	{
		Scene& scene = m_scene.get();
		m_sceneID = scene.getID();
		// Setup editor camera.
		m_editorCameraNode = scene.createChild(nullptr, "EditorCamera");
		{
			CameraComponent& camera = m_editorCameraNode->attach<CameraComponent>();
			ArchiveCameraComponent* component = camera.createArchive();
			component->projectionType = CameraProjectionType::Perpective;
			const aabbox<>& bounds = scene.getBounds();
			camera.fromArchive(*component);
			camera.setNear(0.1f);
			camera.setFar(bounds.extent().norm() * 50.f);
			scene.setMainCameraNode(m_editorCameraNode);
			camera.destroyArchive(component);
		}
		{
			ArcballComponent& controller = m_editorCameraNode->attach<ArcballComponent>();
			ArchiveArcballComponent* component = controller.createArchive();
			const aabbox<>& bounds = scene.getBounds();
			controller.fromArchive(*component);
			controller.setBounds(bounds);
			controller.destroyArchive(component);
		}
		
		{
			Node* rigidBodyNode = scene.createChild(nullptr, "RigidBody");
			RigidBodyComponent& component = rigidBodyNode->attach<RigidBodyComponent>();
		}
		{
			Node* child = scene.createChild(nullptr, "Plane");
			StaticMeshComponent& mesh = child->attach<StaticMeshComponent>();
			ArchiveStaticMeshComponent archive(0);
			archive.assetID = createPlaneMesh(assets(), renderer(), mat4f::TRS(vec3f(0.f), quatf::euler(anglef::degree(-90.f), anglef::degree(0.f), anglef::degree(0.f)), vec3f(100.f)));
			mesh.fromArchive(archive);

			child->setLocalTransform(mat4f::identity());

		}

	}
}

void Editor::onDestroy()
{
}

void Editor::onFixedUpdate(aka::Time time)
{
	if (m_scene.isLoaded())
	{
		m_scene.get().fixedUpdate(time);
	}
}

void Editor::onUpdate(aka::Time time)
{
	if (platform()->keyboard().down(KeyboardKey::Escape))
		EventDispatcher<QuitEvent>::emit();

	if (m_scene.isLoaded())
	{
		// Disable camera inputs if ImGui uses them.
		const ImGuiIO& io = ImGui::GetIO();
		ArcballComponent& camera = m_editorCameraNode->get<ArcballComponent>();
		camera.setUpdateEnabled(!io.WantCaptureMouse && !io.WantCaptureKeyboard);

		renderer()->getDebugDrawList().draw3DCube(mat4f::identity(), color4f(0.f, 0.f, 0.f, 1.f));

		// Update scene
		m_scene.get().update(time);
	}
}


void Editor::onRender(Renderer* _renderer, gfx::FrameHandle _frame)
{
	if (m_scene.isLoaded())
	{
		m_scene.get().update(assets(), _renderer);
	}
}

void Editor::onResize(uint32_t width, uint32_t height)
{
}

void Editor::onReceive(const app::SceneSwitchEvent& event)
{
	m_scene = event.scene;
	if (event.scene.isLoaded())
	{
		Scene& scene = m_scene.get();
		m_sceneID = scene.getID();
		// Setup editor camera.
		m_editorCameraNode = scene.createChild(nullptr, "EditorCamera");
		{
			CameraComponent& camera = m_editorCameraNode->attach<CameraComponent>();
			ArchiveCameraComponent* component = camera.createArchive();
			component->projectionType = CameraProjectionType::Perpective;
			const aabbox<>& bounds = scene.getBounds();
			camera.fromArchive(*component);
			camera.setNear(0.1f);
			camera.setFar(bounds.extent().norm() * 2.f);
			scene.setMainCameraNode(m_editorCameraNode);
			camera.destroyArchive(component);
		}
		{
			ArcballComponent& controller = m_editorCameraNode->attach<ArcballComponent>();
			ArchiveArcballComponent* component = controller.createArchive();
			const aabbox<>& bounds = scene.getBounds();
			controller.fromArchive(*component);
			controller.setBounds(bounds);
			controller.destroyArchive(component);
		}
	}
}

