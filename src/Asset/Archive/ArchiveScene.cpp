#include "ArchiveScene.hpp"

namespace app {

ArchiveLoadResult app::ArchiveScene::load(const ArchivePath& path)
{
	// Quid de si ça a déjà chargé ? Archive cache ?
	// Only big resources such as mesh & co could be efficiently cached. 
	// Should read registry when loading mesh
	FileStream stream(path.getPath(), FileMode::Read, FileType::Binary);
	BinaryArchive archive(stream);

	// Read header
	char sign[4];
	archive.read<char>(sign, 4);
	if (sign[0] != 'a' || sign[1] != 'k' || sign[2] != 'a' || sign[3] != 'w')
		return ArchiveLoadResult::InvalidMagicWord;
	ArchiveSceneVersion version = archive.read<ArchiveSceneVersion>();
	if (version > ArchiveSceneVersion::Latest)
		return ArchiveLoadResult::IncompatibleVersion;

	// Before loading entities, load all components that will be used in the scene.
	// TODO: Some of these data should be cached somehow
	// Or we just save the path / assetID & load it separately...
	// Entities just reference components using ID
	uint32_t nbStaticMesh = archive.read<uint32_t>();
	for (uint32_t iMesh = 0; iMesh < nbStaticMesh; iMesh++)
	{
		ArchivePath path = ArchivePath::read(archive);
		// TODO cache this somehow, request registry before loading.
		this->meshes.append(ArchiveStaticMesh(path));
		this->meshes.last().load(path);
	}

	uint32_t nbEntity = archive.read<uint32_t>();
	for (uint32_t iEntity = 0; iEntity < nbEntity; iEntity++)
	{
		// For each entity,
		SceneComponent components = archive.read<SceneComponent>();
		if (asBool(components & SceneComponent::Transform))
		{
			ArchiveSceneID parentID = archive.read<ArchiveSceneID>();
			//mat4f transform = archive.read<mat4f>();
		}
		if (asBool(components & SceneComponent::Hierarchy))
		{
			ArchiveSceneID parentID = archive.read<ArchiveSceneID>();
		}
		if (asBool(components & SceneComponent::StaticMesh))
		{
			// This is meshID from local array.
			ArchiveSceneID meshID = archive.read<ArchiveSceneID>();

		}
	}

	return ArchiveLoadResult::Success;
}

ArchiveSaveResult app::ArchiveScene::save(const ArchivePath& path)
{
	FileStream stream(path.getPath(), FileMode::Write, FileType::Binary);
	BinaryArchive archive(stream);

	// Write header
	char signature[4] = { 'a', 'k', 'a', 'w' };
	archive.write<char>(signature, 4);
	archive.write<ArchiveSceneVersion>(ArchiveSceneVersion::Latest);


	return ArchiveSaveResult::Success;
}

};