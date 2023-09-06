#include "ArchiveGeometry.hpp"

#include <Aka/OS/Archive.h>

#include "../AssetLibrary.hpp"

namespace app {

ArchiveLoadResult ArchiveGeometry::load(AssetLibrary* _library, const AssetPath& path)
{
	FileStream stream(path.getPath(), FileMode::Read, FileType::Binary);
	BinaryArchive archive(stream);

	// Read header
	char sign[4];
	archive.read<char>(sign, 4);
	if (sign[0] != 'a' || sign[1] != 'k' || sign[2] != 'a' || sign[3] != 'g')
		return ArchiveLoadResult::InvalidMagicWord;
	ArchiveGeometryVersion version = archive.read<ArchiveGeometryVersion>();
	if (version > ArchiveGeometryVersion::Latest)
		return ArchiveLoadResult::IncompatibleVersion;

	// Bounds
	archive.read(this->bounds.min);
	archive.read(this->bounds.max);

	// Indices
	uint32_t indexCount = archive.read<uint32_t>();
	this->indices.resize(indexCount);
	archive.read(this->indices.data(), indexCount);

	// Vertices
	uint32_t vertexCount = archive.read<uint32_t>();
	this->vertices.resize(vertexCount);
	archive.read(this->vertices.data(), vertexCount);

	return ArchiveLoadResult::Success;
}

ArchiveSaveResult ArchiveGeometry::save(AssetLibrary* _library, const AssetPath& path)
{
	FileStream stream(path.getPath(), FileMode::Write, FileType::Binary);
	BinaryArchive archive(stream);

	// Write header
	char signature[4] = { 'a', 'k', 'a', 'g' };
	archive.write<char>(signature, 4);
	archive.write<ArchiveGeometryVersion>(ArchiveGeometryVersion::Latest);

	// Bounds
	archive.write(this->bounds.min);
	archive.write(this->bounds.max);

	archive.write<uint32_t>((uint32_t)this->indices.size());
	archive.write<uint32_t>(this->indices.data(), this->indices.size());

	archive.write<uint32_t>((uint32_t)this->vertices.size());
	archive.write<Vertex>(this->vertices.data(), this->vertices.size());

	return ArchiveSaveResult::Success;
}

}