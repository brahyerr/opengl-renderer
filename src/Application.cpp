#include <GL/glew.h>
#include <SDL_keycode.h>
#include <SDL_opengl.h>
#include <SDL_video.h>
#include <cstddef>
#include <ctime>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "Application.h"
#include "imgui_impl_sdl2.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

static RT::Application *s_Instance = nullptr;
static RT::GUI *Gui = nullptr;

static const float vertices[] = {
	0.5f,  0.5f,  0.0f,  1.0f, 0.0f, 0.0f,
	0.5f,  -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
	-0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,
	-0.5f, 0.5f,  0.0f,  1.0f, 0.0f, 1.0f,
	0.0f, 0.9f,  0.0f,  0.8f, 1.0f, 0.5f
};

// static const float vertices[] = {
// 	0.5f,  0.5f,  0.0f,
// 	0.5f,  -0.5f, 0.0f,
// 	-0.5f, -0.5f, 0.0f
//     // -0.5f, 0.5f,  0.0f  // top left
// };

// static const float vertexColors[] = {
//     1.0f,  0.0f, 0.0f,
//     0.0f, 1.0f, 0.0f,
//     0.0f,  0.0f,  1.0f
//     // 0.5f, 0.5f,  0.5f  // top left
// };

static const GLuint indices[] = {
	0, 1, 2, // first triangle
	2, 3, 0, // second triangle
	0, 3, 4  // third triangle
};

namespace RT {
	Application::Application(const ApplicationSpecification &spec)
		: m_Specification(spec) {
		s_Instance = this;
		Init();
	}
	void Application::Init() {
                // if (true) {
		// 	printf("Testing init error flow");
                //         delete this;
		// 	return;
		// }
		
		// Setup SDL
                if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
		{
			// printf("Error: %s\n", SDL_GetError());
			std::cerr << "Error: " << SDL_GetError() << std::endl;
                        delete this;
			return;
		}
		
		// Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
		// GL ES 2.0 + GLSL 100
		// const char* glsl_version = "#version 100";
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
		// GL 3.2 Core + GLSL 150
		const char* glsl_version = "#version 150";
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
		// GL 3.0 + GLSL 130
		// const char* glsl_version = "#version 130";
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3); // if this causes problems, change back to version 0
		// SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

		// From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
		SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif
		// m_Renderer = SDL_CreateRenderer(
                //     m_WindowHandle, -1,
                //     SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
                // SDL_SetRenderDrawColor(m_Renderer, 255, 0, 0, 255);
                // SDL_RenderDrawLine(m_Renderer, 0, 0, 1280, 720);
		// SDL_RenderPresent(m_Renderer);

		// Create window with graphics context
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
		m_WindowHandle = SDL_CreateWindow(
			m_Specification.Name, SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED, m_Specification.Width,
			m_Specification.Height, window_flags);

		if (m_WindowHandle== nullptr)
		{
			printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
                        delete this;
			return;
                }

                m_glContext = SDL_GL_CreateContext(m_WindowHandle);
		if( m_glContext == NULL )
		{
			printf( "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError() );
                        delete this;
			return;
		}
		else
		{
			//Initialize GLEW
			// glewExperimental = GL_TRUE; 
			GLenum glewError = glewInit();
			if( glewError != GLEW_OK )
			{
				printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError));
                                delete this;
				return;
			}

			// Use Vsync
			if( SDL_GL_SetSwapInterval( 1 ) < 0 )
			{
				printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
                                delete this;
				return;
			}

			//Initialize OpenGL
			// if( !initGL() )
			// {
			// 	printf( "Unable to initialize OpenGL!\n" );
			// 	delete this;
			// }
		}
		SDL_GL_MakeCurrent(m_WindowHandle, m_glContext);
                // SDL_GL_SetSwapInterval(1); // Enable vsync
		
                glViewport(0, 0, m_Specification.Width, m_Specification.Height);
                glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
		#ifdef defined(WL_DIST) && defined(WL_PLATFORM_WINDOWS)
		m_ShaderProgram = CreateShaderProgram("shaders\\hello_triangle.vert", "shaders\\hello_triangle.frag");
		#else
		m_ShaderProgram = CreateShaderProgram("shaders/hello_triangle.vert", "shaders/hello_triangle.frag");
		#endif
                // Gui = new GUI(m_WindowHandle);
        }
	
        Application::~Application() {  // TODO: Improve destruction flow
		Shutdown();
                s_Instance = nullptr;
        }
	
	void Application::Shutdown() {
		// delete Gui;
                glDeleteVertexArrays(VAO.size(), VAO.data());
                glDeleteBuffers(VBO.size(), VBO.data());
		glDeleteProgram(m_ShaderProgram);
                SDL_GL_DeleteContext(m_glContext);
                SDL_DestroyWindow(m_WindowHandle);
                SDL_Quit();

		g_ApplicationRunning = false;
        }

        void Application::Run() {
		m_Running = true;
                while (m_Running) {
			PollEvent();
		  
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUseProgram(m_ShaderProgram);
			glBindVertexArray(VAO[0]);
			// glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);

                        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			// glEnable(GL_DEBUG_OUTPUT);
                        // glDrawArrays(GL_TRIANGLES, 0, 3);
			
			glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

			// Gui->Run();
			SDL_GL_SwapWindow(m_WindowHandle);
		};
        }

        void Application::PollEvent() {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			// ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
				m_Running = false;
			if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE) 
				m_Running = false;
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(m_WindowHandle))
				m_Running = false;

		};
        }

        float Application::GetTime() {
		typedef std::chrono::high_resolution_clock clock;
		typedef std::chrono::duration<float, std::milli> duration;

		duration elapsed = clock::now() - time_start;
	  
		return (float) elapsed.count();
        }
	
        GLuint Application::CreateShaderProgram(const char *vertex_file_path, const char *fragment_file_path) {
		// GLuint Application::CreateShaderProgram(const char *vertex_file_path) {
		// TODO: Move var declaration to header
                GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

		// Read the Vertex Shader code from the file
		std::string VertexShaderCode;
		std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
		if (VertexShaderStream.is_open()) {
			std::stringstream sstr;
			sstr << VertexShaderStream.rdbuf();
			VertexShaderCode = sstr.str();
			VertexShaderStream.close();
		}
		else {
			printf("Unable to open %s.\n", vertex_file_path);
			return 0;
		}

		// Read the Fragment Shader code from the file
		std::string FragmentShaderCode;
		std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
		if (FragmentShaderStream.is_open()) {
			std::stringstream sstr;
			sstr << FragmentShaderStream.rdbuf();
			FragmentShaderCode = sstr.str();
			FragmentShaderStream.close();
		}

		GLint Result = GL_FALSE;
		int InfoLogLength;

		// Compile Vertex Shader
		printf("Compiling shader : %s\n", vertex_file_path);
		char const* VertexSourcePointer = VertexShaderCode.c_str();
		glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
		glCompileShader(VertexShaderID);

		// Check Vertex Shader
		glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
			printf("%s\n", &VertexShaderErrorMessage[0]);
		}

		// Compile Fragment Shader
		printf("Compiling shader : %s\n", fragment_file_path);
		char const* FragmentSourcePointer = FragmentShaderCode.c_str();
		glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
		glCompileShader(FragmentShaderID);

		// Check Fragment Shader
		glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
			printf("%s\n", &FragmentShaderErrorMessage[0]);
		}

		// Link the program
		printf("Linking program\n");
		GLuint ProgramID = glCreateProgram();
		glAttachShader(ProgramID, VertexShaderID);
		glAttachShader(ProgramID, FragmentShaderID);
		glLinkProgram(ProgramID);

		// Check the program
		glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
		glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
			glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
			printf("%s\n", &ProgramErrorMessage[0]);
		}

		glDetachShader(ProgramID, VertexShaderID);
		glDetachShader(ProgramID, FragmentShaderID);

		glDeleteShader(VertexShaderID);
                glDeleteShader(FragmentShaderID);

                // LearnOpenGL Stuff below

		VAO.resize(1); VBO.resize(1);
		glGenVertexArrays(VAO.size(), VAO.data());
                glGenBuffers(VBO.size(), VBO.data());
		
                glBindVertexArray(VAO[0]);
                glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0 * (sizeof(float))));
                glEnableVertexAttribArray(0);

		// Color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * (sizeof(float))));
		glEnableVertexAttribArray(1);

		
                // Element array buffer - This is bound automatically to the current VAO, meaning a VAO must be currently bound first
		EAB.resize(1);
                glGenBuffers(EAB.size(), EAB.data());
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EAB[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		//                     Loc Size  Type    Normalize      Stride        Pos offset
		// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		
                return ProgramID;
	}

}