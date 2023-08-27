#include "ArchiveImage.hpp"

#include <Aka/OS/Archive.h>

namespace app {

ArchiveLoadResult ArchiveImage::load(const ArchivePath& path)
{
	FileStream stream(path.getPath(), FileMode::Read, FileType::Binary);
	BinaryArchive archive(stream);

	// Read header
	char sign[4];
	archive.read<char>(sign, 4);
	if (sign[0] != 'a' || sign[1] != 'k' || sign[2] != 'a' || sign[3] != 'i')
		return ArchiveLoadResult::InvalidMagicWord;
	ArchiveImageVersion version = archive.read<ArchiveImageVersion>();
	if (version > ArchiveImageVersion::Latest)
		return ArchiveLoadResult::IncompatibleVersion;

	this->width = archive.read<uint32_t>();
	this->height = archive.read<uint32_t>();
	this->channels = archive.read<uint32_t>();

	uint32_t compressedSize = archive.read<uint32_t>();
	Vector<uint8_t> data(compressedSize);
	archive.read(data.data(), compressedSize);

	Image image;
	if (!image.decode(data.data(), data.size()))
		return ArchiveLoadResult::Failed;

	this->data.resize(image.size());
	Memory::copy(this->data.data(), image.data(), image.size());

	return ArchiveLoadResult::Success;
}

ArchiveSaveResult ArchiveImage::save(const ArchivePath& path)
{
	FileStream stream(path.getPath(), FileMode::Write, FileType::Binary);
	BinaryArchive archive(stream);

	// Write header
	char signature[4] = { 'a', 'k', 'a', 'i' };
	archive.write<char>(signature, 4);
	archive.write<ArchiveImageVersion>(ArchiveImageVersion::Latest);

	archive.write<uint32_t>(this->width);
	archive.write<uint32_t>(this->height);
	archive.write<uint32_t>(this->channels);

	Image image(this->width, this->height, this->channels, this->data.data());
	std::vector<uint8_t> encodedData = image.encodePNG();
	if (encodedData.empty())
		return ArchiveSaveResult::Failed;

	archive.write<uint32_t>(encodedData.size());
	archive.write(encodedData.data(), encodedData.size());

	return ArchiveSaveResult::Success;
}

}