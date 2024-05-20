#include "SceneEditorLayer.hpp"

#include <Aka/Layer/ImGuiLayer.h>
#include <Aka/Scene/Component/CameraComponent.hpp>
#include <Aka/Scene/Component/ArcballComponent.hpp>
#include <Aka/Scene/Component/SkeletalMeshComponent.hpp>
#include <Aka/Scene/Component/StaticMeshComponent.hpp>
#include <Aka/Scene/Component/RigidBodyComponent.hpp>
#include <Aka/Scene/Component/ColliderComponent.hpp>
#include <Aka/Renderer/DebugDraw/DebugDrawList.hpp>
#include <Aka/Resource/AssetLibrary.hpp>
#include <Aka/Resource/Archive/ArchiveBatch.hpp>
#include <Aka/Resource/Archive/ArchiveGeometry.hpp>

#include "../Component/CustomComponent.hpp"
#include "../Component/RotatorComponent.hpp"

namespace app {

using namespace aka;

static const uint32_t s_vertexCount = 36;

static const StaticVertex s_vertices[s_vertexCount] = {
	StaticVertex{point3f(-1.0f,  1.0f, -1.0f),	norm3f(0.f, 0.f, -1.f), uv2f(0.f, 1.f), color4f(1.f, 1.f, 1.f, 1.f)},
	StaticVertex{point3f(-1.0f, -1.0f, -1.0f),	norm3f(0.f, 0.f, -1.f), uv2f(0.f, 0.f), color4f(1.f, 1.f, 1.f, 1.f)},
	StaticVertex{point3f( 1.0f, -1.0f, -1.0f),	norm3f(0.f, 0.f, -1.f), uv2f(1.f, 0.f), color4f(1.f, 1.f, 1.f, 1.f)},
	StaticVertex{point3f( 1.0f, -1.0f, -1.0f),	norm3f(0.f, 0.f, -1.f), uv2f(1.f, 0.f), color4f(1.f, 1.f, 1.f, 1.f)},
	StaticVertex{point3f( 1.0f,  1.0f, -1.0f),	norm3f(0.f, 0.f, -1.f), uv2f(1.f, 1.f), color4f(1.f, 1.f, 1.f, 1.f)},
	StaticVertex{point3f(-1.0f,  1.0f, -1.0f),	norm3f(0.f, 0.f, -1.f), uv2f(0.f, 1.f), color4f(1.f, 1.f, 1.f, 1.f)},

	StaticVertex{point3f(-1.0f, -1.0f,  1.0f),	norm3f(-1.f, 0.f, 0.f), uv2f(0.f, 1.f), color4f(1.f, 1.f, 1.f, 1.f)},
	StaticVertex{point3f(-1.0f, -1.0f, -1.0f),	norm3f(-1.f, 0.f, 0.f), uv2f(0.f, 0.f), color4f(1.f, 1.f, 1.f, 1.f)},
	StaticVertex{point3f(-1.0f,  1.0f, -1.0f),	norm3f(-1.f, 0.f, 0.f), uv2f(1.f, 0.f), color4f(1.f, 1.f, 1.f, 1.f)},
	StaticVertex{point3f(-1.0f,  1.0f, -1.0f),	norm3f(-1.f, 0.f, 0.f), uv2f(1.f, 0.f), color4f(1.f, 1.f, 1.f, 1.f)},
	StaticVertex{point3f(-1.0f,  1.0f,  1.0f),	norm3f(-1.f, 0.f, 0.f), uv2f(1.f, 1.f), color4f(1.f, 1.f, 1.f, 1.f)},
	StaticVertex{point3f(-1.0f, -1.0f,  1.0f),	norm3f(-1.f, 0.f, 0.f), uv2f(0.f, 1.f), color4f(1.f, 1.f, 1.f, 1.f)},

	StaticVertex{point3f( 1.0f, -1.0f, -1.0f),	norm3f(1.f, 0.f, 0.f), uv2f(0.f, 0.f), color4f(1.f, 1.f, 1.f, 1.f)},
	StaticVertex{point3f( 1.0f, -1.0f,  1.0f),	norm3f(1.f, 0.f, 0.f), uv2f(0.f, 1.f), color4f(1.f, 1.f, 1.f, 1.f)},
	StaticVertex{point3f( 1.0f,  1.0f,  1.0f),	norm3f(1.f, 0.f, 0.f), uv2f(1.f, 1.f), color4f(1.f, 1.f, 1.f, 1.f)},
	StaticVertex{point3f( 1.0f,  1.0f,  1.0f),	norm3f(1.f, 0.f, 0.f), uv2f(1.f, 1.f), color4f(1.f, 1.f, 1.f, 1.f)},
	StaticVertex{point3f( 1.0f,  1.0f, -1.0f),	norm3f(1.f, 0.f, 0.f), uv2f(1.f, 0.f), color4f(1.f, 1.f, 1.f, 1.f)},
	StaticVertex{point3f( 1.0f, -1.0f, -1.0f),	norm3f(1.f, 0.f, 0.f), uv2f(0.f, 0.f), color4f(1.f, 1.f, 1.f, 1.f)},

	StaticVertex{point3f(-1.0f, -1.0f,  1.0f),	norm3f(0.f, 0.f, 1.f), uv2f(0.f, 0.f), color4f(1.f, 1.f, 1.f, 1.f)},
	StaticVertex{point3f(-1.0f,  1.0f,  1.0f),	norm3f(0.f, 0.f, 1.f), uv2f(0.f, 1.f), color4f(1.f, 1.f, 1.f, 1.f)},
	StaticVertex{point3f( 1.0f,  1.0f,  1.0f),	norm3f(0.f, 0.f, 1.f), uv2f(1.f, 1.f), color4f(1.f, 1.f, 1.f, 1.f)},
	StaticVertex{point3f( 1.0f,  1.0f,  1.0f),	norm3f(0.f, 0.f, 1.f), uv2f(1.f, 1.f), color4f(1.f, 1.f, 1.f, 1.f)},
	StaticVertex{point3f( 1.0f, -1.0f,  1.0f),	norm3f(0.f, 0.f, 1.f), uv2f(1.f, 0.f), color4f(1.f, 1.f, 1.f, 1.f)},
	StaticVertex{point3f(-1.0f, -1.0f,  1.0f),	norm3f(0.f, 0.f, 1.f), uv2f(0.f, 0.f), color4f(1.f, 1.f, 1.f, 1.f)},

	StaticVertex{point3f(-1.0f,  1.0f, -1.0f),	norm3f(0.f, 1.f, 0.f), uv2f(0.f, 0.f), color4f(1.f, 1.f, 1.f, 1.f)},
	StaticVertex{point3f( 1.0f,  1.0f, -1.0f),	norm3f(0.f, 1.f, 0.f), uv2f(1.f, 0.f), color4f(1.f, 1.f, 1.f, 1.f)},
	StaticVertex{point3f( 1.0f,  1.0f,  1.0f),	norm3f(0.f, 1.f, 0.f), uv2f(1.f, 1.f), color4f(1.f, 1.f, 1.f, 1.f)},
	StaticVertex{point3f( 1.0f,  1.0f,  1.0f),	norm3f(0.f, 1.f, 0.f), uv2f(1.f, 1.f), color4f(1.f, 1.f, 1.f, 1.f)},
	StaticVertex{point3f(-1.0f,  1.0f,  1.0f),	norm3f(0.f, 1.f, 0.f), uv2f(0.f, 1.f), color4f(1.f, 1.f, 1.f, 1.f)},
	StaticVertex{point3f(-1.0f,  1.0f, -1.0f),	norm3f(0.f, 1.f, 0.f), uv2f(0.f, 0.f), color4f(1.f, 1.f, 1.f, 1.f)},

	StaticVertex{point3f(-1.0f, -1.0f, -1.0f),	norm3f(0.f, -1.f, 0.f), uv2f(0.f, 0.f), color4f(1.f, 1.f, 1.f, 1.f)},
	StaticVertex{point3f(-1.0f, -1.0f,  1.0f),	norm3f(0.f, -1.f, 0.f), uv2f(0.f, 1.f), color4f(1.f, 1.f, 1.f, 1.f)},
	StaticVertex{point3f( 1.0f, -1.0f, -1.0f),	norm3f(0.f, -1.f, 0.f), uv2f(1.f, 0.f), color4f(1.f, 1.f, 1.f, 1.f)},
	StaticVertex{point3f( 1.0f, -1.0f, -1.0f),	norm3f(0.f, -1.f, 0.f), uv2f(1.f, 0.f), color4f(1.f, 1.f, 1.f, 1.f)},
	StaticVertex{point3f(-1.0f, -1.0f,  1.0f),	norm3f(0.f, -1.f, 0.f), uv2f(0.f, 1.f), color4f(1.f, 1.f, 1.f, 1.f)},
	StaticVertex{point3f( 1.0f, -1.0f,  1.0f),	norm3f(0.f, -1.f, 0.f), uv2f(1.f, 1.f), color4f(1.f, 1.f, 1.f, 1.f)},
};

Vector<ArchiveStaticVertex> getSphereVertices(float radius, uint32_t segmentCount, uint32_t ringCount)
{
	// http://www.songho.ca/opengl/gl_sphere.html
	Vector<ArchiveStaticVertex> vertices;

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
			ArchiveStaticVertex v;
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
			vertices.append(v);
			bounds.include(point3f(v.position[0], v.position[1], v.position[2]));
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

AssetID createSphereMesh(AssetLibrary* _library, Renderer* _renderer)
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
		geometry.bounds.include(point3f(geometry.staticVertices[i].position[0], geometry.staticVertices[i].position[1], geometry.staticVertices[i].position[2]));

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

AssetID createCubeMesh(AssetLibrary* _library, Renderer* _renderer)
{
	AssetPath meshPath = AssetPath("shapes/cube/cube.smesh", AssetPathType::Cooked);
	AssetPath batchPath = AssetPath("shapes/cube/cube.batch", AssetPathType::Cooked);
	AssetPath geoPath = AssetPath("shapes/cube/cube.geo", AssetPathType::Cooked);
	AssetPath materialPath = AssetPath("shapes/cube/cube.mat", AssetPathType::Cooked);
	AssetPath imageAlbedoPath = AssetPath("shapes/cube/albedo.img", AssetPathType::Cooked);
	AssetPath imageNormalPath = AssetPath("shapes/cube/normal.img", AssetPathType::Cooked);
	OS::Directory::create(AssetPath("shapes/cube/", AssetPathType::Cooked).getAbsolutePath());

	// Add to library & load it.
	AssetID meshID = _library->registerAsset(meshPath, AssetType::StaticMesh);
	AssetID batchID = _library->registerAsset(batchPath, AssetType::Batch);
	AssetID geometryID = _library->registerAsset(geoPath, AssetType::Geometry);
	AssetID materialID = _library->registerAsset(materialPath, AssetType::Material);
	AssetID imageAlbedoID = _library->registerAsset(imageAlbedoPath, AssetType::Image);
	AssetID imageNormalID = _library->registerAsset(imageNormalPath, AssetType::Image);

	ArchiveGeometry geometry(geometryID);
	// indices
	geometry.indices.resize(s_vertexCount);
	for (uint32_t i = 0; i < s_vertexCount; i++)
		geometry.indices[i] = i;
	// Vertices
	geometry.staticVertices.resize(s_vertexCount);
	Memory::copy(geometry.staticVertices.data(), s_vertices, sizeof(s_vertices));
	geometry.bounds = aabbox(point3f(-1.f), point3f(1.f));

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


AssetID createPlaneMesh(AssetLibrary* _library, Renderer* _renderer)
{
	AssetPath meshPath = AssetPath("shapes/plane/plane.smesh", AssetPathType::Cooked);
	AssetPath batchPath = AssetPath("shapes/plane/plane.batch", AssetPathType::Cooked);
	AssetPath geoPath = AssetPath("shapes/plane/plane.geo", AssetPathType::Cooked);
	AssetPath materialPath = AssetPath("shapes/plane/plane.mat", AssetPathType::Cooked);
	AssetPath imageAlbedoPath = AssetPath("shapes/plane/albedo.img", AssetPathType::Cooked);
	AssetPath imageNormalPath = AssetPath("shapes/plane/normal.img", AssetPathType::Cooked);
	OS::Directory::create(AssetPath("shapes/plane/", AssetPathType::Cooked).getAbsolutePath());

	// Add to library & load it.
	AssetID meshID = _library->registerAsset(meshPath, AssetType::StaticMesh);
	AssetID batchID = _library->registerAsset(batchPath, AssetType::Batch);
	AssetID geometryID = _library->registerAsset(geoPath, AssetType::Geometry);
	AssetID materialID = _library->registerAsset(materialPath, AssetType::Material);
	AssetID imageAlbedoID = _library->registerAsset(imageAlbedoPath, AssetType::Image);
	AssetID imageNormalID = _library->registerAsset(imageNormalPath, AssetType::Image);

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
		StaticVertex{point3f(-1.0f, -1.0f, 0.0f),	norm3f(0.f, 1.f, 0.f), uv2f(0.f, 0.f), color4f(1.f, 1.f, 1.f, 1.f)},
		StaticVertex{point3f(-1.0f,  1.0f, 0.0f),	norm3f(0.f, 1.f, 0.f), uv2f(0.f, 1.f), color4f(1.f, 1.f, 1.f, 1.f)},
		StaticVertex{point3f( 1.0f,  1.0f, 0.0f),	norm3f(0.f, 1.f, 0.f), uv2f(1.f, 1.f), color4f(1.f, 1.f, 1.f, 1.f)},
		StaticVertex{point3f( 1.0f, -1.0f, 0.0f),	norm3f(0.f, 1.f, 0.f), uv2f(1.f, 0.f), color4f(1.f, 1.f, 1.f, 1.f)},
	};
	geometry.staticVertices.resize(4);
	Memory::copy(geometry.staticVertices.data(), s_planeVertices, sizeof(s_planeVertices));
	geometry.bounds = aabbox(point3f(-1.f), point3f(1.f));

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

SceneEditorLayer::SceneEditorLayer() :
	EditorLayer("Scene Editor")
{
}

void SceneEditorLayer::onCreate(aka::Renderer* _renderer)
{
}

void SceneEditorLayer::onDestroy(aka::Renderer* _renderer)
{
}

void SceneEditorLayer::onUpdate(aka::Time deltaTime)
{
	const Keyboard& keyboard = Application::app()->platform()->keyboard();
	if (keyboard.down(KeyboardKey::T))
		m_gizmoOperation = ImGuizmo::TRANSLATE;
	else if (keyboard.down(KeyboardKey::R))
		m_gizmoOperation = ImGuizmo::ROTATE;
	else if (keyboard.down(KeyboardKey::S))
		m_gizmoOperation = ImGuizmo::SCALE;
}

void SceneEditorLayer::onPreRender()
{
	if (m_assetToUnload != aka::AssetID::Invalid)
	{
		// Should have some delay for frames in flight (gfx::MaxFrameInFlight).
		m_library->unload<Scene>(m_assetToUnload, Application::app()->renderer());
		m_assetToUnload = aka::AssetID::Invalid;
	}
	if (m_nodeToDestroy)
	{
		m_nodeToDestroy->unlink();
		m_nodeToDestroy->destroy(m_library, Application::app()->renderer());
		m_scene.get().destroyChild(m_nodeToDestroy);
		m_nodeToDestroy = nullptr;
		m_currentNode = nullptr;
	}
}

template <typename T>
struct ComponentNode 
{
	static const char* name() { return "Unknown"; }
	//static const char* icon() { return ""; }
	static bool draw(AssetLibrary* library, DebugDrawList& debugDrawList, T& component) { AKA_ASSERT(false, "Trying to draw an undefined component"); return false; }
};

template <> const char* ComponentNode<StaticMeshComponent>::name() { return "StaticMeshComponent"; }
template <> bool ComponentNode<StaticMeshComponent>::draw(AssetLibrary* library, DebugDrawList& debugDrawList, StaticMeshComponent& mesh)
{
	if (mesh.getMesh().isLoaded())
	{
		StaticMesh& m = mesh.getMesh().get();

		AssetID currentAssetID = mesh.getMesh().get().getID();
		const String& name = mesh.getMesh().get().getName();
		if (ImGui::TreeNode("Batches"))
		{
			for (size_t iBatch = 0; iBatch < m.getBatches().size(); iBatch++)
			{
				const StaticMeshBatch& batch = m.getBatches()[iBatch];
				if (ImGui::TreeNode(&batch, "Batch (%zu/%zu)", iBatch + 1, m.getBatches().size()))
				{
					batch.indexOffset;
					batch.vertexOffset;
					ImGui::Text("%u indices", batch.indexCount);

					if (ImGui::TreeNode("Material"))
					{
						if (batch.material.isLoaded())
						{
							const Material& material = batch.material.get();
							ImGui::Text("Material: %s", material.getName().cstr());
							material.getAlbedoTexture();
							material.getNormalTexture();
						}
						else
						{
							ImGui::Text("Material not loaded");
						}
						ImGui::TreePop();
					}
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}

		aabbox<> bbox = mesh.getWorldBounds();
		debugDrawList.draw3DCube(mat4f::TRS(point3f(bbox.center()), quatf::identity(), bbox.extent() * 0.5f), color4f(1.f, 1.f, 1.f, 1.f));
		

		// TODO: button to open viewer somehow.
	}
	else
	{
		ImGui::Text("No mesh data");
	}
	return false;
}

template <> const char* ComponentNode<RigidBodyComponent>::name() { return "RigidBodyComponent"; }
template <> bool ComponentNode<RigidBodyComponent>::draw(AssetLibrary* library, DebugDrawList& debugDrawList, RigidBodyComponent& rigidBody)
{
	//const vec3f& velocity = rigidBody.getVelocity();
	//ImGui::Text("Velocity(%f, %f, %f)", velocity.x, velocity.y, velocity.z);
	return false;
}

template <> const char* ComponentNode<ColliderComponent>::name() { return "ColliderComponent"; }
template <> bool ComponentNode<ColliderComponent>::draw(AssetLibrary* library, DebugDrawList& debugDrawList, ColliderComponent& collider)
{
	return false;
}

template <> const char* ComponentNode<SkeletalMeshComponent>::name() { return "SkeletalMeshComponent"; }
template <> bool ComponentNode<SkeletalMeshComponent>::draw(AssetLibrary* library, DebugDrawList& debugDrawList, SkeletalMeshComponent& meshComp)
{
	if (meshComp.getMesh().isLoaded())
	{
		SkeletalMesh& m = meshComp.getMesh().get();

		AssetID currentAssetID = m.getID();
		ImGui::Text("Mesh '%s'", m.getName().cstr());
		if (ImGui::TreeNode("Batches"))
		{
			for (size_t iBatch = 0; iBatch < m.getBatches().size(); iBatch++)
			{
				const SkeletalMeshBatch& batch = m.getBatches()[iBatch];
				if (ImGui::TreeNode(&batch, "Batch (%zu/%zu)", iBatch + 1, m.getBatches().size()))
				{
					batch.indexOffset;
					batch.vertexOffset;
					batch.boneOffset;
					ImGui::Text("%u indices", batch.indexCount);

					if (ImGui::TreeNode("Material"))
					{
						if (batch.material.isLoaded())
						{
							const Material& material = batch.material.get();
							ImGui::Text("Material: %s", material.getName().cstr());
							material.getAlbedoTexture();
							material.getNormalTexture();
						}
						else
						{
							ImGui::Text("Material not loaded");
						}
						ImGui::TreePop();
					}
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Animations"))
		{
			for (size_t iAnim = 0; iAnim < meshComp.getAnimations().size(); iAnim++)
			{
				const SkeletalMeshAnimation& animation = meshComp.getAnimations()[iAnim];
				if (ImGui::TreeNode(&animation, "%s (%zu/%zu)", animation.name.cstr(), iAnim + 1, meshComp.getAnimations().size()))
				{
					ImGui::Text("Duration:  %.3f ticks", animation.durationInTick);
					ImGui::Text("Ticks:	    %.3f /s", animation.tickPerSecond);
					for (size_t iBone = 0; iBone < animation.bones.size(); iBone++)
					{
						const SkeletalMeshBoneAnimation& bone = animation.bones[iBone];
						if (ImGui::TreeNode(&bone, "%s (%zu/%zu)", meshComp.getBones()[iBone].name.cstr(), iBone + 1, animation.bones.size()))
						{
							bone.behaviour;
							float translation[3];
							float rotation[3];
							float scale[3];
							ImGuizmo::DecomposeMatrixToComponents(bone.localTransform.cols[0].data, translation, rotation, scale);
							ImGui::InputFloat3("Translation", translation);
							ImGui::InputFloat3("Rotation", rotation);
							ImGui::InputFloat3("Scale", scale);

							if (ImGui::TreeNode(&bone.positionKeys, "Positions keys (%zu)", bone.positionKeys.size()))
							{
								for (size_t iKey = 0; iKey < bone.positionKeys.size(); iKey++)
								{
									const SkeletalMeshKeyPosition& key = bone.positionKeys[iKey];
									if (ImGui::TreeNode(&key, "Position key (%zu/%zu)", iKey + 1, bone.positionKeys.size()))
									{
										ImGui::Text("Position : point3(%f, %f, %f)", key.position.x, key.position.y, key.position.z);
										ImGui::Text("Timestamp: %f ticks", key.timeStamp);
										ImGui::TreePop();
									}
								}
								ImGui::TreePop();
							}
							if (ImGui::TreeNode(&bone.rotationKeys, "Rotation keys (%zu)", bone.rotationKeys.size()))
							{
								for (size_t iKey = 0; iKey < bone.rotationKeys.size(); iKey++)
								{
									const SkeletalMeshKeyRotation& key = bone.rotationKeys[iKey];
									if (ImGui::TreeNode(&key, "Rotation key (%zu/%zu)", iKey + 1, bone.rotationKeys.size()))
									{
										ImGui::Text("Rotation : quat(%f, %f, %f)", key.orientation.x, key.orientation.y, key.orientation.z, key.orientation.w);
										ImGui::Text("Timestamp: %f ticks", key.timeStamp);
										ImGui::TreePop();
									}
								}
								ImGui::TreePop();
							}
							if (ImGui::TreeNode(&bone.scaleKeys, "Scale keys (%zu)", bone.scaleKeys.size()))
							{
								for (size_t iKey = 0; iKey < bone.scaleKeys.size(); iKey++)
								{
									const SkeletalMeshKeyScale& key = bone.scaleKeys[iKey];
									if (ImGui::TreeNode(&key, "Scale key (%zu/%zu)", iKey + 1, bone.scaleKeys.size()))
									{
										ImGui::Text("Scale : vec3(%f, %f, %f)", key.scale.x, key.scale.y, key.scale.z);
										ImGui::Text("Timestamp: %f ticks", key.timeStamp);
										ImGui::TreePop();
									}
								}
								ImGui::TreePop();
							}
							ImGui::TreePop();
						}
					}
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Bones offset"))
		{
			for (size_t iBone = 0; iBone < meshComp.getBones().size(); iBone++)
			{
				const SkeletalMeshBone& bone = meshComp.getBones()[iBone];
				if (ImGui::TreeNode(&bone, "%s (%zu/%zu)", bone.name.cstr(), iBone + 1, meshComp.getBones().size()))
				{
					if (bone.parentIndex == SkeletalVertex::InvalidBoneIndex)
						ImGui::Text("No parent");
					else
						ImGui::Text("Parent: '%s' (%u)", meshComp.getBones()[bone.parentIndex].name.cstr(), bone.parentIndex);
					float translation[3];
					float rotation[3];
					float scale[3];
					ImGuizmo::DecomposeMatrixToComponents(bone.offset.cols[0].data, translation, rotation, scale);
					ImGui::InputFloat3("Translation", translation);
					ImGui::InputFloat3("Rotation", rotation);
					ImGui::InputFloat3("Scale", scale);
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}

		aabbox<> bbox = meshComp.getWorldBounds();
		debugDrawList.draw3DCube(mat4f::TRS(point3f(bbox.center()), quatf::identity(), bbox.extent() * 0.5f), color4f(1.f, 1.f, 1.f, 1.f));

		// TODO: button to open viewer somehow.
		// + combo to switch mesh
	}
	else
	{
		ImGui::Text("No mesh data");
	}
	return false;
}

bool drawPerspective(CameraPerspective* projection)
{
	bool updated = false;
	vec2 range(projection->nearZ, projection->farZ);
	if (ImGui::DragFloat2("Range", range.data, 0.1f, 0.f, FLT_MAX / INT_MAX))
	{
		projection->nearZ = min(range.x, range.y);
		projection->farZ = max(range.x, range.y);
		updated = true;
	}
	float hFov = projection->hFov.radian();
	if (ImGui::SliderAngle("Horizontal FOV", &hFov, 10.f, 160.f))
	{
		projection->hFov = anglef::radian(hFov);
		updated = true;
	}
	projection->ratio; // Depend on viewport. Should not be editable.
	return updated;
}
bool drawOrthographic(CameraOrthographic* projection)
{
	bool updated = false;
	vec2 range(projection->nearZ, projection->farZ);
	if (ImGui::DragFloat2("Range", range.data, 0.1f, 0.f, FLT_MAX / INT_MAX))
	{
		projection->nearZ = min(range.x, range.y);
		projection->farZ = max(range.x, range.y);
		updated = true;
	}
	projection->left;
	projection->right;
	projection->top;
	projection->bottom;
	return updated;
}

template <> const char* ComponentNode<CameraComponent>::name() { return "CameraComponent"; }
template <> bool ComponentNode<CameraComponent>::draw(AssetLibrary* library, DebugDrawList& debugDrawList, CameraComponent& camera)
{
	bool updated = false;
	if (CameraProjection* projection = camera.getProjection())
	{
		ImGui::Text("Projection");
		switch (projection->type())
		{
		case CameraProjectionType::Perpective:
			updated |= drawPerspective(reinterpret_cast<CameraPerspective*>(projection));
			break;
		case CameraProjectionType::Orthographic:
			updated |= drawOrthographic(reinterpret_cast<CameraOrthographic*>(projection));
			break;
		}
	}
	debugDrawList.draw3DFrustum(camera.getProjectionMatrix() * camera.getViewMatrix(), color4f(1.f, 0.f, 0.f, 1.f));
	return updated;
}

template <> const char* ComponentNode<ArcballComponent>::name() { return "ArcballComponent"; }
template <> bool ComponentNode<ArcballComponent>::draw(AssetLibrary* library, DebugDrawList& debugDrawList, ArcballComponent& controller)
{
	bool updated = false;
	updated |= ImGui::InputFloat3("Position", controller.getController().position.data);
	updated |= ImGui::InputFloat3("Target", controller.getController().target.data);
	updated |= ImGui::InputFloat3("Up", controller.getController().up.data);
	updated |= ImGui::DragFloat("Speed", &controller.getController().speed);
	return updated;
}

template <> const char* ComponentNode<CustomComponent>::name() { return "CustomComponent"; }
template <> bool ComponentNode<CustomComponent>::draw(AssetLibrary* library, DebugDrawList& debugDrawList, CustomComponent& mesh)
{
	ImGui::Text(mesh.getCustomData().cstr());
	return false;
}

template <> const char* ComponentNode<RotatorComponent>::name() { return "RotatorComponent"; }
template <> bool ComponentNode<RotatorComponent>::draw(AssetLibrary* library, DebugDrawList& debugDrawList, RotatorComponent& mesh)
{
	ImGui::DragFloat("Speed", &mesh.getSpeed(), 1.f);
	ImGui::InputFloat3("Axis", mesh.getAxis().data);
	return false;
}

template <typename T>
void component(AssetLibrary* library, DebugDrawList& debugDrawList, Node* current)
{
	static char buffer[256];
	if (current->has<T>())
	{
		ImGui::Separator();
		T& component = current->get<T>();
		int res = snprintf(buffer, 256, "%s##%p", ComponentNode<T>::name(), &component);
		ImGui::PushStyleColor(ImGuiCol_Text, ImGuiLayer::Color::red);
		bool open = ImGui::TreeNodeEx(buffer, ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_DefaultOpen);
		ImGui::PopStyleColor();
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Remove"))
				current->detach<T>();
			ImGui::EndPopup();
		}
		if (open)
		{
			if (ComponentNode<T>::draw(library, debugDrawList, component))
			{
				current->setDirty<T>();
			}
			ImGui::TreePop();
		}
	}
}

void SceneEditorLayer::onRender(aka::Renderer* _renderer, aka::gfx::FrameHandle frame)
{
}

void SceneEditorLayer::onDrawUI(DebugDrawList& debugDrawList)
{
	std::function<void(Node* parent, Node*& current)> recurse;
	recurse = [&recurse, this](Node* parent, Node*& current)
		{
			char buffer[256];
			uint32_t childCount = parent->getChildCount();
			if (childCount > 0)
			{
				int err = snprintf(buffer, 256, "%s##%p", parent->getName().cstr(), parent);
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
				if (parent == current)
					flags |= ImGuiTreeNodeFlags_Selected;
				bool open = ImGui::TreeNodeEx(buffer, flags);
				if (ImGui::IsItemClicked())
					current = parent;
				if (ImGui::BeginPopupContextItem())
				{
					if (ImGui::MenuItem("Delete"))
						m_nodeToDestroy = parent;
					ImGui::EndPopup();
				}
				if (open)
				{
					for (uint32_t iChild = 0; iChild < childCount; iChild++)
						recurse(parent->getChild(iChild), current);
					ImGui::TreePop();
				}
			}
			else
			{
				bool isSelected = current == parent;
				snprintf(buffer, 256, "%s##%p", parent->getName().cstr(), parent);
				if (ImGui::Selectable(buffer, &isSelected))
					current = parent;
				if (ImGui::BeginPopupContextItem())
				{
					if (ImGui::MenuItem("Delete"))
						m_nodeToDestroy = parent;
					ImGui::EndPopup();
				}
			}
		};
	bool resetScene = false;
	bool newScene = false;
	const bool isLoaded = m_scene.isLoaded();
	{
		// --- Menu
		const bool isValid = m_currentNode != nullptr;
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New"))
				{
					newScene = true;
				}
				if (ImGui::MenuItem("Close"))
				{
					resetScene = true;
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Save", nullptr, nullptr, isLoaded))
				{
					if (isLoaded)
					{
						ArchiveScene archive(m_scene.get().getID());
						m_scene.get().toArchive(ArchiveSaveContext(archive, m_library), Application::app()->renderer());
						ArchiveParseResult res = archive.save(ArchiveSaveContext(archive, m_library));
						AKA_ASSERT(res == ArchiveParseResult::Success, "");
						// Also save library if meshes were added somehow.
						m_library->serialize();
					}
				}
				if (ImGui::MenuItem("Load"))
				{
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Node"))
			{
				if (ImGui::BeginMenu("Create"))
				{
					if (ImGui::BeginMenu("Mesh"))
					{
						if (ImGui::MenuItem("Cube", nullptr, nullptr, isLoaded && isValid))
						{
							ArchiveStaticMeshComponent archive(0);
							archive.assetID = createCubeMesh(m_library, Application::app()->renderer());
							m_currentNode = m_scene.get().createChild(m_currentNode, "Sphere node");
							m_currentNode->attach<StaticMeshComponent>().fromArchive(archive);
						}
						if (ImGui::MenuItem("UV Sphere", nullptr, nullptr, isLoaded && isValid))
						{
							ArchiveStaticMeshComponent archive(0);
							archive.assetID = createSphereMesh(m_library, Application::app()->renderer());
							m_currentNode = m_scene.get().createChild(m_currentNode, "Sphere node");
							m_currentNode->attach<StaticMeshComponent>().fromArchive(archive);
						}
						if (ImGui::MenuItem("Plane", nullptr, nullptr, isLoaded && isValid))
						{
							ArchiveStaticMeshComponent archive(0);
							archive.assetID = createPlaneMesh(m_library, Application::app()->renderer());
							m_currentNode = m_scene.get().createChild(m_currentNode, "Plane node");
							m_currentNode->attach<StaticMeshComponent>().fromArchive(archive);
						}
						ImGui::EndMenu();
					}
					if (ImGui::BeginMenu("Light"))
					{
						if (ImGui::MenuItem("Point light", nullptr, nullptr, isLoaded && isValid))
						{
							//m_currentNode = Scene::createPointLightNode(world).handle();
						}
						if (ImGui::MenuItem("Directional light", nullptr, nullptr, isLoaded && isValid))
						{
							//m_currentNode = Scene::createDirectionalLightNode(world).handle();
						}
						ImGui::EndMenu();
					}

					if (ImGui::MenuItem("Camera", nullptr, nullptr, isLoaded && isValid))
					{
						ArchiveCameraComponent archive(0);
						archive.projectionType = CameraProjectionType::Perpective;
						m_currentNode = m_scene.get().createChild(m_currentNode, "Camera");
						m_currentNode->attach<CameraComponent>().fromArchive(archive);
					}
					if (ImGui::MenuItem("Empty", nullptr, nullptr, isLoaded && isValid))
					{
						m_currentNode = m_scene.get().createChild(m_currentNode, "Empty");
					}
					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("Destroy", nullptr, nullptr, isLoaded && isValid))
					m_nodeToDestroy = m_currentNode;
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Component", isLoaded && isValid))
			{
				if (ImGui::BeginMenu("Add", isLoaded && isValid))
				{
					if (ImGui::BeginMenu("Static Mesh", isLoaded && isValid && !m_currentNode->has<StaticMeshComponent>()))
					{
						static ImGuiTextFilter filter;
						filter.Draw();
						for (const auto& asset : m_library->getAssetRange())
						{
							if (asset.second.type != AssetType::StaticMesh)
								continue;
							String name = OS::File::basename(asset.second.path.getRawPath());
							if (!filter.PassFilter(name.cstr()))
								continue;
							if (ImGui::MenuItem(name.cstr(), nullptr, nullptr, isLoaded && isValid && !m_currentNode->has<StaticMeshComponent>()))
							{
								ArchiveStaticMeshComponent archive(0);
								archive.assetID = asset.first;
								m_currentNode->attach<StaticMeshComponent>().fromArchive(archive);
							}
						}
						ImGui::EndMenu();
					}
					if (ImGui::BeginMenu("Skeletal Mesh", isLoaded && isValid && !m_currentNode->has<SkeletalMeshComponent>()))
					{
						static ImGuiTextFilter filter;
						filter.Draw();
						for (const auto& asset : m_library->getAssetRange())
						{
							if (asset.second.type != AssetType::SkeletalMesh)
								continue;
							String name = OS::File::basename(asset.second.path.getRawPath());
							if (!filter.PassFilter(name.cstr()))
								continue;
							if (ImGui::MenuItem(name.cstr(), nullptr, nullptr, isLoaded && isValid && !m_currentNode->has<SkeletalMeshComponent>()))
							{
								ArchiveSkeletalMeshComponent archive(0);
								archive.assetID = asset.first;
								m_currentNode->attach<SkeletalMeshComponent>().fromArchive(archive);
							}
						}
						ImGui::EndMenu();
					}
					if (ImGui::MenuItem("RigidBodyComponent", nullptr, nullptr, isLoaded && isValid && !m_currentNode->has<RigidBodyComponent>()))
					{
						ArchiveRigidBodyComponent archive(0);
						archive.mass = 1.f;
						m_currentNode->attach<RigidBodyComponent>().fromArchive(archive);
					}
					if (ImGui::BeginMenu("ColliderComponent", isLoaded && isValid && !m_currentNode->has<ColliderComponent>()))
					{
						if (ImGui::MenuItem("Plane", nullptr, nullptr, isLoaded && isValid && !m_currentNode->has<ColliderComponent>()))
						{
							ArchiveColliderComponent archive(0);
							archive.shape = ColliderShapeType::Plane;
							m_currentNode->attach<ColliderComponent>().fromArchive(archive);
						}
						if (ImGui::MenuItem("Sphere", nullptr, nullptr, isLoaded && isValid && !m_currentNode->has<ColliderComponent>()))
						{
							ArchiveColliderComponent archive(0);
							archive.shape = ColliderShapeType::Sphere;
							m_currentNode->attach<ColliderComponent>().fromArchive(archive);
						}
						if (ImGui::MenuItem("Box", nullptr, nullptr, isLoaded && isValid && !m_currentNode->has<ColliderComponent>()))
						{
							ArchiveColliderComponent archive(0);
							archive.shape = ColliderShapeType::Cube;
							m_currentNode->attach<ColliderComponent>().fromArchive(archive);
						}
						ImGui::EndMenu();
					}
					if (ImGui::MenuItem("CameraComponent", nullptr, nullptr, isLoaded && isValid && !m_currentNode->has<CameraComponent>()))
					{
						ArchiveCameraComponent archive(0);
						archive.projectionType = CameraProjectionType::Perpective;
						m_currentNode->attach<CameraComponent>().fromArchive(archive);
					}
					if (ImGui::MenuItem("CustomComponent", nullptr, nullptr, isLoaded && isValid && !m_currentNode->has<CustomComponent>()))
						m_currentNode->attach<CustomComponent>();
					if (ImGui::MenuItem("RotatorComponent", nullptr, nullptr, isLoaded && isValid && !m_currentNode->has<RotatorComponent>()))
						m_currentNode->attach<RotatorComponent>();
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Remove", isLoaded && isValid))
				{
					if (ImGui::MenuItem("Mesh", nullptr, nullptr, isLoaded && isValid && m_currentNode->has<StaticMeshComponent>()))
						m_currentNode->detach<StaticMeshComponent>();
					if (ImGui::MenuItem("Camera", nullptr, nullptr, isLoaded && isValid && m_currentNode->has<CameraComponent>()))
						m_currentNode->detach<CameraComponent>();
					if (ImGui::MenuItem("CustomComponent", nullptr, nullptr, isLoaded && isValid && m_currentNode->has<CustomComponent>()))
						m_currentNode->detach<CustomComponent>();
					if (ImGui::MenuItem("RotatorComponent", nullptr, nullptr, isLoaded && isValid && m_currentNode->has<RotatorComponent>()))
						m_currentNode->detach<RotatorComponent>();
					if (ImGui::MenuItem("RigidBodyComponent", nullptr, nullptr, isLoaded && isValid && m_currentNode->has<RigidBodyComponent>()))
						m_currentNode->detach<RigidBodyComponent>();
					if (ImGui::MenuItem("ColliderComponent", nullptr, nullptr, isLoaded && isValid && m_currentNode->has<ColliderComponent>()))
						m_currentNode->detach<ColliderComponent>();
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Transform operation"))
			{
				bool enabled = m_gizmoMode == ImGuizmo::LOCAL;
				if (ImGui::MenuItem("Local", nullptr, &enabled))
					m_gizmoMode = ImGuizmo::LOCAL;
				enabled = m_gizmoMode == ImGuizmo::WORLD;
				if (ImGui::MenuItem("World", nullptr, &enabled))
					m_gizmoMode = ImGuizmo::WORLD;
				ImGui::Separator();
				enabled = m_gizmoOperation == ImGuizmo::TRANSLATE;
				if (ImGui::MenuItem("Translate", "T", &enabled))
					m_gizmoOperation = ImGuizmo::TRANSLATE;
				enabled = m_gizmoOperation == ImGuizmo::ROTATE;
				if (ImGui::MenuItem("Rotate", "R", &enabled))
					m_gizmoOperation = ImGuizmo::ROTATE;
				enabled = m_gizmoOperation == ImGuizmo::SCALE;
				if (ImGui::MenuItem("Scale", "S", &enabled))
					m_gizmoOperation = ImGuizmo::SCALE;
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		if (ImGui::BeginChild("##list", ImVec2(0, 200), true))
		{
			if (isLoaded)
			{
				m_scene.get().visitChildrens([&](Node* children) {
					recurse(children, m_currentNode);
				});
			}
		}
		ImGui::EndChild();

		// --- Add entity
		static char m_newEntityName[256];
		ImGui::InputTextWithHint("##entityName", "Entity name", m_newEntityName, 256);
		ImGui::SameLine();
		if (ImGui::Button("Create entity") && isLoaded)
		{
			m_currentNode = m_scene.get().createChild(m_currentNode, m_newEntityName);
		}
		ImGui::Separator();

		// --- Entity info
		if (isLoaded)
		{
			Scene& scene = m_scene.get();
			if (m_currentNode)
			{
				ImGui::TextColored(ImGuiLayer::Color::red, "%s", m_currentNode->getName().cstr());
				if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_DefaultOpen))
				{
					bool updatedTransform = false;
					mat4f localTransform = m_currentNode->getLocalTransform();
					mat4f worldTransform = m_currentNode->getWorldTransform();
					float translation[3];
					float rotation[3];
					float scale[3];
					ImGuizmo::DecomposeMatrixToComponents(localTransform.cols[0].data, translation, rotation, scale);
					updatedTransform |= ImGui::InputFloat3("Local Translation", translation);
					updatedTransform |= ImGui::InputFloat3("Local Rotation", rotation);
					updatedTransform |= ImGui::InputFloat3("Local Scale", scale);
					if (updatedTransform)
					{
						ImGuizmo::RecomposeMatrixFromComponents(translation, rotation, scale, localTransform.cols[0].data);
						m_currentNode->setLocalTransform(localTransform);
					}
					if (const Node* node = scene.getMainCameraNode())
					{
						const CameraComponent& camera = node->get<CameraComponent>();
						mat4f view = camera.getViewMatrix();
						mat4f projection = camera.getProjectionMatrix();
						// Draw gizmo axis
						ImGuiIO& io = ImGui::GetIO();
						ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
						if (ImGuizmo::Manipulate(view[0].data, projection[0].data, m_gizmoOperation, m_gizmoMode, worldTransform[0].data))
						{
							mat4f inverseParentWorld = mat4f::inverse(m_currentNode->getParentTransform());
							m_currentNode->setLocalTransform(inverseParentWorld * worldTransform);
							updatedTransform = true;
						}
					}

					ImGui::TreePop();
				}
				if (m_currentNode->isOrphan())
				{
					ImGui::Separator();
					ImGui::Text("Add a component to the entity.");
				}
				else
				{
					// Draw every component.
					component<StaticMeshComponent>(m_library, debugDrawList, m_currentNode);
					component<SkeletalMeshComponent>(m_library, debugDrawList, m_currentNode);
					component<RigidBodyComponent>(m_library, debugDrawList, m_currentNode);
					component<ColliderComponent>(m_library, debugDrawList, m_currentNode);
					component<CameraComponent>(m_library, debugDrawList, m_currentNode);
					component<ArcballComponent>(m_library, debugDrawList, m_currentNode);
					component<CustomComponent>(m_library, debugDrawList, m_currentNode);
					component<RotatorComponent>(m_library, debugDrawList, m_currentNode);
				}
			}
			else
			{
				ImGui::TextColored(ImGuiLayer::Color::red, "No node selected");
			}
		}
	}
	if (resetScene && m_scene.isLoaded())
	{
		m_assetToUnload = m_scene.get().getID();
		m_scene = ResourceHandle<Scene>::invalid();
		m_currentNode = nullptr;
		m_nodeToDestroy = nullptr;
	}
	if (newScene)
	{
		// If we want to create a new scene, letsss go.
		// Asset ID based on path, so...
		// Should have an asset ID based on something else
	}
}

void SceneEditorLayer::onPostRender()
{
}

void SceneEditorLayer::onResize(uint32_t width, uint32_t height)
{
}

void SceneEditorLayer::onReceive(const aka::ResourceLoadedEvent& event)
{
	if (event.type == ResourceType::Scene)
	{
		setCurrentScene(Application::app()->assets()->get<Scene>(event.resource));
	}
}

void SceneEditorLayer::setLibrary(aka::AssetLibrary* library)
{
	m_library = library;
}

void SceneEditorLayer::setCurrentScene(ResourceHandle<Scene> _scene)
{
	// TODO handle multiple scene with tabs ?
	setVisible(true);
	setEnabled(true);

	if (m_scene.isLoaded())
		m_assetToUnload = m_scene.get().getID();
	m_scene = _scene;
	m_currentNode = nullptr;
	m_nodeToDestroy = nullptr;
}

};