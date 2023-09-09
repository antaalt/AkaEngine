#pragma once
#include "Importer.hpp"

namespace app {

class AssimpImporter : public Importer {
public:
	ImportResult import(AssetLibrary * _library, const aka::Path & path) override;
	ImportResult import(AssetLibrary * _library, const aka::Blob & blob) override;
};

};