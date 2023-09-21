#pragma once

#include <Aka/Resource/Importer/Importer.hpp>

namespace app {

class AssimpImporter : public aka::Importer {
public:
	aka::ImportResult import(aka::AssetLibrary * _library, const aka::Path & path) override;
	aka::ImportResult import(aka::AssetLibrary * _library, const aka::Blob & blob) override;
};

};