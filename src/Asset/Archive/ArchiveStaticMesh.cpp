#include "ArchiveStaticMesh.hpp"

#include <Aka/OS/Archive.h>

#include "../AssetLibrary.hpp"

namespace app {

ArchiveLoadResult ArchiveStaticMesh::load(AssetLibrary* _library, const AssetPath& path)
{
	FileStream stream(path.getPath(), FileMode::Read, FileType::Binary);
	BinaryArchive archive(stream);

	// Read header
	char sign[4];
	archive.read<char>(sign, 4);
	if (sign[0] != 'a' || sign[1] != 'k' || sign[2] != 'a' || sign[3] != 's')
		return ArchiveLoadResult::InvalidMagicWord;
	ArchiveStaticMeshVersion version = archive.read<ArchiveStaticMeshVersion>();
	if (version > ArchiveStaticMeshVersion::Latest)
		return ArchiveLoadResult::IncompatibleVersion;

	uint32_t batchCount = archive.read<uint32_t>();
	for (uint32_t i = 0; i < batchCount; i++)
	{
		AssetID batchID = archive.read<AssetID>();
		AssetInfo info = _library->getAssetInfo(batchID);
		this->batches.append(ArchiveBatch(batchID));
		this->batches.last().load(_library, info.path);
	}

	return ArchiveLoadResult::Success;
}

ArchiveSaveResult ArchiveStaticMesh::save(AssetLibrary* _library, const AssetPath& path)
{
	FileStream stream(path.getPath(), FileMode::Write, FileType::Binary);
	BinaryArchive archive(stream);

	// Write header
	char signature[4] = { 'a', 'k', 'a', 's' };
	archive.write<char>(signature, 4);
	archive.write<ArchiveStaticMeshVersion>(ArchiveStaticMeshVersion::Latest);

	archive.write<uint32_t>((uint32_t)this->batches.size());
	for (uint32_t i = 0; i < this->batches.size(); i++)
	{
		archive.write<AssetID>(this->batches[i].id());
		AssetInfo info = _library->getAssetInfo(this->batches[i].id());
		this->batches[i].save(_library, info.path);
	}

	return ArchiveSaveResult::Success;
}

}