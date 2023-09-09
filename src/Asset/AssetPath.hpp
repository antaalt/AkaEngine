#pragma once

#include <Aka/OS/Path.h>

namespace app {

struct AssetPath
{
	AssetPath();
	AssetPath(const char* _path);
	AssetPath(const aka::String& _path);
	AssetPath(const AssetPath&) = default;
	AssetPath(AssetPath&&) = default;
	AssetPath& operator=(const AssetPath&) = default;
	AssetPath& operator=(AssetPath&&) = default;
	~AssetPath() = default;


	bool operator==(const AssetPath& _path) const;
	bool operator!=(const AssetPath& _path) const;

	// Get the absolute path of the data
	const aka::Path getAbsolutePath() const;

	const char* cstr() const;
	size_t size() const;

	static aka::Path getAssetPath();
private:
	aka::Path m_path;
};

};