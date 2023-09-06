#pragma once

#include <Aka/Aka.h>

#include "Archive.hpp"
#include "ArchiveImage.hpp"

namespace app {
using namespace aka;

enum class ArchiveMaterialVersion : uint32_t
{
	ArchiveCreation = 0,

	Latest = ArchiveCreation
};

struct ArchiveMaterial : Archive
{
	ArchiveMaterial() {}
	ArchiveMaterial(const AssetID& id) : Archive(id) {}
	ArchiveLoadResult load(AssetLibrary* _library, const AssetPath& path) override;
	ArchiveSaveResult save(AssetLibrary* _library, const AssetPath& path) override;

	color4f color;
	ArchiveImage albedo;
	ArchiveImage normal;
};

}