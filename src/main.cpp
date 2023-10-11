#include <Aka/Core/Application.h>

#include "Editor.h"

int main(int argc, char* argv[])
{
	aka::Config cfg;
	cfg.graphic.api = aka::gfx::GraphicAPI::Vulkan;
	cfg.graphic.connectRenderDoc = true;
	cfg.platform.name = "Aka editor";
	cfg.platform.width = 1280;
	cfg.platform.height = 720;
	cfg.argc = argc;
	cfg.argv = argv;
	cfg.directory = "./";

	Editor editor(cfg);

	aka::Application::run(&editor, cfg);
	return 0;
}