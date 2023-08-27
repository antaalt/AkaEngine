#pragma once

#include <Aka/Aka.h>

#include "Archive.hpp"

namespace app {
using namespace aka;

enum class ArchiveImageVersion : uint32_t
{
	ArchiveCreation = 0,

	Latest = ArchiveCreation
};

struct ArchiveImage : Archive
{
	ArchiveImage() {}
	ArchiveImage(const ArchivePath& path) : Archive(path) {}
	ArchiveLoadResult load(const ArchivePath& blob) override;
	ArchiveSaveResult save(const ArchivePath& blob) override;

	Vector<uint8_t> data;
	uint32_t width;
	uint32_t height;
	uint32_t channels;
};

}