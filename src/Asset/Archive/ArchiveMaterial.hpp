#pragma once

#include <Aka/Aka.h>

#include "Archive.hpp"

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
	ArchiveMaterial(const ArchivePath& path) : Archive(path) {}
	ArchiveLoadResult load(const ArchivePath& blob) override;
	ArchiveSaveResult save(const ArchivePath& blob) override;

};

}