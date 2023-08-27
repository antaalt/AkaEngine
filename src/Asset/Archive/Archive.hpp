#pragma once

#include <Aka/OS/Path.h>
#include <Aka/OS/Archive.h> // TODO remove with cpp code

namespace app {
using namespace aka;

enum class ArchiveLoadResult {
	Success,

	InvalidMagicWord,
	IncompatibleVersion,
	InvalidDependency,

	Failed,
};

enum class ArchiveSaveResult {
	Success,

	InvalidDependency,

	Failed,
};

struct ArchivePath {
	ArchivePath(const Path& path) : m_path(path) {}

	uint32_t length() const { return (uint32_t)m_path.size(); }
	char* cstr() { return m_path.cstr(); }
	const char* cstr() const { return m_path.cstr(); }

	Path getPath() const { return m_path; }

	static ArchivePath invalid() { return Path(""); }

	static ArchivePath read(BinaryArchive& archive) {
		uint32_t pathLength = archive.read<uint32_t>();
		String path;
		path.resize(pathLength);
		archive.read<String::Char>(path.cstr(), pathLength);
		path[pathLength] = '\0';
		return ArchivePath(path);
	}
	static void write(BinaryArchive& archive, const ArchivePath& path) {
		AKA_ASSERT(path.length() > 0, "Invalid path set");
		archive.write<uint32_t>(path.length());
		archive.write<String::Char>(path.cstr(), path.length());
	}

	Path m_path;
};

struct Archive {
	Archive() : m_path(ArchivePath::invalid()) {}
	Archive(const ArchivePath& path) : m_path(path) {}

	virtual ArchiveLoadResult load(const ArchivePath& path) = 0;
	virtual ArchiveSaveResult save(const ArchivePath& path) = 0;

	ArchivePath getPath() const { return m_path; }
protected:
	void setPath(const ArchivePath& path);
	void readHeader();
	void writeHeader();
	void getMagicNumber();
private:
	ArchivePath m_path;
};

}