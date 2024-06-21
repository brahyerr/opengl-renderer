#include <chrono>
#include <glm/fwd.hpp>
#include <stdlib.h>
/* #include <vector> */
/* #include <memory> */
#include <string>
#include <functional>
#include <glm/glm.hpp>
#include <SDL.h>
#include <SDL_events.h>
#include <SDL_keycode.h>
#include <SDL_opengl.h>
#include "GUI.h"
// #include "Shader.h"

extern bool g_ApplicationRunning;
extern const char* glsl_version;

namespace RT {
	struct ApplicationSpecification {
		const char *Name = "App name";
		int Width = 1280;
		int Height = 720;
        };
	
	struct Image {
		int width, height, nrChannels;
		std::vector<unsigned int> texture;
		unsigned char* data;
	};
	const std::chrono::high_resolution_clock::time_point time_start = std::chrono::high_resolution_clock::now();

        class Application {
        public:
		Application(const ApplicationSpecification &applicationSpecification = ApplicationSpecification());
                ~Application();
		
		void Init();
                void Shutdown();
                void Run();

		void PollEvent();
                void KeyCallback();
                void MouseButtonCallback();

		float GetTime();

                // void Render();
		void GenCircle(float radius, int vertCount, std::vector<glm::vec3>* vertices, std::vector<glm::vec2>* uv);
		void GenTexture(Image* image, std::string path, int index);
		GLuint CreateShaderProgram(const char* vertex_file_path, const char* fragment_file_path);
                // GLuint CreateShaderProgram(const char* vertex_file_path);

		// void SetMenubarCallback(const std::function<void()>& menubarCallback) { m_MenubarCallback = menubarCallback; }
	private:
		ApplicationSpecification m_Specification;
                SDL_Window *m_WindowHandle = nullptr;
		// SDL_Renderer* m_Renderer = nullptr;
                SDL_GLContext m_glContext = NULL;
		
		const glm::mat4 Identity = glm::mat4(1.0);
		// Most of these fields should be moved to different classes
                struct Image image;
                GLuint m_ShaderProgram;
                std::vector<glm::vec3> vertices;
		std::vector<glm::vec2> uv;
		std::vector<GLuint> VAO, VBO, EAB, indices;
		// std::vector<GLuint> UVO;

		bool m_Running = false;

		float m_TimeStep = 0.0f;
		float m_FrameTime = 0.0f;
		float m_LastFrameTime = 0.0f;

		std::function<void()> m_MenubarCallback;
        };
	// Implemented by CLIENT (aka. in main.cpp)
	Application* CreateApplication(int argc, char** argv);
}
