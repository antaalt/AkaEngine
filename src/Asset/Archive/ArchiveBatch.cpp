#include "ArchiveBatch.hpp"

#include <Aka/OS/Archive.h>

#include "../AssetLibrary.hpp"

namespace app {

ArchiveLoadResult ArchiveBatch::load(AssetLibrary* _library, const AssetPath& path)
{
	FileStream stream(path.getPath(), FileMode::Read, FileType::Binary);
	BinaryArchive archive(stream);

	// Read header
	char sign[4];
	archive.read<char>(sign, 4);
	if (sign[0] != 'a' || sign[1] != 'k' || sign[2] != 'a' || sign[3] != 'b')
		return ArchiveLoadResult::InvalidMagicWord;
	ArchiveBatchVersion version = archive.read<ArchiveBatchVersion>();
	if (version > ArchiveBatchVersion::Latest)
		return ArchiveLoadResult::IncompatibleVersion;
	{
		AssetID materialID = archive.read<AssetID>();

		this->material = ArchiveMaterial(materialID);
		AssetInfo info = _library->getAssetInfo(materialID);
		ArchiveLoadResult result = this->material.load(_library, info.path);
		if (result != ArchiveLoadResult::Success)
			return ArchiveLoadResult::InvalidDependency;
	}
	{
		AssetID geometryID = archive.read<AssetID>();

		this->geometry = ArchiveGeometry(geometryID);
		AssetInfo info = _library->getAssetInfo(geometryID);
		ArchiveLoadResult result = this->geometry.load(_library, info.path);
		if (result != ArchiveLoadResult::Success)
			return ArchiveLoadResult::InvalidDependency;
	}


	return ArchiveLoadResult::Success;
}

ArchiveSaveResult ArchiveBatch::save(AssetLibrary* _library, const AssetPath& path)
{
	FileStream stream(path.getPath(), FileMode::Write, FileType::Binary);
	BinaryArchive archive(stream);

	// Write header
	char signature[4] = { 'a', 'k', 'a', 'b' };
	archive.write<char>(signature, 4);
	archive.write<ArchiveBatchVersion>(ArchiveBatchVersion::Latest);

	{
		archive.write<AssetID>(this->material.id());
		AssetInfo info = _library->getAssetInfo(this->material.id());
		ArchiveSaveResult result = this->material.save(_library, info.path);
		if (result != ArchiveSaveResult::Success)
			return ArchiveSaveResult::InvalidDependency;
	}
	{
		archive.write<AssetID>(this->geometry.id());
		AssetInfo info = _library->getAssetInfo(this->geometry.id());
		ArchiveSaveResult result = this->geometry.save(_library, info.path);
		if (result != ArchiveSaveResult::Success)
			return ArchiveSaveResult::InvalidDependency;
	}

	return ArchiveSaveResult::Success;
}

}