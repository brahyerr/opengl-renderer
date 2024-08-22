#include <GL/glew.h>
#include <SDL_blendmode.h>
#include <SDL_events.h>
#include <SDL_keyboard.h>
#include <SDL_keycode.h>
#include <SDL_mouse.h>
#include <SDL_opengl.h>
#include <SDL_scancode.h>
#include <SDL_shape.h>
#include <SDL_timer.h>
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
// #include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#define PI 3.14159265358979323846f

#include "Clock.h"
// #include "Shape.h"
#include "Mesh.h"

#include "Application.h"
#include "Camera.h"
#include "Shader.h"
#include "GUI.h"
#include "imgui.h"

#include "imgui_impl_sdl2.h"

static RT::Application *s_Instance = nullptr;
// static RT::GUI *s_Gui = nullptr;
static RT::Camera *s_Camera = nullptr;
// static RT::Shader *s_Shader = nullptr;

namespace RT {
	// GUI m_Gui = nullptr;
	Application::Application(const ApplicationSpecification &spec)
		: m_Specification(spec) {
		s_Instance = this;
		Init();
	}
	void Application::Init() {
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

		s_Camera = new Camera();

                // s_Gui = new GUI(m_WindowHandle);
        }
	
        Application::~Application() {  // TODO: Improve destruction flow
		Shutdown();
                s_Instance = nullptr;
        }
	
	void Application::Shutdown() {
                glDeleteVertexArrays(vao.size(), vao.data());
                glDeleteBuffers(vbo.size(), vbo.data());
		// glDeleteProgram(m_ShaderProgram);
                SDL_GL_DeleteContext(m_glContext);
                SDL_DestroyWindow(m_WindowHandle);
                SDL_Quit();

		// delete Gui;
		delete s_Camera;
		g_ApplicationRunning = false;
        }

        void Application::Run() {
		// TODO: Split gl function calls into proper classes
                m_Running = true;
		// Shape::GenCube(0.5f, 1.0f, 1.0f, 1.0f);
		Cube cube = Cube();
		Cube lightSource = Cube();
		
		cube.PopulateVertices(1.0f);
                lightSource.PopulateIndexedVertices(0.25f);

		Shader cubeShader = Shader("shaders/cubeShader.vert", "shaders/cubeShader.frag");
		Shader lightSourceShader = Shader("shaders/lightSource.vert", "shaders/lightSource.frag");

		cubeShader.GenTexture("assets/textures/16xsi.png", 0);
                cubeShader.SetIntUni("u_tex", 0);

		// Gen/bind buffers/objects - consider moving to a Scene class?
                vao.resize(2);
                vbo.resize(2);    // size 1 + 1 for light framebuffer
		GLuint cubeLightVAO;
		glGenVertexArrays(vao.size(), vao.data());
                glGenBuffers(vbo.size(), vbo.data());

		// Pos
                glBindVertexArray(vao[0]);
                glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, cube.Vertices.size() * sizeof(Mesh::Vertex), cube.Vertices.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)(0));
                glEnableVertexAttribArray(0);

		// // Normal
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)(sizeof(Mesh::Vertex().pos)));
                glEnableVertexAttribArray(1);

		// UV
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)(sizeof(Mesh::Vertex().pos) + sizeof(Mesh::Vertex().normal)));
		glEnableVertexAttribArray(2);

		// Bind buffer data for lighting shader here

                // Element array buffer - This is bound automatically to the current vao, meaning a vao must be currently bound first
		// eab.resize(2);
                // glGenBuffers(eab.size(), eab.data());
                // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eab[0]);
		// glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(std::vector<GLuint>) * cube.Idx.size(), cube.Idx.data(), GL_STATIC_DRAW);

		// lightSource.Use();
		// Light VAO + VBO + EAB
		// std::vector<GLuint> lightVAO;
                // lightVAO.resize(1);
		// glGenVertexArrays(1, &cubeLightVAO);
                // glBindVertexArray(cubeLightVAO);
		glBindVertexArray(vao[1]);
                glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, lightSource.Vertices.size() * sizeof(Mesh::Vertex), lightSource.Vertices.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh::Vertex), (void*)(0));
                glEnableVertexAttribArray(0);

                // glGenBuffers(eab.size(), eab.data());
		eab.resize(1);
                glGenBuffers(eab.size(), eab.data());
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eab[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(std::vector<GLuint>) * lightSource.Idx.size(), lightSource.Idx.data(), GL_STATIC_DRAW);
		
		// For reference
		//                     Loc Size  Type    Normalize      Stride        Pos offset
		// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

                glm::mat4 trans, model, view, projection;
		glm::mat3 normal;
		glm::vec3 cubeLightPos = glm::vec3(1.2f, 1.0f, -1.5f);
		glm::vec3 lightColor = glm::vec3(1.0f, 0.9f, 0.9f);
		trans = Identity;

                // TODO: Properly calculate aspect ratio (move shader/gl stuff to shader class)
		projection = glm::perspective(glm::radians(s_Camera->GetZoom()), 1.0f, 0.1f, 100.0f);

                // Clones
                // std::array<glm::vec3, 10> cubeClones = {
		// 	glm::vec3( 0.0f,  0.0f,  0.0f),
		// 	glm::vec3( 2.0f,  5.0f, -15.0f),
		// 	glm::vec3(-1.5f, -2.2f, -2.5f),
		// 	glm::vec3(-3.8f, -2.0f, -12.3f), 
		// 	glm::vec3( 2.4f, -0.4f, -3.5f),
		// 	glm::vec3(-1.7f,  3.0f, -7.5f), 
		// 	glm::vec3( 1.3f, -2.0f, -2.5f), 
		// 	glm::vec3( 1.5f,  2.0f, -2.5f),
		// 	glm::vec3( 1.5f,  0.2f, -1.5f),
		// 	glm::vec3(-1.3f,  1.0f, -1.5f)
		// };

                // float angle;
		model = glm::rotate(trans, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, -0.5f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
		normal = glm::mat3(glm::transpose(glm::inverse(model)));
		glm::mat4 cubeLightModel = glm::translate(model, cubeLightPos);
		
                while (m_Running) {
			// glEnable(GL_DEBUG_OUTPUT);
                        PollEvent();
			view = s_Camera->GetViewMatrix();
			projection = glm::perspective(glm::radians(s_Camera->GetZoom()), 1.0f, 0.1f, 100.0f);

			glEnable(GL_DEPTH_TEST);
                        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glActiveTexture(GL_TEXTURE0);
			// glBindTexture(GL_TEXTURE_2D, Image.texture[0]);
			// cubeShader.BindTexture(GL_TEXTURE_2D, 0);
			
                        // !!! Remember to bind used objects and shaders before setting uniforms and drawing !!!
			
			float time = Clock::GetTime();
			cubeShader.Use();
			cubeShader.SetFloatUni("u_time", time);
			cubeShader.SetFloatUni("u_time2", time);
			cubeShader.SetMat4FUni("u_model", model);
			cubeShader.SetMat4FUni("u_view", view);
			cubeShader.SetMat4FUni("u_projection", projection);
			cubeShader.SetMat3FUni("u_normal", normal);
			cubeShader.SetVec3Uni("u_lightColor", lightColor);
			cubeShader.SetVec3Uni("u_objectColor", glm::vec3(0.6f, 1.0f, 0.7f));
			cubeShader.SetVec3Uni("u_lightSourcePos", cubeLightPos);
			
			// glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
			// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eab[0]);
                        glBindVertexArray(vao[0]);
			glDrawArrays(GL_TRIANGLES, 0, cube.Vertices.size());
			
			lightSourceShader.Use();
			lightSourceShader.SetMat4FUni("u_model", cubeLightModel);
			lightSourceShader.SetMat4FUni("u_view", view);
			lightSourceShader.SetMat4FUni("u_projection", projection);

			// glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
			// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eab[0]);
			glBindVertexArray(vao[1]);
			glDrawElements(GL_TRIANGLES, lightSource.Idx.size(), GL_UNSIGNED_INT, 0);
			
			// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // wireframe mode
			
                        // for (int i = 0; i < 10; i++) {
			// 	model = glm::rotate(trans, 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
                        //         model = glm::translate(model, cubeClones[i]);
			// 	angle = 20.0f * i;
			// 	model = glm::rotate(model, angle, glm::vec3(1.0f, 1.0f, 0.0f));
			// 	s_Shader->SetMat4FUni("u_model", model);
			// 	s_Shader->SetMat4FUni("u_view", view);
			// 	s_Shader->SetMat4FUni("u_projection", projection);
				
			// 	glDrawElements(GL_TRIANGLES, Mesh::Idx.size(), GL_UNSIGNED_INT, 0);
			// 	// glDrawArrays(GL_TRIANGLES, 0, Vertices.size());
				// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // wireframe mode
			// };

                        // Gui->Run();
			
			SDL_GL_SwapWindow(m_WindowHandle);
		};
        }

        void Application::PollEvent() {
		// SDL_GetTicks64();
		Clock::UpdateDeltaTime();
		float deltaTime = Clock::GetDeltaTime();
		SDL_Event event;
                int x = 0, y = 0, dx = 0, dy = 0;
		Uint32 relativeMouseState;
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

		relativeMouseState = SDL_GetRelativeMouseState(&dx, &dy);
		if (SDL_GetRelativeMouseMode() == SDL_TRUE) {
			// printf("relativeMouseState: %u\n", mouseDelta);
			s_Camera->ProcessMouseInput(dx, dy, deltaTime);
		}
		
		if (keyStates[SDL_SCANCODE_W] >= 1)
			s_Camera->ProcessKeyInput(FORWARD, deltaTime);
		else if (keyStates[SDL_SCANCODE_S] >= 1)
			s_Camera->ProcessKeyInput(BACKWARD, deltaTime);
		if (keyStates[SDL_SCANCODE_D] >= 1)
			s_Camera->ProcessKeyInput(RIGHT, deltaTime);
		else if (keyStates[SDL_SCANCODE_A] >= 1)
			s_Camera->ProcessKeyInput(LEFT, deltaTime);
		
		// Could sum up events into a bitmask instead of if branches for poll event?
        }
}
