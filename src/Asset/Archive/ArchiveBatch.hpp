#pragma once

#include <Aka/Aka.h>

#include "Archive.hpp"
#include "ArchiveMaterial.hpp"
#include "ArchiveGeometry.hpp"

namespace app {
using namespace aka;


enum class ArchiveBatchVersion : uint32_t
{
	ArchiveCreation = 0,

	Latest = ArchiveCreation
};

struct ArchiveBatch : Archive
{
	ArchiveBatch() {}
	ArchiveBatch(const ArchivePath& path) : Archive(path), material(ArchivePath::invalid()), geometry(ArchivePath::invalid()) {}

	ArchiveLoadResult load(const ArchivePath& path) override;
	ArchiveSaveResult save(const ArchivePath& path) override;

	ArchiveMaterial material;
	ArchiveGeometry geometry;
};

}