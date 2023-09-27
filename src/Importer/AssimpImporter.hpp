#pragma once

#include <Aka/Resource/Importer/Importer.hpp>

namespace app {

class AssimpImporter : public aka::Importer {
public:
	AssimpImporter(aka::AssetLibrary* _library);

	aka::ImportResult import(const aka::Path & path) override;
	aka::ImportResult import(const aka::Blob & blob) override;
	friend class AssimpImporterImpl;
};

};