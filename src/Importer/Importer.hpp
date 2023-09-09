#pragma once

#include <Aka/Aka.h>

#include "../Asset/AssetLibrary.hpp"

namespace app {

enum class ImportResult {
	Succeed,

	CouldNotReadFile,

	Failed,
};

class Importer {
public:
	virtual ImportResult import(AssetLibrary* _library, const aka::Path & path) = 0;
	virtual ImportResult import(AssetLibrary* _library, const aka::Blob& blob) = 0;
};

class StbiImporter {

};


class FreeTypeImporter {

};

class AudioImporter {

};

}