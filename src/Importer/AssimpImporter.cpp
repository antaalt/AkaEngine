#include "AssimpImporter.hpp"

#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/pbrmaterial.h>

#include <Aka/Aka.h>

#include "../Asset/AssetLibrary.hpp"
#include "../Asset/Archive/ArchiveScene.hpp"

namespace app {

using namespace aka;

struct AssimpLocalImporter
{
	AssimpLocalImporter(const Path& directory, AssetLibrary* _library, const aiScene* scene);

	AssetID registerAsset(AssetType type, const String& name);

	void process();
	void processNode(ArchiveSceneID parent, aiNode* node);
	ArchiveSceneID processMesh(aiMesh* mesh);
	ArchiveImage processImage(const Path& path);
private:
	AssetLibrary* m_library;
	Path m_directory;
	const aiScene* m_assimpScene;
	ArchiveScene m_scene;
private:
	ArchiveImage m_missingColorTexture;
	ArchiveImage m_blankColorTexture;
	ArchiveImage m_missingNormalTexture;
	ArchiveImage m_missingRoughnessTexture;
};

ArchiveSceneEntity createEntity(const char* name)
{
	ArchiveSceneEntity entity{};
	entity.name = name;
	entity.components = SceneComponentMask::None;
	for (uint32_t i = 0; i < EnumCount<SceneComponent>(); i++)
		entity.id[i] = InvalidArchiveSceneID;
	return entity;
}
void addComponent(ArchiveSceneEntity& entity, SceneComponent _component, ArchiveSceneID id)
{
	entity.components |= SceneComponentMask(1U << EnumToIndex(_component));
	entity.id[EnumToIndex(_component)] = id;
}
ArchiveSceneID addEntity(ArchiveScene& scene, ArchiveSceneEntity entity)
{
	return ArchiveSceneID(scene.entities.append(entity).size() - 1);
}
ArchiveSceneID addTransform(ArchiveScene& scene, const mat4f& transform)
{
	return ArchiveSceneID(scene.transforms.append(ArchiveSceneTransform{ transform }).size() - 1);
}
ArchiveSceneID addStaticMesh(ArchiveScene& scene, ArchiveStaticMesh&& mesh)
{
	return ArchiveSceneID(scene.meshes.append(std::move(mesh)).size() - 1);
}
void addTextureData(ArchiveImage& image, uint32_t width, uint32_t height, uint32_t channels, const uint8_t* data)
{
	image.channels = channels;
	image.width = width;
	image.height = height;
	image.data = Vector<uint8_t>(data, width * height * channels);
}
const char* getAssetTypeFolder(AssetType type)
{
	switch (type)
	{
	default:
	case app::AssetType::Unknown:
		AKA_UNREACHABLE;
		return nullptr;
	case app::AssetType::Geometry: return "geometries";
	case app::AssetType::Material: return "materials";
	case app::AssetType::Batch: return "batches";
	case app::AssetType::StaticMesh: return "static-meshes";
	case app::AssetType::DynamicMesh: return "dynamic-meshes";
	case app::AssetType::Image:return "images";
	case app::AssetType::Font:return "fonts";
	case app::AssetType::Audio:return "audios";
	case app::AssetType::Scene:return "scenes";
	}
}
const char* getAssetExtension(AssetType type)
{
	switch (type)
	{
	default:
	case app::AssetType::Unknown:
		AKA_UNREACHABLE;
		return nullptr;
	case app::AssetType::Geometry: return "geo";
	case app::AssetType::Material: return "mat";
	case app::AssetType::Batch: return "bat";
	case app::AssetType::StaticMesh: return "smesh";
	case app::AssetType::DynamicMesh: return "dmesh";
	case app::AssetType::Image:return "img";
	case app::AssetType::Font:return "fnt";
	case app::AssetType::Audio:return "odio";
	case app::AssetType::Scene:return "sce";
	}
}
AssetPath getAssetPath(const String& sceneName, AssetType type, const String& name)
{
	String path;
	path.append("import/");
	path.append(sceneName);
	path.append("/");
	path.append(getAssetTypeFolder(type));
	path.append("/");
	path.append(name);
	path.append(".");
	path.append(getAssetExtension(type));
	return AssetPath(path);
}

AssimpLocalImporter::AssimpLocalImporter(const Path& directory, AssetLibrary* _library, const aiScene* aiScene) :
	m_library(_library),
	m_directory(directory),
	m_assimpScene(aiScene),
	m_scene(registerAsset(AssetType::Scene, aiScene->mName.C_Str()))
{
	// Create folder for saving
	Path path;
	path.append(AssetPath::getAssetPath());
	path.append("import/scene/");
	for (AssetType type : EnumRange<AssetType>())
	{
		Path copyPath = path;
		copyPath.append(getAssetTypeFolder(type));
		copyPath.append("/");
		bool created = OS::Directory::create(copyPath);
	}
	m_missingColorTexture = ArchiveImage(registerAsset(AssetType::Image, "missingColorTexture"));
	m_blankColorTexture = ArchiveImage(registerAsset(AssetType::Image, "blankColorTexture"));
	m_missingNormalTexture = ArchiveImage(registerAsset(AssetType::Image, "missingNormalTexture"));
	m_missingRoughnessTexture = ArchiveImage(registerAsset(AssetType::Image, "missingRoughnessTexture"));

	uint8_t bytesMissingColor[4] = { 255, 0, 255, 255 };
	uint8_t bytesBlankColor[4] = { 255, 255, 255, 255 };
	uint8_t bytesNormal[4] = { 128,128,255,255 };
	uint8_t bytesRoughness[4] = { 255,255,255,255 };
	addTextureData(m_missingColorTexture, 1, 1, 4, bytesMissingColor);
	addTextureData(m_blankColorTexture, 1, 1, 4, bytesBlankColor);
	addTextureData(m_missingNormalTexture, 1, 1, 4, bytesNormal);
	addTextureData(m_missingRoughnessTexture, 1, 1, 4, bytesRoughness);

	// Reserve vector to avoid rellocation
	m_scene.meshes.reserve(m_assimpScene->mNumMeshes);
	m_scene.transforms.reserve(m_assimpScene->mNumMeshes);
}

AssetID AssimpLocalImporter::registerAsset(AssetType type, const String& name)
{
	String finalName = name;
	if (finalName == "")
		finalName = "unnamed";
	return m_library->registerAsset(getAssetPath("scene", type, finalName), type);
}

void AssimpLocalImporter::process()
{
	ArchiveSceneEntity root = createEntity("ROOT");
	addComponent(root, SceneComponent::Transform, addTransform(m_scene, mat4f::identity()));
	addComponent(root, SceneComponent::Hierarchy, InvalidArchiveSceneID); // No parent here.

	ArchiveSceneID rootID = addEntity(m_scene, root);
	processNode(rootID, m_assimpScene->mRootNode);

	// Compute scene bbox.
	/*for (ArchiveSceneEntity& entity : m_scene.entities)
	{
		SceneComponentMask mask = SceneComponentMask::Transform | SceneComponentMask::StaticMesh;
		if ((entity.components & mask) == mask)
		{
			ArchiveSceneID transformID = entity.id[EnumToIndex(SceneComponent::Transform)];
			ArchiveSceneID meshID = entity.id[EnumToIndex(SceneComponent::StaticMesh)];
			const mat4f& transform = scene.transforms[toIntegral(transformID)].matrix;
			const ArchiveStaticMesh& mesh = scene.meshes[toIntegral(meshID)];
			for (const ArchiveBatch& batch : mesh.batches)
			{
				scene.bounds.include(transform * batch.geometry.bounds);
			}
		}
	}*/

	m_scene.save(ArchiveSaveContext(m_library));
}


mat4f getParentTransform(ArchiveScene& _scene, ArchiveSceneID _parentID)
{
	if (_parentID == InvalidArchiveSceneID)
	{
		return mat4f::identity();
	}
	else
	{
		const ArchiveSceneEntity& e = _scene.entities[toIntegral(_parentID)];
		ArchiveSceneID parentID = e.id[EnumToIndex(SceneComponent::Hierarchy)];
		ArchiveSceneID transformID = e.id[EnumToIndex(SceneComponent::Transform)];
		return getParentTransform(_scene, parentID) * _scene.transforms[toIntegral(transformID)].matrix;
	}
}

void AssimpLocalImporter::processNode(ArchiveSceneID _parent, aiNode* _node)
{
	mat4f transform = mat4f(
		col4f(_node->mTransformation[0][0], _node->mTransformation[1][0], _node->mTransformation[2][0], _node->mTransformation[3][0]),
		col4f(_node->mTransformation[0][1], _node->mTransformation[1][1], _node->mTransformation[2][1], _node->mTransformation[3][1]),
		col4f(_node->mTransformation[0][2], _node->mTransformation[1][2], _node->mTransformation[2][2], _node->mTransformation[3][2]),
		col4f(_node->mTransformation[0][3], _node->mTransformation[1][3], _node->mTransformation[2][3], _node->mTransformation[3][3])
	);
	// Inverse computed at runtime instead.
	/*mat4f inverseParentTransform;
	if (_parent != InvalidArchiveSceneID)
	{
		ArchiveSceneID transformID = m_scene.entities[toIntegral(_parent)].id[EnumToIndex(SceneComponent::Transform)];
		mat4f parentTransform = m_scene.transforms[toIntegral(transformID)].matrix;
		transform = parentTransform * transform;
		inverseParentTransform = mat4f::inverse(parentTransform);
	}
	else
	{
		inverseParentTransform = mat4f::identity();
	}*/
	// process all the node's meshes (if any)
	// TODO: those are batches ? Return batch instead of mesh here...
	ArchiveSceneID transformID = addTransform(m_scene, transform);
	for (unsigned int i = 0; i < _node->mNumMeshes; i++)
	{
		aiMesh* aiMesh = m_assimpScene->mMeshes[_node->mMeshes[i]];
		ArchiveSceneID meshID = processMesh(aiMesh);

		ArchiveSceneEntity entity = createEntity(aiMesh->mName.C_Str());
		addComponent(entity, SceneComponent::Transform, transformID);
		addComponent(entity, SceneComponent::Hierarchy, _parent);
		addComponent(entity, SceneComponent::StaticMesh, meshID);
		
		ArchiveSceneID entityID = addEntity(m_scene, entity);

		// Compute bounds
		for (size_t j = 0; j < m_scene.meshes[toIntegral(meshID)].batches.size(); j++)
			m_scene.bounds.include(getParentTransform(m_scene, entityID) * m_scene.meshes[toIntegral(meshID)].batches[j].geometry.bounds);
	}
	if (_node->mNumChildren > 0)
	{
		ArchiveSceneEntity entity = createEntity(_node->mName.C_Str());
		addComponent(entity, SceneComponent::Transform, transformID);
		addComponent(entity, SceneComponent::Hierarchy, _parent);

		ArchiveSceneID entityID = addEntity(m_scene, entity);

		for (unsigned int i = 0; i < _node->mNumChildren; i++)
			processNode(entityID, _node->mChildren[i]);
	}
}

ArchiveSceneID AssimpLocalImporter::processMesh(aiMesh* mesh)
{
	AKA_ASSERT(mesh->HasPositions(), "Mesh need positions");
	AKA_ASSERT(mesh->HasNormals(), "Mesh needs normals");

	// process vertices
	ArchiveGeometry archiveGeometry = ArchiveGeometry(registerAsset(AssetType::Geometry, mesh->mName.C_Str()));
	archiveGeometry.vertices.resize(mesh->mNumVertices);
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex& vertex = archiveGeometry.vertices[i];
		// process vertex positions, normals and texture coordinates
		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;
		archiveGeometry.bounds.include(vertex.position);

		//vertex.normal.x = mesh->mNormals[i].x;
		//vertex.normal.y = mesh->mNormals[i].y;
		//vertex.normal.z = mesh->mNormals[i].z;
		if (mesh->HasTextureCoords(0))
		{
			vertex.uv.u = mesh->mTextureCoords[0][i].x;
			vertex.uv.v = mesh->mTextureCoords[0][i].y;
		}
		else
			vertex.uv = uv2f(0.f);
		/*if (mesh->HasVertexColors(0))
		{
			vertex.color.r = mesh->mColors[0][i].r;
			vertex.color.g = mesh->mColors[0][i].g;
			vertex.color.b = mesh->mColors[0][i].b;
			vertex.color.a = mesh->mColors[0][i].a;
		}
		else
			vertex.color = color4f(1.f);*/
		mesh->mTangents; // TODO:
		mesh->mBitangents;
	}
	// process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			archiveGeometry.indices.append(face.mIndices[j]);
	}

	// process material
	ArchiveMaterial archiveMaterial;
	if (mesh->mMaterialIndex >= 0)
	{
		//aiTextureType_EMISSION_COLOR = 14,
		//aiTextureType_METALNESS = 15,
		//aiTextureType_DIFFUSE_ROUGHNESS = 16,
		//aiTextureType_AMBIENT_OCCLUSION = 17,
		aiMaterial* material = m_assimpScene->mMaterials[mesh->mMaterialIndex];
		archiveMaterial = ArchiveMaterial(registerAsset(AssetType::Material, material->GetName().C_Str()));
		aiColor4D c;
		bool doubleSided = false;
		material->Get(AI_MATKEY_COLOR_DIFFUSE, c);
		material->Get(AI_MATKEY_TWOSIDED, doubleSided);
		archiveMaterial.color = color4f(c.r, c.g, c.b, c.a);
		if (doubleSided)
			archiveMaterial.flags |= ArchiveMaterialFlag::DoubleSided;

		// Albedo
		if (material->GetTextureCount(aiTextureType_BASE_COLOR) > 0)
		{
			aiTextureType type = aiTextureType_BASE_COLOR;
			for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
			{
				aiString str;
				material->GetTexture(type, i, &str);
				archiveMaterial.albedo = processImage(Path(m_directory + str.C_Str()));
				if (archiveMaterial.albedo.size() == 0)
					archiveMaterial.albedo = m_missingColorTexture;
				break; // Ignore others textures for now.
			}
		}
		else if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiTextureType type = aiTextureType_DIFFUSE;
			for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
			{
				aiString str;
				material->GetTexture(type, i, &str);
				archiveMaterial.albedo = processImage(Path(m_directory + str.C_Str()));
				if (archiveMaterial.albedo.size() == 0)
					archiveMaterial.albedo = m_missingColorTexture;
				break; // Ignore others textures for now.
			}
		}
		else
		{
			archiveMaterial.albedo = m_blankColorTexture;
		}

		// Normal
		if (material->GetTextureCount(aiTextureType_NORMAL_CAMERA) > 0)
		{
			aiTextureType type = aiTextureType_NORMAL_CAMERA;
			for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
			{
				aiString str;
				material->GetTexture(type, i, &str);
				archiveMaterial.normal = processImage(Path(m_directory + str.C_Str()));
				if (archiveMaterial.normal.size() == 0)
					archiveMaterial.normal = m_missingNormalTexture;
				break; // Ignore others textures for now.
			}
		}
		else if (material->GetTextureCount(aiTextureType_NORMALS) > 0)
		{
			aiTextureType type = aiTextureType_NORMALS;
			for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
			{
				aiString str;
				material->GetTexture(type, i, &str);
				archiveMaterial.normal = processImage(Path(m_directory + str.C_Str()));
				if (archiveMaterial.normal.size() == 0)
					archiveMaterial.normal = m_missingNormalTexture;
				break; // Ignore others textures for now.
			}
		}
		else
		{
			archiveMaterial.normal = m_missingNormalTexture;
		}

		// PBR textures
		/*if (material->GetTextureCount(aiTextureType_UNKNOWN) > 0) // GLTF pbr texture is retrieved this way (?)
		{
			aiTextureType type = aiTextureType_UNKNOWN;
			for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
			{
				aiString str;
				material->GetTexture(type, 0, &str);
				archiveMaterial.material = processImage(Path(m_directory + str.C_Str()));
				if (archiveMaterial.material.size() == 0)
					archiveMaterial.material = m_missingRoughnessTexture;
				break; // Ignore others textures for now.
			}
		}
		else if (material->GetTextureCount(aiTextureType_SHININESS) > 0)
		{
			aiTextureType type = aiTextureType_SHININESS;
			for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
			{
				aiString str;
				material->GetTexture(type, i, &str);
				archiveMaterial.material = processImage(Path(m_directory + str.C_Str()));
				if (archiveMaterial.material.size() == 0)
					archiveMaterial.material = m_missingRoughnessTexture;
				break; // Ignore others textures for now.
			}
		}
		else
		{
			archiveMaterial.material = m_missingRoughnessTexture;
		}*/
	}
	else
	{
		// No material !
		archiveMaterial = ArchiveMaterial(registerAsset(AssetType::Material, "DummyMaterial"));
		archiveMaterial.color = color4f(1.f);
		archiveMaterial.flags = ArchiveMaterialFlag::DoubleSided;
		archiveMaterial.albedo = m_blankColorTexture;
		archiveMaterial.normal = m_missingNormalTexture;
		//archiveMaterial.material = m_missingRoughnessTexture;
	}
	ArchiveBatch archiveBatch = ArchiveBatch(registerAsset(AssetType::Batch, mesh->mName.C_Str()));
	archiveBatch.geometry = std::move(archiveGeometry);
	archiveBatch.material = std::move(archiveMaterial);
	ArchiveStaticMesh archiveMesh = ArchiveStaticMesh(registerAsset(AssetType::StaticMesh, mesh->mName.C_Str()));
	archiveMesh.batches.append(std::move(archiveBatch));
	
	return addStaticMesh(m_scene, std::move(archiveMesh));
}

ArchiveImage AssimpLocalImporter::processImage(const Path& path)
{
	ArchiveImage image(registerAsset(AssetType::Image, OS::File::basename(path)));

	Image img = ImageDecoder::fromDisk(path);
	image.channels = getImageComponentCount(img.components);
	image.width = img.width;
	image.height = img.height;
	image.data = std::move(img.bytes);

	return image;
}

template <Assimp::Logger::ErrorSeverity Severity>
class LogStream : public Assimp::LogStream
{
public:
	LogStream() {}

	virtual void write(const char* message) override
	{
		// TODO better parsing
		std::string str(message);
		if (str.size() == 0)
			return;
		str.pop_back();
		switch (Severity)
		{
		case Assimp::Logger::Debugging:
			Logger::debug("[assimp]", str);
			break;
		case Assimp::Logger::Info:
			Logger::info("[assimp]", str);
			break;
		case Assimp::Logger::Warn:
			Logger::warn("[assimp]", str);
			break;
		case Assimp::Logger::Err:
			Logger::error("[assimp]", str);
			break;
		default:
			break;
		}
	}
};
using DebugLogStream = LogStream<Assimp::Logger::Debugging>;
using InfoLogStream = LogStream<Assimp::Logger::Info>;
using WarnLogStream = LogStream<Assimp::Logger::Warn>;
using ErrorLogStream = LogStream<Assimp::Logger::Err>;

Assimp::Logger* createAssimpLogger()
{
#if defined(AKA_DEBUG)
	Assimp::Logger::LogSeverity severity = Assimp::Logger::LogSeverity::VERBOSE;
#else
	Assimp::Logger::LogSeverity severity = Assimp::Logger::LogSeverity::NORMAL;
#endif
	Assimp::Logger* logger = Assimp::DefaultLogger::create("", severity, 0);
	logger->attachStream(new DebugLogStream(), Assimp::Logger::Debugging);
	logger->attachStream(new InfoLogStream(), Assimp::Logger::Info);
	logger->attachStream(new WarnLogStream(), Assimp::Logger::Warn);
	logger->attachStream(new ErrorLogStream(), Assimp::Logger::Err);
	return logger;
}

unsigned int getAssimpFlags()
{
	return aiProcess_Triangulate |
		//aiProcess_CalcTangentSpace |
#if defined(AKA_ORIGIN_TOP_LEFT)
		aiProcess_FlipUVs |
#endif
#if defined(GEOMETRY_LEFT_HANDED)
		aiProcess_MakeLeftHanded |
#endif
		aiProcess_GenSmoothNormals;
}


ImportResult AssimpImporter::import(AssetLibrary * _library, const aka::Path & path)
{
	createAssimpLogger();
	Assimp::Importer assimpImporter;
	const aiScene* aiScene = assimpImporter.ReadFile(path.cstr(), getAssimpFlags());
	if (!aiScene || aiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !aiScene->mRootNode)
	{
		Logger::error("[assimp] ", assimpImporter.GetErrorString());
		return ImportResult::CouldNotReadFile;
	}
	AssimpLocalImporter importer(path.up(), _library, aiScene);
	importer.process();
	Assimp::DefaultLogger::kill();
	return ImportResult::Succeed;
}

ImportResult AssimpImporter::import(AssetLibrary * _library, const aka::Blob & blob)
{
#if 1
	// With blob, we dont have ref folder for texture loading...
	return ImportResult::Failed;
#else
	createAssimpLogger();
	Assimp::Importer assimpImporter;
	const aiScene* aiScene = assimpImporter.ReadFileFromMemory(blob.data(), blob.size(), getAssimpFlags());
	if (!aiScene || aiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !aiScene->mRootNode)
	{
		Logger::error("[assimp] ", assimpImporter.GetErrorString());
		return ImportResult::CouldNotReadFile;
	}
	Path directory = path.up();
	AssimpLocalImporter importer(directory, _library, aiScene);
	importer.process();
	Assimp::DefaultLogger::kill();
	return ImportResult::Succeed;
#endif
}

}