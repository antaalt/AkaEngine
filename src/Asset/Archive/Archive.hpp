#pragma once

#include <Aka/OS/Path.h>
#include <Aka/OS/Archive.h> // TODO remove with cpp code

#include "../Asset.hpp"

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

class AssetLibrary;

struct Archive {
	Archive() : m_id(AssetID::Invalid) {}
	Archive(AssetID id) : m_id(id) {}

	virtual ArchiveLoadResult load(AssetLibrary* _library, const AssetPath& path) = 0;
	virtual ArchiveSaveResult save(AssetLibrary* _library, const AssetPath& path) = 0;

	AssetID id() const { return m_id; }
protected:
	//void setPath(const ArchivePath& path);
	//void readHeader();
	//void writeHeader();
	//void getMagicNumber();
private:
	AssetID m_id;
};

}