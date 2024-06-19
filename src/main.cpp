#include "EntryPoint.h"
#include <iostream>
// #include <glm/glm.hpp>
// #include <SDL.h>
// #include <SDL_opengl.h>

RT::Application* RT::CreateApplication(int argc, char **argv)
{
	RT::ApplicationSpecification spec;
	spec.Name = "Raytracer!";
	spec.Width = 1024;
	spec.Height = 768;

	RT::Application *app = new RT::Application(spec);

	// Callback functions here?
	return app;
}
