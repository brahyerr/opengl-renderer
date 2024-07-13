#include "Shader.h"
#include <SDL_opengl.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


namespace RT {
	Shader::Shader(const char *vertPath, const char *fragPath) {
		GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

		// Read the Vertex Shader code from the file
		std::string VertexShaderCode;
		std::ifstream VertexShaderStream(vertPath, std::ios::in);
		if (VertexShaderStream.is_open()) {
			std::stringstream sstr;
			sstr << VertexShaderStream.rdbuf();
			VertexShaderCode = sstr.str();
			VertexShaderStream.close();
		}
		else {
			printf("Unable to open %s.\n", vertPath);
			Shader::~Shader();
		}

		// Read the Fragment Shader code from the file
		std::string FragmentShaderCode;
		std::ifstream FragmentShaderStream(fragPath, std::ios::in);
		if (FragmentShaderStream.is_open()) {
			std::stringstream sstr;
			sstr << FragmentShaderStream.rdbuf();
			FragmentShaderCode = sstr.str();
			FragmentShaderStream.close();
		}

		// Compile Vertex Shader
		printf("Compiling shader : %s\n", vertPath);
		char const* VertexSourcePointer = VertexShaderCode.c_str();
		glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
		glCompileShader(VertexShaderID);
		CheckCompileErrors(VertexShaderID);

		// Compile Fragment Shader
		printf("Compiling shader : %s\n", fragPath);
		char const* FragmentSourcePointer = FragmentShaderCode.c_str();
		glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
		glCompileShader(FragmentShaderID);
		CheckCompileErrors(FragmentShaderID);

		// Link the program
		printf("Linking program\n");
		GLuint ProgramID = glCreateProgram();
		glAttachShader(ProgramID, VertexShaderID);
		glAttachShader(ProgramID, FragmentShaderID);
		glLinkProgram(ProgramID);

                // Check the program
                CheckCompileErrors(ProgramID);
		
		glDetachShader(ProgramID, VertexShaderID);
		glDetachShader(ProgramID, FragmentShaderID);

		glDeleteShader(VertexShaderID);
                glDeleteShader(FragmentShaderID);
                ID = ProgramID;
        }

        Shader::~Shader() {
		glDeleteProgram(ID);
        }

        void Shader::CheckCompileErrors(GLuint id) {
		GLint Result = GL_FALSE;
                int InfoLogLength;
		glGetShaderiv(id, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (Result == GL_FALSE && InfoLogLength > 0) {
			std::vector<char> ErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(id, InfoLogLength, NULL, &ErrorMessage[0]);
			printf("%s\n", &ErrorMessage[0]);
		}
	}
	
        void Shader::SetBoolUni(const char* name, const bool b) const {
		glUniform1i(glGetUniformLocation(ID, name), (int) b);
	}
        void Shader::SetIntUni(const char* name, const int i) const {
		glUniform1f(glGetUniformLocation(ID, name), i);
        }
        void Shader::SetFloatUni(const char *name, const float f) const {
		glUniform1f(glGetUniformLocation(ID, name), f);
        } 
	void Shader::SetMat4FUni(const char* name, const glm::mat4 &matrix) {
		glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, glm::value_ptr(matrix));
	}

        void Shader::GenTexture(const std::string &path, int index) {
		Image.texture.resize(Image.texture.size() + 1);
		stbi_set_flip_vertically_on_load(true);  // must flip since images usually have y = 0.0 on the top, while openGL has it on the bottom
		Image.data = stbi_load(&path[0], &(Image.width), &(Image.height), &(Image.nrChannels), 0);
		
                glGenTextures(1, &(Image.texture[index]));
		glBindTexture(GL_TEXTURE_2D, Image.texture[index]);
		
		// set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		
		if (Image.data) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Image.width, Image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Image.data);
                        glGenerateMipmap(GL_TEXTURE_2D);
                } else {
			std::cout << "Failed to load texture!" << std::endl;
                }
		stbi_image_free(Image.data);
        }

        void Shader::BindTexture(GLenum target, unsigned int index) {
		glBindTexture(target, Image.texture[index]);
	}
	
        void Shader::Use() {
		glUseProgram(ID);
	};
}
