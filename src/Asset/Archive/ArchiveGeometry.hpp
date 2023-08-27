#pragma once

#include <Aka/Aka.h>

#include "Archive.hpp"

namespace app {
using namespace aka;


enum class ArchiveGeometryVersion : uint32_t
{
	ArchiveCreation = 0,

	Latest = ArchiveCreation
};

struct Vertex {
	float position[3];
	float uv[2];
};

struct ArchiveGeometry : Archive 
{
	ArchiveGeometry() {}
	ArchiveGeometry(const ArchivePath& path) : Archive(path) {}

	ArchiveLoadResult load(const ArchivePath& path) override;
	ArchiveSaveResult save(const ArchivePath& path) override;

	Vector<Vertex> vertices;
	Vector<uint32_t> indices;
	aabbox<> bounds;
};

}