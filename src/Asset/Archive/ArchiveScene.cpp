#include "ArchiveScene.hpp"

#include "../AssetLibrary.hpp"

namespace app {

ArchiveLoadResult app::ArchiveScene::load(AssetLibrary* _library, const AssetPath& path)
{
	// Archive opened by lirbary ?
	// 
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

	// TODO: should write assetID aswell

	// Before loading entities, load all components that will be used in the scene.
	{
		uint32_t nbTransform = archive.read<uint32_t>();
		for (uint32_t iTransform = 0; iTransform < nbTransform; iTransform++)
		{
			ArchiveSceneTransform transform{};
			transform.matrix = archive.read<mat4f>();
			this->transforms.append(transform);
		}
	}
	{
		// hierarchy only need an ID, do not store it.
	}
	{
		uint32_t nbStaticMesh = archive.read<uint32_t>();
		for (uint32_t iMesh = 0; iMesh < nbStaticMesh; iMesh++)
		{
			AssetID assetID = archive.read<AssetID>();
			AssetInfo info = _library->getAssetInfo(assetID);
			this->meshes.append(ArchiveStaticMesh(assetID));
			// TODO cache
			this->meshes.last().load(_library, info.path);
		}
	}

	uint32_t nbEntity = archive.read<uint32_t>();
	for (uint32_t iEntity = 0; iEntity < nbEntity; iEntity++)
	{
		ArchiveSceneEntity entity{};
		entity.components = archive.read<SceneComponentMask>();

		for (uint32_t i = 0; i < EnumCount<SceneComponent>(); i++)
		{
			if (asBool(static_cast<SceneComponentMask>(1 << i) & entity.components))
			{
				entity.id[i] = archive.read<ArchiveSceneID>();
			}
			else
			{
				entity.id[i] = InvalidArchiveSceneID;
			}
			this->entities.append(entity);
		}
	}

	return ArchiveLoadResult::Success;
}

ArchiveSaveResult app::ArchiveScene::save(AssetLibrary* _library, const AssetPath& path)
{
	FileStream stream(path.getPath(), FileMode::Write, FileType::Binary);
	BinaryArchive archive(stream);

	// Write header
	char signature[4] = { 'a', 'k', 'a', 'w' };
	archive.write<char>(signature, 4);
	archive.write<ArchiveSceneVersion>(ArchiveSceneVersion::Latest);

	{
		archive.write<uint32_t>((uint32_t)this->transforms.size());
		for (size_t iTransform = 0; iTransform < this->transforms.size(); iTransform++)
		{
			archive.write<mat4f>(this->transforms[iTransform].matrix);
		}
	}
	{
		// hierarchy only need an ID, do not store it.
	}
	{
		archive.write<uint32_t>((uint32_t)this->meshes.size());
		for (size_t iMesh = 0; iMesh < this->meshes.size(); iMesh++)
		{
			archive.write<AssetID>(this->meshes[iMesh].id());
		}
	}

	archive.write<uint32_t>((uint32_t)this->entities.size());
	for (size_t iEntity = 0; iEntity < this->entities.size(); iEntity++)
	{
		ArchiveSceneEntity& entity = this->entities[iEntity];
		archive.write<SceneComponentMask>(entity.components);

		for (uint32_t i = 0; i < EnumCount<SceneComponent>(); i++)
		{
			SceneComponentMask mask = static_cast<SceneComponentMask>(1 << i);
			if (asBool(mask & entity.components))
			{
				archive.write<ArchiveSceneID>(entity.id[i]);
			}
		}
	}

	return ArchiveSaveResult::Success;
}

};