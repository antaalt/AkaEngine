#pragma once

namespace app {
// .font->hold the ttf to be parsed or other depending on method(check valve paper with sdf).
//
struct ArchiveFont : Archive {
	Blob ttf;
	void load(const Path& path) override;
	void save(const Path& path) override;
};
struct FontMesh : Resource {
public:
	void create(const ArchiveStaticMesh& _archive);
	void destroy(const ArchiveStaticMesh& _archive);

};

}