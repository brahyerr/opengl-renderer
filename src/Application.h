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

extern bool g_ApplicationRunning;
extern const char* glsl_version;

namespace RT {
	struct ApplicationSpecification {
		const char *Name = "App name";
		int Width = 1280;
		int Height = 720;
        };
	
	// const std::chrono::high_resolution_clock::time_point time_start = std::chrono::high_resolution_clock::now();

        class Application {
        private:
		// typedef int VBOInt;
		// struct ImageData {
		// 	int width, height, nrChannels;
		// 	std::vector<unsigned int> texture;
		// 	unsigned char* data;
		// } Image;

                // struct Vertex {
		// 	glm::vec3 pos;
		// 	glm::vec2 uv;
                // };
		// std::vector<Vertex> vertices;
		// std::vector<GLuint> idx;
        public:
		Application(const ApplicationSpecification &applicationSpecification = ApplicationSpecification());
                ~Application();
		
		void Init();
                void Shutdown();
                void Run();

		void PollEvent();
                void KeyCallback();
                void MouseButtonCallback();

                // void Render();
		// void GenCircle(float radius, int vertCount, std::vector<Vertex> &vertices);
		// void GenQuad(float scale, float width, float height, std::vector<Vertex> &vertices, float z = 0.0f);
		// void GenTri(float scale, float top, float right, float left, std::vector<Vertex> &vertices);
		// void GenCube(float scale, float width, float height, float length, std::vector<Vertex> &vertices);
		// void GenTexture(struct ImageData* imageData, std::string path, int index);

		// void SetMenubarCallback(const std::function<void()>& menubarCallback) { m_MenubarCallback = menubarCallback; }
	private:
		ApplicationSpecification m_Specification;
                SDL_Window *m_WindowHandle = nullptr;
		// SDL_Renderer* m_Renderer = nullptr;
                SDL_GLContext m_glContext = NULL;
		
		const glm::mat4 Identity = glm::mat4(1.0);
		// Most of these fields should be moved to different classes
		std::vector<GLuint> vao, vbo, eab;

		bool m_Running = false;

		float m_TimeStep = 0.0f;
		float m_FrameTime = 0.0f;
                float m_LastFrameTime = 0.0f;

                std::function<void()> m_MenubarCallback;
        };
	// Implemented by CLIENT (aka. in main.cpp)
	Application* CreateApplication(int argc, char** argv);
}
