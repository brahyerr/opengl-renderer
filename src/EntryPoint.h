// Taken from TheCherno/Walnut
// #include <iostream>

#include "Application.h" // TODO: use cmake to not need this include anymore

extern RT::Application *RT::CreateApplication(int argc, char **argv);
bool g_ApplicationRunning = true;

namespace RT {

	int Main(int argc, char** argv)
	{
		while (g_ApplicationRunning) {
			RT::Application* app = RT::CreateApplication(argc, argv);
			if (g_ApplicationRunning) {
				app->Run();
				delete app;
			};
		}
		return 0;
	}

}

#ifdef defined(WL_DIST) && defined(WL_PLATFORM_WINDOWS)

#include <Windows.h>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	return RT::Main(__argc, __argv);
}

#else

int main(int argc, char** argv)
{
	return RT::Main(argc, argv);
}

#endif // WL_DIST && WL_PLATFORM_WINDOWS
