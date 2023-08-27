#pragma once

#include <Aka/Aka.h>

#include "Archive.hpp"
#include "ArchiveBatch.hpp"

namespace app {
using namespace aka;

enum class ArchiveStaticMeshVersion : uint32_t
{
	ArchiveCreation = 0,

	Latest = ArchiveCreation
};

struct ArchiveStaticMesh : Archive {
	ArchiveStaticMesh(const ArchivePath& path) : Archive(path) {}

	ArchiveLoadResult load(const ArchivePath& path) override;
	ArchiveSaveResult save(const ArchivePath& path) override;

	// LOD will be generated at runtime, maybe cache it somewhere for perf...
	// Use simplygon or mesh opt...
	Vector<ArchiveBatch> batches;
};

}