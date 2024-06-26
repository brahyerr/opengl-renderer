#include <GL/glew.h>
#include <SDL_events.h>
#include <SDL_keycode.h>
#include <SDL_opengl.h>
#include <SDL_shape.h>
#include <SDL_video.h>
#include <cmath>
#include <cstddef>
#include <ctime>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/fwd.hpp>
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

#define PI 3.14159265358979323846f
#define EQ_TRI_RATIO 0.86602540

#include "Application.h"
#include "imgui_impl_sdl2.h"

static RT::Application *s_Instance = nullptr;
static RT::GUI *Gui = nullptr;

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
		SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_BORDERLESS | SDL_WINDOW_RESIZABLE);
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
			printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
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
                glViewport((m_Specification.Width - m_Specification.Height) * 0.5, 0, m_Specification.Height, m_Specification.Height);  // TEMP
                glClearColor(0.04f, 0.02f, 0.08f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                // Gui = new GUI(m_WindowHandle);
        }
	
        Application::~Application() {  // TODO: Improve destruction flow
		Shutdown();
                s_Instance = nullptr;
        }
	
	void Application::Shutdown() {
		// delete Gui;
                glDeleteVertexArrays(vao.size(), vao.data());
                glDeleteBuffers(vbo.size(), vbo.data());
		glDeleteProgram(m_ShaderProgram);
                SDL_GL_DeleteContext(m_glContext);
                SDL_DestroyWindow(m_WindowHandle);
                SDL_Quit();

		g_ApplicationRunning = false;
        }

        void Application::Run() {
		// TODO: Split gl function calls into proper classes
                m_Running = true;
		// GenQuad(1.5f, 1.0f, 1.0f, &RenderData);
		GenCube(0.5f, 1.0f, 1.0f, 1.0f, &RenderData);
                #ifdef defined(WL_DIST) && defined(WL_PLATFORM_WINDOWS)
                GenTexture(&Image, "assets\\textures\\16xsi.png", 0);
		m_ShaderProgram = CreateShaderProgram("shaders\\shader.vert", "shaders\\shader.frag");
		#else
		// GenTexture(&Image, "assets/textures/16xsi.png", 0);
		GenTexture(&Image, "assets/textures/awesomeface.png", 0);
		GenTexture(&Image, "assets/textures/500_yen_bicolor_clad_coin_obverse.jpg", 1);
		m_ShaderProgram = CreateShaderProgram("shaders/shader.vert", "shaders/shader.frag");
		#endif
		
		// GenCircle(0.25f, 36, &RenderData);
		// GenTri(1.0f, EQ_TRI_RATIO * 0.8f, 0.4f, 0.4f, &RenderData);
		// glUseProgram(m_ShaderProgram);
                glUseProgram(m_ShaderProgram);
	        glUniform1i(glGetUniformLocation(m_ShaderProgram, "u_tex"), 0);
                // glUniform1i(glGetUniformLocation(m_ShaderProgram, "u_tex2"), 1);
                glBindVertexArray(vao[0]);
		
                glm::mat4 trans, model, view, projection;
		trans = glm::rotate(Identity, glm::radians(-90.0f), glm::vec3(0.0, 0.0, -0.5));
		// model = glm::rotate(Identity, glm::radians(-90.0f), glm::vec3(0.0, 0.0, -0.5));
		view = glm::translate(Identity, glm::vec3(0.0f, 0.0f, -2.75f));
		projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);

                while (m_Running) {
			PollEvent(&trans);
			model = glm::rotate(trans, glm::radians(-90.0f), glm::vec3(0.0, 0.0, -0.5));
			// glEnable(GL_DEBUG_OUTPUT);

			glEnable(GL_DEPTH_TEST);
                        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, Image.texture[0]);
			// glActiveTexture(GL_TEXTURE1);
                        // glBindTexture(GL_TEXTURE_2D, Image.texture[1]);
			
			float time = GetTime();
			float u_time = glGetUniformLocation(m_ShaderProgram, "u_time");
			float u_time2 = glGetUniformLocation(m_ShaderProgram, "u_time2");
                        glUniform1f(u_time, time);
                        glUniform1f(u_time2, time);

			// trans = glm::translate(Identity, glm::vec3(0.0f, -0.6f, 0.0f));
			// trans = glm::rotate(trans, glm::radians(-(time * 50 + (float) (cos(time) * 20 + 30))), glm::vec3(0.5, 0.5, 0.5));
			// trans = glm::scale(trans, glm::vec3(1,1,1));
			glUniformMatrix4fv(glGetUniformLocation(m_ShaderProgram, "u_model"), 1, GL_FALSE, glm::value_ptr(model));
			glUniformMatrix4fv(glGetUniformLocation(m_ShaderProgram, "u_view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(m_ShaderProgram, "u_projection"), 1, GL_FALSE, glm::value_ptr(projection));
			
			glDrawElements(GL_TRIANGLES, RenderData.idx.size(), GL_UNSIGNED_INT, 0);
			// glDrawElements(GL_TRIANGLES, RenderData.idx.size() - 36, GL_UNSIGNED_INT, 36);
			// glUniform1f(u_time2, time + 9);
			// trans = glm::translate(Identity, glm::vec3(0.5, 0.0, 0.0));
			// trans = glm::rotate(trans, glm::radians(time * 50 + (float) (sin(time) * 20 + 30)), glm::vec3(0.5, 0.5, 0.5));
			// trans = glm::scale(trans, glm::vec3(2,2,2));
			// glUniformMatrix4fv(glGetUniformLocation(m_ShaderProgram, "u_trans"), 1, GL_FALSE, glm::value_ptr(trans));

			// glDrawElements(GL_TRIANGLES, RenderData.idx.size(), GL_UNSIGNED_INT, 0);
                        // Gui->Run();
			
			SDL_GL_SwapWindow(m_WindowHandle);
		};
        }

        void Application::PollEvent(glm::mat4 *matrix) {
		SDL_Event event;
		float x = 0, y = 0, dx = 0, dy = 0;
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
                        if (event.type == SDL_KEYDOWN) {
				// x = dx, y = dy;
				// dx += event.motion.y / 720.0f;
                                // dy += event.motion.x / 720.0f;
				// std::cout << "dx: " << dx << "dy: " << dy << std::endl;
                                if (event.key.keysym.sym == SDLK_UP)
					*matrix = glm::rotate(*matrix, glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				else if (event.key.keysym.sym == SDLK_DOWN)
					*matrix = glm::rotate(*matrix, glm::radians(-10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				if (event.key.keysym.sym == SDLK_RIGHT)
					*matrix = glm::rotate(*matrix, glm::radians(10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				else if (event.key.keysym.sym == SDLK_LEFT)
					*matrix = glm::rotate(*matrix, glm::radians(-10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                        };
		};
        }

        float Application::GetTime() {
		typedef std::chrono::high_resolution_clock clock;
		typedef std::chrono::duration<float, std::ratio<1>> duration; // use std::milli or std::ratio<1,1000> for ms instead

		duration elapsed = clock::now() - time_start;
	  
		return (float) elapsed.count(); // seconds
        }
	void Application::GenTexture(struct ImageData* Image, std::string path, int index) {
		stbi_set_flip_vertically_on_load(true);  // must flip since images usually have y = 0.0 on the top, while openGL has it on the bottom
		Image->texture.resize(Image->texture.size() + 1);
		Image->data = stbi_load(&path[0], &(Image->width), &(Image->height), &(Image->nrChannels), 0);
		
                glGenTextures(1, &Image->texture[index]);
		glBindTexture(GL_TEXTURE_2D, Image->texture[index]);
		
		// set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		
		if (Image->data) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Image->width, Image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Image->data);
                        glGenerateMipmap(GL_TEXTURE_2D);
                } else {
			std::cout << "Failed to load texture!" << std::endl;
                }
		stbi_image_free(Image->data);
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

		vao.resize(1); vbo.resize(2);
		glGenVertexArrays(vao.size(), vao.data());
                glGenBuffers(vbo.size(), vbo.data());
		
                glBindVertexArray(vao[0]);
                glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * RenderData.vert.size(), RenderData.vert.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0 * (sizeof(float))));
                glEnableVertexAttribArray(0);

		// Color attribute
		// glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * (sizeof(float))));
                // glEnableVertexAttribArray(1);

		// UV attribute
		// glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * (sizeof(float))));
		// glEnableVertexAttribArray(2);
                glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(std::vector<glm::vec3>) * RenderData.uv.size(), RenderData.uv.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
		glEnableVertexAttribArray(1);

                // Element array buffer - This is bound automatically to the current vao, meaning a vao must be currently bound first
		eab.resize(1);
                glGenBuffers(eab.size(), eab.data());
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eab[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(std::vector<GLuint>) * RenderData.idx.size(), RenderData.idx.data(), GL_STATIC_DRAW);

		//                     Loc Size  Type    Normalize      Stride        Pos offset
		// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

                return ProgramID;
	}

	void Application::GenCircle(float radius, int vertCount, struct RenderData* RenderData) {
		const int tri_count = vertCount - 2;
		const float angle = 360.0f / vertCount;
		const float pi = glm::pi<float>();
                float theta, x, y;
		
		RenderData->vert.clear();
		RenderData->uv.clear();
		RenderData->idx.clear();
		RenderData->vert.reserve(RenderData->vert.size() + vertCount);
		RenderData->uv.reserve(RenderData->vert.size() + vertCount);
                RenderData->idx.reserve(RenderData->vert.size() + tri_count);
		
                for (int i = 0; i < vertCount; i++) {
			theta = i * angle;
			x = radius * cos(glm::radians(theta));
			y = radius * sin(glm::radians(theta));
			RenderData->vert.push_back(glm::vec3(x, y, 0.0f));
			RenderData->uv.push_back(glm::vec2(0.0f + 1.0f * (x / radius + 1) * 0.5f, 0.0f + 1.0f * (y / radius + 1)*0.5f));
			// uv->push_back(glm::vec2(1.2 * ((cos(glm::radians(theta))+1) * 0.5), (1.2 * (cos((glm::radians(theta))-pi*1.5)+1) * 0.5)));
			// std::cout << "(" << uv[0][i].y << ", " << uv[0][i].y << ")" << std::endl;
                        if (i < tri_count) {
				RenderData->idx.push_back(0);
				RenderData->idx.push_back(i+1);
				RenderData->idx.push_back(i+2);
			};
                }
        }
	
	void Application::GenTri(float scale, float top, float right, float left, struct RenderData* RenderData) {
		// origin is aligned with the top y axis
		// so right is right from top, and left is left from top
		const float n_top = top * scale * 0.25;
		const float n_right = right * scale * 0.5;
		const float n_left = -left * scale * 0.5;
		RenderData->vert.clear();
		RenderData->uv.clear();
		RenderData->idx.clear();
		RenderData->vert.reserve(RenderData->vert.size() + 3);
		RenderData->uv.reserve(RenderData->vert.size() + 3);
		RenderData->idx.reserve(RenderData->vert.size() + 3);

		RenderData->vert.push_back(glm::vec3(0.0f, n_top, 0.0f));  // top
		RenderData->vert.push_back(glm::vec3(n_right, -n_top, 0.0f));  // right
		RenderData->vert.push_back(glm::vec3(n_left, -n_top, 0.0f));  // left
		
		// RenderData->vert.push_back(glm::vec3(0.0f, 0.5f, 0.0f));  // top
		// RenderData->vert.push_back(glm::vec3( 0.5f, -0.5f, 0.0f));  // right
		// RenderData->vert.push_back(glm::vec3(-0.5f, -0.5f, 0.0f));  // left
		
		RenderData->uv.push_back(glm::vec2(0.5f, 1.0f));  // top
		RenderData->uv.push_back(glm::vec2(1.0f, 0.0f));  // right
		RenderData->uv.push_back(glm::vec2(0.0f, 0.0f));  // left
	  
		RenderData->idx.push_back(0);
		RenderData->idx.push_back(1);
		RenderData->idx.push_back(2);

        }
	
	void Application::GenQuad(float scale, float width, float height, struct RenderData* RenderData, float z) {
		const float n_width = width * scale * 0.5;
		const float n_height = height * scale * 0.5;
		RenderData->vert.clear();
		RenderData->uv.clear();
		RenderData->idx.clear();
		RenderData->vert.reserve(RenderData->vert.size() + 4);
		RenderData->uv.reserve(RenderData->vert.size() + 4);
		RenderData->idx.reserve(RenderData->vert.size() + 6);

		RenderData->vert.push_back(glm::vec3(n_width, n_height, z));  // top right
		RenderData->vert.push_back(glm::vec3(n_width, -n_height, z));  // bot right
		RenderData->vert.push_back(glm::vec3(-n_width, -n_height, z));  // bot left
		RenderData->vert.push_back(glm::vec3(-n_width, n_height, z));   // top left
	  
		RenderData->idx.push_back(0);
		RenderData->idx.push_back(1);
		RenderData->idx.push_back(2);
		RenderData->idx.push_back(2);
		RenderData->idx.push_back(3);
		RenderData->idx.push_back(0);

		RenderData->uv.push_back(glm::vec2(1.0f, 1.0f));  // top right
		RenderData->uv.push_back(glm::vec2(1.0f, 0.0f));  // bot right
		RenderData->uv.push_back(glm::vec2(0.0f, 0.0f));  // bot left
                RenderData->uv.push_back(glm::vec2(0.0f, 1.0f));  // top left
        }
	
	void Application::GenCube(float scale, float width, float height, float length, struct RenderData* RenderData) {
		// TODO: Fix vertex indices
		const float n_width = width * scale * 0.5;
                const float n_height = height * scale * 0.5;
                const float n_length = length * scale * 0.5;
		// const int uvOffset = 36;
		
		RenderData->vert.clear();
		RenderData->uv.clear();
		RenderData->idx.clear();
		RenderData->vert.reserve(RenderData->vert.size() + 8);
		RenderData->uv.reserve(RenderData->vert.size() + 4);
		RenderData->idx.reserve(RenderData->vert.size() + 36);

		// Orthogonal to xz plane
		// Back
		RenderData->vert.push_back(glm::vec3(n_width, n_height, -n_length));  // top right
		RenderData->vert.push_back(glm::vec3(n_width, -n_height, -n_length));  // bot right
		RenderData->vert.push_back(glm::vec3(-n_width, -n_height, -n_length));  // bot left
		RenderData->vert.push_back(glm::vec3(-n_width, n_height, -n_length));   // top left
		
		// Front
		RenderData->vert.push_back(glm::vec3(n_width, n_height, n_length));  // top right
		RenderData->vert.push_back(glm::vec3(n_width, -n_height, n_length));  // bot right
		RenderData->vert.push_back(glm::vec3(-n_width, -n_height, n_length));  // bot left
		RenderData->vert.push_back(glm::vec3(-n_width, n_height, n_length));   // top left

		// Orthogonal to xy plane
		// Top
		RenderData->vert.push_back(glm::vec3(n_width, n_height, -n_length));  // top right
		RenderData->vert.push_back(glm::vec3(n_width, n_height, n_length));  // bot right
		RenderData->vert.push_back(glm::vec3(-n_width, n_height, n_length));   // bot left
		RenderData->vert.push_back(glm::vec3(-n_width, n_height, -n_length));   // top left

                // Bottom
		RenderData->vert.push_back(glm::vec3(n_width, -n_height, -n_length));  // top right
		RenderData->vert.push_back(glm::vec3(n_width, -n_height, n_length));  // bot right
		RenderData->vert.push_back(glm::vec3(-n_width, -n_height, n_length));  // bot left
		RenderData->vert.push_back(glm::vec3(-n_width, -n_height, -n_length));  // top left
		
		// Indicies
                // back
		RenderData->idx.push_back(0);
		RenderData->idx.push_back(1);
		RenderData->idx.push_back(2);
		RenderData->idx.push_back(2);
		RenderData->idx.push_back(3);
                RenderData->idx.push_back(0);
		// front
		RenderData->idx.push_back(7);
		RenderData->idx.push_back(6);
		RenderData->idx.push_back(5);
		RenderData->idx.push_back(5);
		RenderData->idx.push_back(4);
                RenderData->idx.push_back(7);
		// left
		RenderData->idx.push_back(7);
		RenderData->idx.push_back(6);
		RenderData->idx.push_back(2);
		RenderData->idx.push_back(2);
		RenderData->idx.push_back(3);
                RenderData->idx.push_back(7);
		// right
		RenderData->idx.push_back(0);
		RenderData->idx.push_back(1);
		RenderData->idx.push_back(5);
		RenderData->idx.push_back(5);
		RenderData->idx.push_back(4);
                RenderData->idx.push_back(0);
		// top
		RenderData->idx.push_back(8);
		RenderData->idx.push_back(9);
		RenderData->idx.push_back(10);
		RenderData->idx.push_back(10);
		RenderData->idx.push_back(11);
                RenderData->idx.push_back(8);
		// bot
		RenderData->idx.push_back(12);
		RenderData->idx.push_back(13);
		RenderData->idx.push_back(14);
		RenderData->idx.push_back(14);
		RenderData->idx.push_back(15);
                RenderData->idx.push_back(12);
		
		RenderData->uv.push_back(glm::vec2(1.0f, 1.0f));  // top right
		RenderData->uv.push_back(glm::vec2(1.0f, 0.0f));  // bot right
		RenderData->uv.push_back(glm::vec2(0.0f, 0.0f));  // bot left
                RenderData->uv.push_back(glm::vec2(0.0f, 1.0f));  // top left
				
		RenderData->uv.push_back(glm::vec2(0.0f, 1.0f));  // top left
		RenderData->uv.push_back(glm::vec2(0.0f, 0.0f));  // bot left
		RenderData->uv.push_back(glm::vec2(1.0f, 0.0f));  // bot right
		RenderData->uv.push_back(glm::vec2(1.0f, 1.0f));  // top right
		
		RenderData->uv.push_back(glm::vec2(1.0f, 1.0f));  // top right
		RenderData->uv.push_back(glm::vec2(1.0f, 0.0f));  // bot right
		RenderData->uv.push_back(glm::vec2(0.0f, 0.0f));  // bot left
                RenderData->uv.push_back(glm::vec2(0.0f, 1.0f));  // top left

		RenderData->uv.push_back(glm::vec2(1.0f, 1.0f));  // top right
		RenderData->uv.push_back(glm::vec2(1.0f, 0.0f));  // bot right
		RenderData->uv.push_back(glm::vec2(0.0f, 0.0f));  // bot left
                RenderData->uv.push_back(glm::vec2(0.0f, 1.0f));  // top left
	}
}
