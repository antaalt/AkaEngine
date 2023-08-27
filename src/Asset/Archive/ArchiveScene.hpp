#pragma once 

#include "../Archive/ArchiveStaticMesh.hpp"

namespace app {

using namespace aka;

struct ArchiveSceneTransform {
	mat4f matrix;
};

enum class ArchiveSceneMeshID : uint32_t {};

struct ArchiveSceneInstance {
	ArchiveSceneTransform transform;
	ArchiveSceneMeshID meshID;
};

struct ArchiveScene : Archive {
	Vector<ArchiveStaticMesh> meshes;
	Vector<ArchiveSceneInstance> instances;
	// TODO: add lights, envmap, cameras, gameplay struct


	ArchiveLoadResult load(const ArchivePath& path) override;
	ArchiveSaveResult save(const ArchivePath& path) override;
};

};