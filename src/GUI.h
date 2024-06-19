#pragma once
// #include <GL/glew.h>
// #include "Application.h"
#include <SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

/* #include "scene.h" */

extern const char* glsl_version;

namespace RT {
	/* extern SDL_Window *window; */
	/* extern SDL_GLContext gl_context; */
	/* extern bool shouldQuit; */
        /* extern bool animationRenderWindowVisible; */
	
	class GUI {
		// extern SDL_Window *window;
		// extern SDL_GLContext gl_context;
		// extern bool shouldQuit;
		// extern bool animationRenderWindowVisible;

	public:
                GUI(SDL_Window* window);
                ~GUI();
		
		void Init();
                void Cleanup();

		void DockspaceUI();

		void Run();

	private:
		SDL_Window *m_WindowHandle = nullptr;
		SDL_GLContext m_glContext = NULL;
		bool shouldQuit = false;
		bool animationRenderWindowVisible = false; // should figure out what this is
	};


} // namespace RT
