#include <Aka/Core/Application.h>

#include "Editor.h"

int main(int argc, char* argv[])
{
	Editor editor;

	aka::Config cfg;
	cfg.app = &editor;
	cfg.graphic.api = aka::gfx::GraphicAPI::Vulkan;
	cfg.platform.name = "Aka editor";
	cfg.platform.width = 1280;
	cfg.platform.height = 720;
	cfg.argc = argc;
	cfg.argv = argv;
	cfg.directory = "./";

	aka::Application::run(cfg);
	return 0;
}