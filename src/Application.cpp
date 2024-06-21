#include <GL/glew.h>
#include <SDL_keycode.h>
#include <SDL_opengl.h>
#include <SDL_shape.h>
#include <SDL_video.h>
#include <cmath>
#include <cstddef>
#include <ctime>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/fwd.hpp>
// #include <glm/glm.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "Application.h"
#include "imgui_impl_sdl2.h"

static RT::Application *s_Instance = nullptr;
static RT::GUI *Gui = nullptr;

// const std::array<float, 12> vertices = {
//     // Viewport - vertices
//     0.8f,  0.8f,  0.0f,
//     0.8f,  -0.8f, 0.0f,
//     -0.8f, -0.8f, 0.0f,
//     -0.8f, 0.8f,  0.0f
// };

// const std::vector<GLuint> indices = {
// 	0, 1, 2,
// 	2, 3, 0
// };

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
		// Create window with graphics context
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
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

                // Generate opengl shader program
		GenCircle(0.4f, 48, &vertices, &uv);

                glViewport((m_Specification.Width - m_Specification.Height) * 0.5, 0, m_Specification.Height, m_Specification.Height);  // TEMP
                glClearColor(0.04f, 0.02f, 0.08f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
		#ifdef defined(WL_DIST) && defined(WL_PLATFORM_WINDOWS)
		GenTexture(&image, "assets\\textures\\16xsi.png");
		m_ShaderProgram = CreateShaderProgram("shaders\\sphere_test.vert", "shaders\\sphere_test.frag");
		#else
		GenTexture(&image, "assets/textures/500_yen_bicolor_clad_coin_obverse.jpg", 0);
		GenTexture(&image, "assets/textures/16xsi.png", 1);
		m_ShaderProgram = CreateShaderProgram("shaders/shader.vert", "shaders/shader.frag");
		#endif
		glUseProgram(m_ShaderProgram);
		
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
		// TODO: Split gl function calls into proper classes
		m_Running = true;
		
                glm::mat4 Trans;
                glUseProgram(m_ShaderProgram);
	        glUniform1i(glGetUniformLocation(m_ShaderProgram, "u_tex"), 0);
                glUniform1i(glGetUniformLocation(m_ShaderProgram, "u_tex2"), 1);

                while (m_Running) {
			PollEvent();

                        glClear(GL_COLOR_BUFFER_BIT);
			float time = GetTime();
			float gl_time = glGetUniformLocation(m_ShaderProgram, "u_time");
                        glUniform1f(gl_time, time);
			
			Trans = glm::rotate(Identity, glm::radians(time * 50 + (float) (sin(time) * 20 + 30)), glm::vec3(0.5, 0.5, 0.5));
			Trans = glm::scale(Trans, glm::vec3(2,2,2));
			glUniformMatrix4fv(glGetUniformLocation(m_ShaderProgram, "u_trans"), 1, GL_FALSE, glm::value_ptr(Trans));

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, image.texture[0]);
			glActiveTexture(GL_TEXTURE1);
                        glBindTexture(GL_TEXTURE_2D, image.texture[1]);

			glUseProgram(m_ShaderProgram);
			glBindVertexArray(VAO[0]);

			// glEnable(GL_DEBUG_OUTPUT);
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

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
                        if (event.type == SDL_WINDOWEVENT_RESIZED) {
				SDL_GetWindowSize(m_WindowHandle, &(m_Specification.Width), &(m_Specification.Height));
				glViewport(0, 0, m_Specification.Width, m_Specification.Height);
			};

		};
        }

        float Application::GetTime() {
		typedef std::chrono::high_resolution_clock clock;
		typedef std::chrono::duration<float, std::ratio<1>> duration; // use std::milli or std::ratio<1,1000> for ms instead

		duration elapsed = clock::now() - time_start;
	  
		return (float) elapsed.count(); // seconds
        }

        void Application::GenTexture(Image* image, std::string path, int index) {
		stbi_set_flip_vertically_on_load(true);  // must flip since images usually have y = 0.0 on the top, while openGL has it on the bottom
		image->texture.resize(image->texture.size() + 1);
		image->data = stbi_load(&path[0], &(image->width), &(image->height), &(image->nrChannels), 0);
		
		// consider using stbi_load(FileSystem::getPath("resources/textures/awesomeface.png").c_str(), &width, &height, &nrChannels, 0);
                glGenTextures(1, &image->texture[index]);
		glBindTexture(GL_TEXTURE_2D, image->texture[index]);
		
		// set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		if (image->data) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data);
                        glGenerateMipmap(GL_TEXTURE_2D);
                } else {
			std::cout << "Failed to load texture!" << std::endl;
                }
		stbi_image_free(image->data);
	};
	
        GLuint Application::CreateShaderProgram(const char *vertex_file_path, const char *fragment_file_path) {
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

		VAO.resize(1); VBO.resize(2);
		glGenVertexArrays(VAO.size(), VAO.data());
                glGenBuffers(VBO.size(), VBO.data());
		
                glBindVertexArray(VAO[0]);
                glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0 * (sizeof(float))));
                glEnableVertexAttribArray(0);

		// Color attribute
		// glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * (sizeof(float))));
                // glEnableVertexAttribArray(1);

		// UV attribute
		// glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * (sizeof(float))));
		// glEnableVertexAttribArray(2);
                glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(std::vector<glm::vec3>) * uv.size(), uv.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
		glEnableVertexAttribArray(1);

                // Element array buffer - This is bound automatically to the current VAO, meaning a VAO must be currently bound first
		EAB.resize(1);
                glGenBuffers(EAB.size(), EAB.data());
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EAB[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(std::vector<GLuint>) * indices.size(), indices.data(), GL_STATIC_DRAW);

		//                     Loc Size  Type    Normalize      Stride        Pos offset
		// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

                return ProgramID;
	}

	void Application::GenCircle(float radius, int vertCount, std::vector<glm::vec3>* vertices, std::vector<glm::vec2>* uv) {
		float angle = 360.0f / vertCount;
		float theta, x, y;
		float pi = glm::pi<float>();
                int tri_count = vertCount - 2;

                for (int i = 0; i < vertCount; i++) {
			theta = i * angle;
			x = radius * cos(glm::radians(theta));
			y = radius * sin(glm::radians(theta));
			vertices->push_back(glm::vec3(x, y, 0.0f));
			uv->push_back(glm::vec2(0.0 + 1.0 * (x / radius + 1) * 0.5, 0.0 + 1.0 * (y / radius + 1)*0.5));
			// uv->push_back(glm::vec2(1.2 * ((cos(glm::radians(theta))+1) * 0.5), (1.2 * (cos((glm::radians(theta))-pi*1.5)+1) * 0.5)));
			// std::cout << "(" << uv[0][i].x << ", " << uv[0][i].y << ")" << std::endl;
                        if (i < tri_count) {
				indices.push_back(0);
				indices.push_back(i+1);
				indices.push_back(i+2);
			};
                }
	}
}
