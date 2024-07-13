#include <vector>
#include <GL/glew.h>
#include <string>
#include <SDL_opengl.h>
#include <glm/glm.hpp>

namespace RT {
	struct ImageData {
		int width, height, nrChannels;
		std::vector<unsigned int> texture;
		unsigned char* data;
	};
	class Shader {
        public:
		Shader(const char *vertPath, const char *fragPath);
		~Shader();
		
		void CheckCompileErrors(GLuint id);
		void SetBoolUni(const char* name, const bool b) const; 
		void SetIntUni(const char* name, const int i) const; 
		void SetFloatUni(const char* name, const float f) const;
                void SetMat4FUni(const char *name, const glm::mat4 &matrix);

		void Use();
                void GenTexture(const std::string &path, int index);
		void BindTexture(GLenum target, unsigned int index);
	private:
		GLuint ID;
		ImageData Image;
	};
}
