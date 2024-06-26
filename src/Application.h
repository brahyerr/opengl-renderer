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
	
	const std::chrono::high_resolution_clock::time_point time_start = std::chrono::high_resolution_clock::now();

        class Application {
        private:
		typedef int VBOInt;
		struct ImageData {
			int width, height, nrChannels;
			std::vector<unsigned int> texture;
			unsigned char* data;
		} Image;
                struct RenderData {
			std::vector<glm::vec3> vert;
			std::vector<glm::vec2> uv;
                        std::vector<VBOInt> off;
			std::vector<GLuint> idx;
                } RenderData;

                struct Vertex {
			glm::vec3 pos;
			glm::vec2 uv;
		};

        public:
		Application(const ApplicationSpecification &applicationSpecification = ApplicationSpecification());
                ~Application();
		
		void Init();
                void Shutdown();
                void Run();

		void PollEvent(glm::mat4 *matrix);
                void KeyCallback();
                void MouseButtonCallback();

		float GetTime();

                // void Render();
		void GenCircle(float radius, int vertCount, struct RenderData* RenderData);
		void GenQuad(float scale, float width, float height, struct RenderData* RenderData, float z = 0.0f);
		void GenTri(float scale, float top, float right, float left, struct RenderData* RenderData);
		void GenCube(float scale, float width, float height, float length, struct RenderData* RenderData);
		void GenTexture(struct ImageData* imageData, std::string path, int index);
		GLuint CreateShaderProgram(const char* vertex_file_path, const char* fragment_file_path);

		// 		void GenCircle(float radius, int vertCount, std::vector<glm::vec3>* vertices, std::vector<glm::vec2>* uv = nullptr);
		// void GenQuad(float scale, float width, float height, std::vector<glm::vec3>* vertices, std::vector<glm::vec2>* uv = nullptr);
		// void GenTri(float scale, float top, float right, float left, std::vector<glm::vec3>* vertices, std::vector<glm::vec2>* uv = nullptr);

		// void SetMenubarCallback(const std::function<void()>& menubarCallback) { m_MenubarCallback = menubarCallback; }
	private:
		ApplicationSpecification m_Specification;
                SDL_Window *m_WindowHandle = nullptr;
		// SDL_Renderer* m_Renderer = nullptr;
                SDL_GLContext m_glContext = NULL;
		
		const glm::mat4 Identity = glm::mat4(1.0);
		// Most of these fields should be moved to different classes
                GLuint m_ShaderProgram;
                // std::vector<glm::vec3> vertices;
		// std::vector<glm::vec2> uv;
		std::vector<GLuint> vao, vbo, eab;
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
