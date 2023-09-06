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
	ArchiveBatch(AssetID path) : Archive(path), material(AssetID::Invalid), geometry(AssetID::Invalid) {}

	ArchiveLoadResult load(AssetLibrary* _library, const AssetPath& path) override;
	ArchiveSaveResult save(AssetLibrary* _library, const AssetPath& path) override;

	ArchiveMaterial material;
	ArchiveGeometry geometry;
};

}