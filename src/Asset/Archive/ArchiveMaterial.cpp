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

	archive.read(this->color);

	ArchivePath albedoPath = ArchivePath::read(archive);
	this->albedo.load(albedoPath);

	ArchivePath normalPath = ArchivePath::read(archive);
	this->normal.load(normalPath);

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

	archive.write(this->color);

	ArchivePath::write(archive, this->albedo.getPath());
	this->albedo.save(this->albedo.getPath());

	ArchivePath::write(archive, this->normal.getPath());
	this->normal.save(this->normal.getPath());

	return ArchiveSaveResult::Success;
}

}