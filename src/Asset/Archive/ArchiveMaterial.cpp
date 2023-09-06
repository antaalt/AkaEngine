#include "ArchiveMaterial.hpp"

#include <Aka/OS/Archive.h>

#include "../AssetLibrary.hpp"

namespace app {

ArchiveLoadResult ArchiveMaterial::load(AssetLibrary* _library, const AssetPath& path)
{
	FileStream stream(path.getPath(), FileMode::Read, FileType::Binary);
	BinaryArchive archive(stream);

	// Read header
	char sign[4];
	archive.read<char>(sign, 4);
	if (sign[0] != 'a' || sign[1] != 'k' || sign[2] != 'a' || sign[3] != 'm')
		return ArchiveLoadResult::InvalidMagicWord;
	ArchiveMaterialVersion version = archive.read<ArchiveMaterialVersion>();
	if (version > ArchiveMaterialVersion::Latest)
		return ArchiveLoadResult::IncompatibleVersion;

	archive.read(this->color);

	{
		AssetID albedoID = archive.read<AssetID>();
		AssetInfo info = _library->getAssetInfo(albedoID);
		this->albedo.load(_library, info.path);
	}

	{
		AssetID normalID = archive.read<AssetID>();
		AssetInfo info = _library->getAssetInfo(normalID);
		this->normal.load(_library, info.path);
	}

	return ArchiveLoadResult::Success;
}

ArchiveSaveResult ArchiveMaterial::save(AssetLibrary* _library, const AssetPath& path)
{
	FileStream stream(path.getPath(), FileMode::Write, FileType::Binary);
	BinaryArchive archive(stream);

	// Write header
	char signature[4] = { 'a', 'k', 'a', 'm' };
	archive.write<char>(signature, 4);
	archive.write<ArchiveMaterialVersion>(ArchiveMaterialVersion::Latest);

	archive.write(this->color);

	{
		archive.write<AssetID>(this->albedo.id());
		AssetInfo info = _library->getAssetInfo(this->albedo.id());
		this->albedo.save(_library, info.path);
	}

	{
		archive.write<AssetID>(this->normal.id());
		AssetInfo info = _library->getAssetInfo(this->normal.id());
		this->normal.save(_library, info.path);
	}

	return ArchiveSaveResult::Success;
}

}