#pragma once

namespace app {

struct Archive {
	virtual void load(const Path& path) = 0;
	virtual void save(const Path& path) = 0;
};

}