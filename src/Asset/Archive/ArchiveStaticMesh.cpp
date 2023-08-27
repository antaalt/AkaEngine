#include "ArchiveStaticMesh.hpp"

#include <Aka/OS/Archive.h>

namespace app {

ArchiveLoadResult ArchiveStaticMesh::load(const ArchivePath& path)
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
		ArchivePath path = ArchivePath::read(archive);
		this->batches.append(ArchiveBatch(path));
		this->batches.last().load(path);
	}

	return ArchiveLoadResult::Success;
}

ArchiveSaveResult ArchiveStaticMesh::save(const ArchivePath& path)
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
		ArchivePath batchPath = this->batches[i].getPath();
		ArchivePath::write(archive, batchPath);
		this->batches[i].save(batchPath);
	}

	return ArchiveSaveResult::Success;
}

}