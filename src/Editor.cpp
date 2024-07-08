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

Vector<ArchiveStaticVertex> getSphereVertices(float radius, uint32_t segmentCount, uint32_t ringCount)
{
	// http://www.songho.ca/opengl/gl_sphere.html
	Vector<ArchiveStaticVertex> vertices;
	vertices.reserve((segmentCount + 1) * (ringCount + 1));

	float length = 1.f / radius;
	anglef sectorStep = 2.f * pi<float> / (float)ringCount;
	anglef stackStep = pi<float> / (float)segmentCount;
	anglef ringAngle, segmentAngle;
	aabbox<> bounds;

	for (uint32_t i = 0; i <= segmentCount; ++i)
	{
		segmentAngle = pi<float> / 2.f - (float)i * stackStep; // starting from pi/2 to -pi/2
		float xy = radius * cos(segmentAngle); // r * cos(u)
		float z = radius * sin(segmentAngle); // r * sin(u)

		// add (ringCount+1) vertices per segment
		// the first and last vertices have same position and normal, but different uv
		for (uint32_t j = 0; j <= ringCount; ++j)
		{
			ArchiveStaticVertex& v = vertices.emplace();
			ringAngle = (float)j * sectorStep; // starting from 0 to 2pi

			v.position[0] = xy * cos(ringAngle); // r * cos(u) * cos(v)
			v.position[1] = xy * sin(ringAngle); // r * cos(u) * sin(v)
			v.position[2] = z;

			for (uint32_t k = 0; k < 3; ++k)
				v.normal[k] = v.position[k] / radius;

			v.uv[0] = (float)j / ringCount;
			v.uv[1] = (float)i / segmentCount;
			v.color[0] = 1.f;
			v.color[1] = 1.f;
			v.color[2] = 1.f;
			v.color[3] = 1.f;
			bounds.include(v.position[0], v.position[1], v.position[2]);
		}
	}
	return vertices;
}

Vector<uint32_t> getSphereIndices(float radius, uint32_t segmentCount, uint32_t ringCount)
{
	Vector<uint32_t> indices;
	for (uint32_t i = 0; i < segmentCount; ++i)
	{
		uint32_t k1 = i * (ringCount + 1);     // beginning of current stack
		uint32_t k2 = k1 + ringCount + 1;      // beginning of next stack

		for (uint32_t j = 0; j < ringCount; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding first and last stacks
			// k1 => k2 => k1+1
			if (i != 0)
			{
				indices.append(k1);
				indices.append(k2);
				indices.append(k1 + 1);
			}
			// k1+1 => k2 => k2+1
			if (i != (segmentCount - 1))
			{
				indices.append(k1 + 1);
				indices.append(k2);
				indices.append(k2 + 1);
			}
		}
	}
	return indices;
}

AssetID createSphereMesh(AssetLibrary* _library, Renderer* _renderer, mat4f _transform)
{
	AssetPath meshPath = AssetPath("shapes/sphere/sphere.smesh", AssetPathType::Cooked);
	AssetPath batchPath = AssetPath("shapes/sphere/sphere.batch", AssetPathType::Cooked);
	AssetPath geoPath = AssetPath("shapes/sphere/sphere.geo", AssetPathType::Cooked);
	AssetPath materialPath = AssetPath("shapes/sphere/sphere.mat", AssetPathType::Cooked);
	AssetPath imageAlbedoPath = AssetPath("shapes/sphere/albedo.img", AssetPathType::Cooked);
	AssetPath imageNormalPath = AssetPath("shapes/sphere/normal.img", AssetPathType::Cooked);
	OS::Directory::create(AssetPath("shapes/sphere/", AssetPathType::Cooked).getAbsolutePath());

	// Add to library & load it.
	AssetID meshID = _library->registerAsset(meshPath, AssetType::StaticMesh);
	AssetID batchID = _library->registerAsset(batchPath, AssetType::Batch);
	AssetID geometryID = _library->registerAsset(geoPath, AssetType::Geometry);
	AssetID materialID = _library->registerAsset(materialPath, AssetType::Material);
	AssetID imageAlbedoID = _library->registerAsset(imageAlbedoPath, AssetType::Image);
	AssetID imageNormalID = _library->registerAsset(imageNormalPath, AssetType::Image);

	ArchiveGeometry geometry(geometryID);
	geometry.indices = getSphereIndices(1.0, 32, 16);
	geometry.staticVertices = getSphereVertices(1.0, 32, 16);
	for (uint32_t i = 0; i < geometry.staticVertices.size(); i++)
		geometry.bounds.include(geometry.staticVertices[i].position[0], geometry.staticVertices[i].position[1], geometry.staticVertices[i].position[2]);

	ArchiveBatch batch(batchID);
	batch.geometry = geometryID;
	batch.material = materialID;
	// Material
	ArchiveMaterial material(materialID);
	material.color = color4f(0.0, 0.0, 1.0, 1.0);
	material.albedo = imageAlbedoID;
	material.normal = imageNormalID;

	ArchiveImage albedo(imageAlbedoID);
	Image img = ImageDecoder::fromDisk("./asset/textures/skyscraper.jpg");
	AKA_ASSERT(img.width > 0 && img.height > 0, "Invalid image");
	albedo = ArchiveImage(imageAlbedoID);
	albedo.width = img.width;
	albedo.height = img.height;
	albedo.channels = img.getComponents();
	albedo.data = std::move(img.bytes);

	ArchiveImage normal(imageNormalID);
	Image imgNormal = ImageDecoder::fromDisk("./asset/textures/skyscraper-normal.jpg");
	AKA_ASSERT(imgNormal.width > 0 && imgNormal.height > 0, "Invalid image");
	normal = ArchiveImage(imageNormalID);
	normal.width = imgNormal.width;
	normal.height = imgNormal.height;
	normal.channels = imgNormal.getComponents();
	normal.data = std::move(imgNormal.bytes);

	ArchiveStaticMesh sphereMesh(meshID);
	sphereMesh.batches.append(batchID);
	ArchiveSaveContext ctx(sphereMesh, _library);
	ArchiveParseResult res;
	res = sphereMesh.save(ctx);
	res = batch.save(ctx);
	res = geometry.save(ctx);
	res = material.save(ctx);
	res = albedo.save(ctx);
	res = normal.save(ctx);

	ArchiveLoadContext loadCtx(sphereMesh, _library);
	sphereMesh.load(loadCtx);
	_library->load<StaticMesh>(meshID, loadCtx, _renderer);
	return meshID;
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
		StaticVertex{_transform.multiplyPoint(point3f(1.0f,   1.0f, 0.0f)),	norm3f(_transform.multiplyVector(vec3f(0.f, 0.f, 1.f))), uv2f(1.f, 1.f), color4f(1.f, 1.f, 1.f, 1.f)},
		StaticVertex{_transform.multiplyPoint(point3f(1.0f,  -1.0f, 0.0f)),	norm3f(_transform.multiplyVector(vec3f(0.f, 0.f, 1.f))), uv2f(1.f, 0.f), color4f(1.f, 1.f, 1.f, 1.f)},
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
	archive.bounds = aabbox<>(point3f(-10.f), point3f(20.f));

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
		
		static const point3f s_positions[] = {
			point3f(0.f, 0.f, 10.f),
			point3f(0.f, 1.f, 12.f),
			point3f(1.f, 2.f, 10.f),
			point3f(1.f, 0.f, 8.f),
		};
		AssetID sphereMesh = createSphereMesh(assets(), renderer(), mat4f::identity());
		for (uint32_t iSphere = 0; iSphere < countof(s_positions); iSphere++)
		{
			Node* sphereNode = scene.createChild(nullptr, String::format("Sphere%u", iSphere).cstr());
			sphereNode->setLocalTransform(mat4f::translate(s_positions[iSphere]));
			{
				StaticMeshComponent& mesh = sphereNode->attach<StaticMeshComponent>();
				ArchiveStaticMeshComponent archive(0);
				archive.assetID = sphereMesh;
				mesh.fromArchive(archive);
			}
			{
				RigidBodyComponent& rb = sphereNode->attach<RigidBodyComponent>();
				ArchiveRigidBodyComponent archive(0);
				archive.mass = 1.f;
				rb.fromArchive(archive);
			}

			{
				ColliderComponent& rb = sphereNode->attach<ColliderComponent>();
				ArchiveColliderComponent archive(0);
				archive.shape = ColliderShapeType::Sphere;
				rb.fromArchive(archive);
			}
		}
		{
			Node* child = scene.createChild(nullptr, "Plane");
			child->setLocalTransform(mat4f::TRS(point3f(0.f), quatf::identity(), vec3f(100.f)));
			{
				StaticMeshComponent& mesh = child->attach<StaticMeshComponent>();
				ArchiveStaticMeshComponent archive(0);
				archive.assetID = createPlaneMesh(assets(), renderer(), mat4f::identity());
				mesh.fromArchive(archive);
			}
			{
				ColliderComponent& collider = child->attach<ColliderComponent>();
				ArchiveColliderComponent archive(0);
				archive.shape = ColliderShapeType::Plane;
				collider.fromArchive(archive);
			}

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

