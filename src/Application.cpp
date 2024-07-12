#include <GL/glew.h>
#include <SDL_events.h>
#include <SDL_keyboard.h>
#include <SDL_keycode.h>
#include <SDL_mouse.h>
#include <SDL_opengl.h>
#include <SDL_scancode.h>
#include <SDL_shape.h>
#include <SDL_video.h>
#include <cmath>
#include <cstddef>
#include <ctime>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
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
	// TEMP global variables
	float deltaTime = 0.0f;
        float currentTime = 0.0f;
        float lastFrame = 0.0f;
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
		SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_BORDERLESS);
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
		GenCube(0.5f, 1.0f, 1.0f, 1.0f, vertices);
                #ifdef defined(WL_DIST) && defined(WL_PLATFORM_WINDOWS)
                GenTexture(&Image, "assets\\textures\\16xsi.png", 0);
		m_ShaderProgram = CreateShaderProgram("shaders\\shader.vert", "shaders\\shader.frag");
		#else
		// GenTexture(&Image, "assets/textures/16xsi.png", 0);
		GenTexture(&Image, "assets/textures/awesomeface.png", 0);
		GenTexture(&Image, "assets/textures/500_yen_bicolor_clad_coin_obverse.jpg", 1);
		m_ShaderProgram = CreateShaderProgram("shaders/shader.vert", "shaders/shader.frag");
		#endif
		
                glUseProgram(m_ShaderProgram);
	        glUniform1i(glGetUniformLocation(m_ShaderProgram, "u_tex"), 0);
                // glUniform1i(glGetUniformLocation(m_ShaderProgram, "u_tex2"), 1);
                glBindVertexArray(vao[0]);
		
                glm::mat4 trans, model, view, projection;
		trans = Identity;
		// trans = glm::rotate(Identity, glm::radians(-90.0f), glm::vec3(0.0, 0.0, -0.5));
		// model = glm::rotate(Identity, glm::radians(-90.0f), glm::vec3(0.0, 0.0, -0.5));
		
                glm::vec3 direction;
		// glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
		// glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		// glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
		// glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
		
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
                glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
                glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		
		projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);

                // Clones
                std::array<glm::vec3, 10> cubeClones = {
			glm::vec3( 0.0f,  0.0f,  0.0f),
			glm::vec3( 2.0f,  5.0f, -15.0f),
			glm::vec3(-1.5f, -2.2f, -2.5f),
			glm::vec3(-3.8f, -2.0f, -12.3f), 
			glm::vec3( 2.4f, -0.4f, -3.5f),
			glm::vec3(-1.7f,  3.0f, -7.5f), 
			glm::vec3( 1.3f, -2.0f, -2.5f), 
			glm::vec3( 1.5f,  2.0f, -2.5f),
			glm::vec3( 1.5f,  0.2f, -1.5f),
			glm::vec3(-1.3f,  1.0f, -1.5f)
		};

                float camX, camY, camZ, angle;
                float yaw = -90.0f;
		float pitch = 0.0f;
                while (m_Running) {
			// glEnable(GL_DEBUG_OUTPUT);
			lastFrame = currentTime;
			currentTime = GetTime();
			deltaTime = currentTime - lastFrame;
                        PollEvent(cameraPos, cameraFront, cameraUp, yaw, pitch);
			direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
			direction.y = sin(glm::radians(pitch));
			direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
			
			cameraFront = glm::normalize(direction);
			// cameraFront = glm::vec3(direction.x, direction.y, direction.z - 5.5f);
			view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
			// view = glm::lookAt(cameraPos, cameraPos + cameraTarget, up);

			glEnable(GL_DEPTH_TEST);
                        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, Image.texture[0]);
			
			float time = GetTime();
			float u_time = glGetUniformLocation(m_ShaderProgram, "u_time");
			float u_time2 = glGetUniformLocation(m_ShaderProgram, "u_time2");
                        glUniform1f(u_time, time);
                        glUniform1f(u_time2, time);

			model = glm::rotate(trans, glm::radians(0.0f), glm::vec3(0.0, 0.0, -0.5));
                        for (int i = 0; i < 10; i++) {
				model = glm::rotate(trans, 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
                                model = glm::translate(model, cubeClones[i]);
				angle = 20.0f * i;
				model = glm::rotate(model, angle, glm::vec3(1.0f, 1.0f, 0.0f));
				glUniformMatrix4fv(glGetUniformLocation(m_ShaderProgram, "u_model"), 1, GL_FALSE, glm::value_ptr(model));
				glUniformMatrix4fv(glGetUniformLocation(m_ShaderProgram, "u_view"), 1, GL_FALSE, glm::value_ptr(view));
				glUniformMatrix4fv(glGetUniformLocation(m_ShaderProgram, "u_projection"), 1, GL_FALSE, glm::value_ptr(projection));
				glDrawElements(GL_TRIANGLES, idx.size(), GL_UNSIGNED_INT, 0);
			};

                        // Gui->Run();
			
			SDL_GL_SwapWindow(m_WindowHandle);
		};
        }

        void Application::PollEvent(glm::vec3 &cameraPos, glm::vec3 &cameraFront, glm::vec3 &cameraUp, float &yaw, float &pitch) {
		SDL_Event event;
                int x = 0, y = 0, dx = 0, dy = 0;
                float cameraSpeed = 3.5f * deltaTime;
		const Uint8* keyStates = SDL_GetKeyboardState(NULL);
		while (SDL_PollEvent(&event)) {
			// ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
				m_Running = false;
                        if (event.type == SDL_MOUSEBUTTONDOWN)
				if (event.button.button == SDL_BUTTON_LEFT)
					SDL_SetRelativeMouseMode(SDL_GetRelativeMouseMode() == SDL_FALSE ? SDL_TRUE : SDL_FALSE);
			if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE)
				m_Running = false;
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(m_WindowHandle))
                          m_Running = false;
                        if (event.type == SDL_WINDOWEVENT_RESIZED) {
				SDL_GetWindowSize(m_WindowHandle, &(m_Specification.Width), &(m_Specification.Height));
				glViewport(0, 0, m_Specification.Width, m_Specification.Height);
                        };
                };

                SDL_PumpEvents();

		Uint32 mouseDelta = SDL_GetRelativeMouseState(&dx, &dy);
		if (SDL_GetRelativeMouseMode() == SDL_TRUE) {
			yaw += 5.0f * dx * deltaTime;
			pitch -= 5.0f * dy * deltaTime;
		}
			
		if (keyStates[SDL_SCANCODE_W] >= 1)
			cameraPos += cameraFront * cameraSpeed;
		else if (keyStates[SDL_SCANCODE_S] >= 1)
			cameraPos -= cameraFront * cameraSpeed;
		if (keyStates[SDL_SCANCODE_D] >= 1)
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		else if (keyStates[SDL_SCANCODE_A] >= 1)
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

		if(pitch > 89.0f)
			pitch =  89.0f;
		if(pitch < -89.0f)
			pitch = -89.0f;
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

		vao.resize(1); vbo.resize(1);
		glGenVertexArrays(vao.size(), vao.data());
                glGenBuffers(vbo.size(), vbo.data());
		
                glBindVertexArray(vao[0]);
                glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(0));
                glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vertex().pos)));
		glEnableVertexAttribArray(1);

                // Element array buffer - This is bound automatically to the current vao, meaning a vao must be currently bound first
		eab.resize(1);
                glGenBuffers(eab.size(), eab.data());
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eab[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(std::vector<GLuint>) * idx.size(), idx.data(), GL_STATIC_DRAW);

		//                     Loc Size  Type    Normalize      Stride        Pos offset
		// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

                return ProgramID;
	}

	void Application::GenCircle(float radius, int vertCount, std::vector<Vertex> &vertices) {
		const int TriCount = vertCount - 2;
		const float Angle = 360.0f / vertCount;
		// const float PI = glm::pi<float>();
                float theta, x, y;
		
		vertices.clear();
		idx.clear();
		vertices.reserve(vertCount);
		idx.reserve(vertCount);
		
                for (int i = 0; i < vertCount; i++) {
			theta = i * Angle;
			x = radius * cos(glm::radians(theta));
			y = radius * sin(glm::radians(theta));
                        vertices.push_back(Vertex());
			vertices[i].pos = (glm::vec3(x, y, 0.0f));
			vertices[i].uv = (glm::vec2(0.0f + 1.0f * (x / radius + 1) * 0.5f, 0.0f + 1.0f * (y / radius + 1)*0.5f));
			// uv->push_back(glm::vec2(1.2 * ((cos(glm::radians(theta))+1) * 0.5), (1.2 * (cos((glm::radians(theta))-pi*1.5)+1) * 0.5)));
			// std::cout << "(" << uv[0][i].y << ", " << uv[0][i].y << ")" << std::endl;
                        if (i < TriCount) {
				idx.push_back(0);
				idx.push_back(i+1);
				idx.push_back(i+2);
			};
                }
        }
	
	void Application::GenTri(float scale, float top, float right, float left, std::vector<Vertex> &vertices) {
		// origin is aligned with the top y axis
		// so right is right from top, and left is left from top
		const float n_top = top * scale * 0.25;
		const float n_right = right * scale * 0.5;
		const float n_left = -left * scale * 0.5;
		vertices.clear();
                idx.clear();
		vertices.reserve(3);
                idx.reserve(3);
		
                vertices.push_back(Vertex());
                vertices.push_back(Vertex());
                vertices.push_back(Vertex());
		
                vertices[0].pos = (glm::vec3(0.0f, n_top, 0.0f));  // top
		vertices[1].pos = (glm::vec3(n_right, -n_top, 0.0f));  // right
		vertices[2].pos = (glm::vec3(n_left, -n_top, 0.0f));  // left
		vertices[0].uv = (glm::vec2(0.5f, 1.0f));  // top
		vertices[1].uv = (glm::vec2(1.0f, 0.0f));  // right
		vertices[2].uv = (glm::vec2(0.0f, 0.0f));  // left
	  
		idx.push_back(0);
		idx.push_back(1);
		idx.push_back(2);
        }
	
	void Application::GenQuad(float scale, float width, float height, std::vector<Vertex> &vertices, float z) {
		const float n_width = width * scale * 0.5;
		const float n_height = height * scale * 0.5;
		vertices.clear();
		idx.clear();
		vertices.reserve(4);
		idx.reserve(6);

                for (int i = 0; i < 4; i++)
			vertices.push_back(Vertex());
		
                vertices[0].pos = glm::vec3(n_width, n_height, z);  // top right
		vertices[1].pos = glm::vec3(n_width, -n_height, z);  // bot right
		vertices[2].pos = glm::vec3(-n_width, -n_height, z);  // bot left
		vertices[3].pos = glm::vec3(-n_width, n_height, z);   // top left
		
		vertices[0].uv = glm::vec2(1.0f, 1.0f);  // top right
		vertices[1].uv = glm::vec2(1.0f, 0.0f);  // bot right
	        vertices[2].uv = glm::vec2(0.0f, 0.0f);  // bot left
                vertices[3].uv = glm::vec2(0.0f, 1.0f);  // top left
	  
		idx.push_back(0);
		idx.push_back(1);
		idx.push_back(2);
		idx.push_back(2);
		idx.push_back(3);
		idx.push_back(0);

        }
	
	void Application::GenCube(float scale, float width, float height, float length, std::vector<Vertex> &vertices) {
		// TODO: Fix vertex indices
		const float n_width = width * scale * 0.5;
                const float n_height = height * scale * 0.5;
                const float n_length = length * scale * 0.5;
		// const int uvOffset = 36;
		
		vertices.clear();
		idx.clear();
		vertices.reserve(8);
		idx.reserve(36);

                for (int i = 0; i < 16; i++)
			vertices.push_back(Vertex());
		
		// Orthogonal to xz plane
		// Back
		vertices[0].pos = (glm::vec3(n_width, n_height, -n_length));  // top right
		vertices[1].pos = (glm::vec3(n_width, -n_height, -n_length));  // bot right
		vertices[2].pos = (glm::vec3(-n_width, -n_height, -n_length));  // bot left
		vertices[3].pos = (glm::vec3(-n_width, n_height, -n_length));   // top left
		
		// Front
		vertices[4].pos = (glm::vec3(n_width, n_height, n_length));  // top right
		vertices[5].pos = (glm::vec3(n_width, -n_height, n_length));  // bot right
		vertices[6].pos = (glm::vec3(-n_width, -n_height, n_length));  // bot left
		vertices[7].pos = (glm::vec3(-n_width, n_height, n_length));   // top left

		// Orthogonal to xy plane
		// Top
		vertices[8].pos = (glm::vec3(n_width, n_height, -n_length));  // top right
		vertices[9].pos = (glm::vec3(n_width, n_height, n_length));  // bot right
		vertices[10].pos = (glm::vec3(-n_width, n_height, n_length));   // bot left
		vertices[11].pos = (glm::vec3(-n_width, n_height, -n_length));   // top left

                // Bottom
		vertices[12].pos = (glm::vec3(n_width, -n_height, -n_length));  // top right
		vertices[13].pos = (glm::vec3(n_width, -n_height, n_length));  // bot right
		vertices[14].pos = (glm::vec3(-n_width, -n_height, n_length));  // bot left
		vertices[15].pos = (glm::vec3(-n_width, -n_height, -n_length));  // top left
		
		// Indicies
                // back
		idx.push_back(0);
		idx.push_back(1);
		idx.push_back(2);
		idx.push_back(2);
		idx.push_back(3);
                idx.push_back(0);
		// front
		idx.push_back(7);
		idx.push_back(6);
		idx.push_back(5);
		idx.push_back(5);
		idx.push_back(4);
                idx.push_back(7);
		// left
		idx.push_back(7);
		idx.push_back(6);
		idx.push_back(2);
		idx.push_back(2);
		idx.push_back(3);
                idx.push_back(7);
		// right
		idx.push_back(0);
		idx.push_back(1);
		idx.push_back(5);
		idx.push_back(5);
		idx.push_back(4);
                idx.push_back(0);
		// top
		idx.push_back(8);
		idx.push_back(9);
		idx.push_back(10);
		idx.push_back(10);
		idx.push_back(11);
                idx.push_back(8);
		// bot
		idx.push_back(12);
		idx.push_back(13);
		idx.push_back(14);
		idx.push_back(14);
		idx.push_back(15);
                idx.push_back(12);
		
		vertices[0].uv = (glm::vec2(1.0f, 1.0f));  // top right
		vertices[1].uv = (glm::vec2(1.0f, 0.0f));  // bot right
		vertices[2].uv = (glm::vec2(0.0f, 0.0f));  // bot left
                vertices[3].uv = (glm::vec2(0.0f, 1.0f));  // top left
				
		vertices[4].uv = (glm::vec2(0.0f, 1.0f));  // top left
		vertices[5].uv = (glm::vec2(0.0f, 0.0f));  // bot left
		vertices[6].uv = (glm::vec2(1.0f, 0.0f));  // bot right
		vertices[7].uv = (glm::vec2(1.0f, 1.0f));  // top right
		
		vertices[8].uv = (glm::vec2(1.0f, 1.0f));  // top right
		vertices[9].uv = (glm::vec2(1.0f, 0.0f));  // bot right
		vertices[10].uv = (glm::vec2(0.0f, 0.0f));  // bot left
                vertices[11].uv = (glm::vec2(0.0f, 1.0f));  // top left

		vertices[12].uv = (glm::vec2(1.0f, 1.0f));  // top right
		vertices[13].uv = (glm::vec2(1.0f, 0.0f));  // bot right
		vertices[14].uv = (glm::vec2(0.0f, 0.0f));  // bot left
                vertices[15].uv = (glm::vec2(0.0f, 1.0f));  // top left
	}
}
