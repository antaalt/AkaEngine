#include "ArchiveMaterial.hpp"

#include <Aka/OS/Archive.h>

namespace app {

ArchiveLoadResult ArchiveMaterial::load(const ArchivePath& path)
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

	// TODO implement material

	return ArchiveLoadResult::Success;
}

ArchiveSaveResult ArchiveMaterial::save(const ArchivePath& path)
{
	FileStream stream(path.getPath(), FileMode::Write, FileType::Binary);
	BinaryArchive archive(stream);

	// Write header
	char signature[4] = { 'a', 'k', 'a', 'm' };
	archive.write<char>(signature, 4);
	archive.write<ArchiveMaterialVersion>(ArchiveMaterialVersion::Latest);

	// TODO implement material

	return ArchiveSaveResult::Success;
}

}