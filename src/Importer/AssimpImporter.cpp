#include "AssimpImporter.hpp"

#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/pbrmaterial.h>

#include <Aka/Aka.h>

#include <Aka/Resource/AssetLibrary.hpp>
#include <Aka/Resource/Archive/ArchiveScene.hpp>

namespace app {

using namespace aka;

class AssimpImporterImpl
{
public:
	AssimpImporterImpl(AssimpImporter* importer, const Path& directory, const aiScene* scene);

	void process();
	void processNode(ArchiveSceneID parent, aiNode* node);
	ArchiveSceneID processMesh(aiMesh* mesh);
	ArchiveMaterial processMaterial(aiMaterial* material);
	ArchiveImage processImage(const Path& path);
private:
	AssimpImporter* m_importer;
	Path m_directory;
	const aiScene* m_assimpScene;
	ArchiveScene m_scene;
private:
	ArchiveImage m_missingColorTexture;
	ArchiveImage m_blankColorTexture;
	ArchiveImage m_missingNormalTexture;
	ArchiveImage m_missingRoughnessTexture;
	ArchiveMaterial m_dummyMaterial;
	std::map<AssetID, ArchiveImage> m_imageCache;
	std::map<AssetID, ArchiveMaterial> m_materialCache;
};

ArchiveSceneEntity createEntity(const char* name)
{
	ArchiveSceneEntity entity{};
	entity.name = name;
	entity.components = SceneComponentMask::None;
	for (uint32_t i = 0; i < EnumCount<SceneComponent>(); i++)
		entity.id[i] = ArchiveSceneID::Invalid;
	return entity;
}
void addComponent(ArchiveSceneEntity& entity, SceneComponent _component, ArchiveSceneID id)
{
	entity.components |= SceneComponentMask(1U << EnumToIndex(_component));
	entity.id[EnumToIndex(_component)] = id;
}
ArchiveSceneID addEntity(ArchiveScene& scene, ArchiveSceneEntity entity)
{
	scene.entities.append(entity);
	return ArchiveSceneID(scene.entities.size() - 1);
}
ArchiveSceneID addTransform(ArchiveScene& scene, const mat4f& transform)
{
	scene.transforms.append(ArchiveSceneTransform{ transform });
	return ArchiveSceneID(scene.transforms.size() - 1);
}
ArchiveSceneID addStaticMesh(ArchiveScene& scene, ArchiveStaticMesh&& mesh)
{
	scene.meshes.append(std::move(mesh));
	return ArchiveSceneID(scene.meshes.size() - 1);
}
void addTextureData(ArchiveImage& image, uint32_t width, uint32_t height, uint32_t channels, const uint8_t* data)
{
	image.channels = channels;
	image.width = width;
	image.height = height;
	image.data = Vector<uint8_t>(data, width * height * channels);
}

String getSceneName(const aiScene* scene, const String backupName)
{
	if (scene->mName.length > 0)
		return scene->mName.C_Str();
	else if (scene->mRootNode->mName.length > 0)
		return scene->mRootNode->mName.C_Str();
	else
		return backupName;
}

AssimpImporterImpl::AssimpImporterImpl(AssimpImporter* _importer, const Path& directory, const aiScene* aiScene) :
	m_importer(_importer),
	m_directory(directory),
	m_assimpScene(aiScene),
	m_scene(m_importer->registerAsset(AssetType::Scene, getSceneName(aiScene, _importer->getName())))
{
	// Create folder for saving
	Path path = m_importer->getAssetPath().getAbsolutePath();
	for (AssetType type : EnumRange<AssetType>())
	{
		const char* folder = Importer::getAssetTypeName(type);
		if (folder == nullptr)
			continue; // Do not need this folder.
		Path copyPath = path;
		copyPath.append(folder);
		copyPath.append("/");
		bool created = OS::Directory::create(copyPath);
	}
	m_missingColorTexture = ArchiveImage(m_importer->registerAsset(AssetType::Image, "missingColorTexture"));
	m_blankColorTexture = ArchiveImage(m_importer->registerAsset(AssetType::Image, "blankColorTexture"));
	m_missingNormalTexture = ArchiveImage(m_importer->registerAsset(AssetType::Image, "missingNormalTexture"));
	m_missingRoughnessTexture = ArchiveImage(m_importer->registerAsset(AssetType::Image, "missingRoughnessTexture"));

	uint8_t bytesMissingColor[4] = { 255, 0, 255, 255 };
	uint8_t bytesBlankColor[4] = { 255, 255, 255, 255 };
	uint8_t bytesNormal[4] = { 128,128,255,255 };
	uint8_t bytesRoughness[4] = { 255,255,255,255 };
	addTextureData(m_missingColorTexture, 1, 1, 4, bytesMissingColor);
	addTextureData(m_blankColorTexture, 1, 1, 4, bytesBlankColor);
	addTextureData(m_missingNormalTexture, 1, 1, 4, bytesNormal);
	addTextureData(m_missingRoughnessTexture, 1, 1, 4, bytesRoughness);

	m_dummyMaterial = ArchiveMaterial(m_importer->registerAsset(AssetType::Material, "DummyMaterial"));
	m_dummyMaterial.color = color4f(1.f);
	m_dummyMaterial.flags = ArchiveMaterialFlag::DoubleSided;
	m_dummyMaterial.albedo = m_blankColorTexture;
	m_dummyMaterial.normal = m_missingNormalTexture;
	//m_dummyMaterial.roughness = m_missingRoughnessTexture;

	// Reserve vector to avoid rellocation
	m_scene.meshes.reserve(m_assimpScene->mNumMeshes);
	m_scene.transforms.reserve(m_assimpScene->mNumMeshes);
}

void AssimpImporterImpl::process()
{
	ArchiveSceneEntity root = createEntity("ImporterRoot");
	addComponent(root, SceneComponent::Transform, addTransform(m_scene, mat4f::identity()));
	addComponent(root, SceneComponent::Hierarchy, ArchiveSceneID::Invalid); // No parent here.

	// Pre process meshes to keep them instanced.
	for (uint32_t i = 0; i < m_assimpScene->mNumMeshes; i++)
		processMesh(m_assimpScene->mMeshes[i]);
	
	ArchiveSceneID rootID = addEntity(m_scene, root);
	processNode(rootID, m_assimpScene->mRootNode);

	m_scene.save(ArchiveSaveContext(m_importer->getAssetLibrary()));
}


mat4f getParentTransform(ArchiveScene& _scene, ArchiveSceneID _parentID)
{
	if (_parentID == ArchiveSceneID::Invalid)
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

void AssimpImporterImpl::processNode(ArchiveSceneID _parent, aiNode* _node)
{
	mat4f transform = mat4f(
		col4f(_node->mTransformation[0][0], _node->mTransformation[1][0], _node->mTransformation[2][0], _node->mTransformation[3][0]),
		col4f(_node->mTransformation[0][1], _node->mTransformation[1][1], _node->mTransformation[2][1], _node->mTransformation[3][1]),
		col4f(_node->mTransformation[0][2], _node->mTransformation[1][2], _node->mTransformation[2][2], _node->mTransformation[3][2]),
		col4f(_node->mTransformation[0][3], _node->mTransformation[1][3], _node->mTransformation[2][3], _node->mTransformation[3][3])
	);
	// process all the node's meshes (if any)
	// TODO: those are batches ? Return batch instead of mesh here...
	ArchiveSceneID transformID = addTransform(m_scene, transform);
	for (unsigned int i = 0; i < _node->mNumMeshes; i++)
	{
		aiMesh* aiMesh = m_assimpScene->mMeshes[_node->mMeshes[i]];
		ArchiveSceneID meshID = ArchiveSceneID(_node->mMeshes[i]);

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

ArchiveSceneID AssimpImporterImpl::processMesh(aiMesh* mesh)
{
	AKA_ASSERT(mesh->HasPositions(), "Mesh need positions");
	AKA_ASSERT(mesh->HasNormals(), "Mesh needs normals");

	// process vertices
	ArchiveGeometry archiveGeometry = ArchiveGeometry(m_importer->registerAsset(AssetType::Geometry, mesh->mName.C_Str()));
	archiveGeometry.vertices.resize(mesh->mNumVertices);
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		StaticVertex& vertex = archiveGeometry.vertices[i];
		// process vertex positions, normals and texture coordinates
		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;
		archiveGeometry.bounds.include(vertex.position);

		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = mesh->mNormals[i].z;
		if (mesh->HasTextureCoords(0))
		{
			vertex.uv.u = mesh->mTextureCoords[0][i].x;
			vertex.uv.v = mesh->mTextureCoords[0][i].y;
		}
		else
			vertex.uv = uv2f(0.f);
		if (mesh->HasVertexColors(0))
		{
			vertex.color.r = mesh->mColors[0][i].r;
			vertex.color.g = mesh->mColors[0][i].g;
			vertex.color.b = mesh->mColors[0][i].b;
			vertex.color.a = mesh->mColors[0][i].a;
		}
		else
			vertex.color = color4f(1.f);
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
		aiMaterial* material = m_assimpScene->mMaterials[mesh->mMaterialIndex];
		archiveMaterial = processMaterial(material);
	}
	else
	{
		// No material !
		archiveMaterial = m_dummyMaterial;
	}
	ArchiveBatch archiveBatch = ArchiveBatch(m_importer->registerAsset(AssetType::Batch, mesh->mName.C_Str()));
	archiveBatch.geometry = std::move(archiveGeometry);
	archiveBatch.material = std::move(archiveMaterial);
	ArchiveStaticMesh archiveMesh = ArchiveStaticMesh(m_importer->registerAsset(AssetType::StaticMesh, mesh->mName.C_Str()));
	archiveMesh.batches.append(std::move(archiveBatch));
	
	return addStaticMesh(m_scene, std::move(archiveMesh));
}

ArchiveMaterial AssimpImporterImpl::processMaterial(aiMaterial* material)
{
	ArchiveMaterial archiveMaterial;
	//aiTextureType_EMISSION_COLOR = 14,
	//aiTextureType_METALNESS = 15,
	//aiTextureType_DIFFUSE_ROUGHNESS = 16,
	//aiTextureType_AMBIENT_OCCLUSION = 17,
	archiveMaterial = ArchiveMaterial(m_importer->registerAsset(AssetType::Material, material->GetName().C_Str()));
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
	return archiveMaterial;
}

ArchiveImage AssimpImporterImpl::processImage(const Path& path)
{
	ArchiveImage image(m_importer->registerAsset(AssetType::Image, OS::File::basename(path)));

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
		//aiProcess_JoinIdenticalVertices |
		aiProcess_RemoveRedundantMaterials |
		aiProcess_FindInstances | 
		//aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph |
		aiProcess_GenSmoothNormals;
}


AssimpImporter::AssimpImporter(aka::AssetLibrary* _library) :
	Importer(_library)
{
}

ImportResult AssimpImporter::import(const aka::Path & path)
{
	createAssimpLogger();
	Assimp::Importer assimpImporter;
	const aiScene* aiScene = assimpImporter.ReadFile(path.cstr(), getAssimpFlags());
	if (!aiScene || aiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !aiScene->mRootNode)
	{
		Logger::error("[assimp] ", assimpImporter.GetErrorString());
		return ImportResult::CouldNotReadFile;
	}
	AssimpImporterImpl importer(this, path.up(), aiScene);
	importer.process();
	Assimp::DefaultLogger::kill();
	return ImportResult::Succeed;
}

ImportResult AssimpImporter::import(const aka::Blob & blob)
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
	AssimpImporterImpl importer(directory, _library, aiScene);
	importer.process();
	Assimp::DefaultLogger::kill();
	return ImportResult::Succeed;
#endif
}

}