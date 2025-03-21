#include <Aka/Core/Application.h>

#include "Editor.h"

// TODO:
// - hide program() behind controls from Application & Layer. Debug draw could be a layer, same as instance renderers. Would need some ordering & such.

int main(int argc, char* argv[])
{
	aka::Config cfg;
	cfg.graphic.api = aka::gfx::GraphicAPI::Vulkan;
	cfg.platform.name = "Aka editor";
	cfg.platform.width = 1920;
	cfg.platform.height = 1080;
	cfg.argc = argc;
	cfg.argv = argv;

	Editor editor(cfg);

	aka::Application::run(&editor);
	return 0;
}
