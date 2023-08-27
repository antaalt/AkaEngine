#include "ArchiveBatch.hpp"

#include <Aka/OS/Archive.h>

namespace app {

ArchiveLoadResult ArchiveBatch::load(const ArchivePath& path)
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
		ArchivePath path = ArchivePath::read(archive);

		this->material = ArchiveMaterial(path);
		ArchiveLoadResult result = this->material.load(path);
		if (result != ArchiveLoadResult::Success)
			return ArchiveLoadResult::InvalidDependency;
	}
	{
		ArchivePath path = ArchivePath::read(archive);

		this->geometry = ArchiveGeometry(ArchivePath(path));
		ArchiveLoadResult result = this->geometry.load(ArchivePath(path));
		if (result != ArchiveLoadResult::Success)
			return ArchiveLoadResult::InvalidDependency;
	}


	return ArchiveLoadResult::Success;
}

ArchiveSaveResult ArchiveBatch::save(const ArchivePath& path)
{
	FileStream stream(path.getPath(), FileMode::Write, FileType::Binary);
	BinaryArchive archive(stream);

	// Write header
	char signature[4] = { 'a', 'k', 'a', 'b' };
	archive.write<char>(signature, 4);
	archive.write<ArchiveBatchVersion>(ArchiveBatchVersion::Latest);

	{
		ArchivePath path = this->material.getPath();
		ArchivePath::write(archive, path);

		ArchiveSaveResult result = this->material.save(ArchivePath(path));
		if (result != ArchiveSaveResult::Success)
			return ArchiveSaveResult::InvalidDependency;
	}
	{
		ArchivePath path = this->geometry.getPath();
		ArchivePath::write(archive, path);

		ArchiveSaveResult result = this->geometry.save(ArchivePath(path));
		if (result != ArchiveSaveResult::Success)
			return ArchiveSaveResult::InvalidDependency;
	}

	return ArchiveSaveResult::Success;
}

}