#include <vector>
#include <SDL_opengl.h>
#include <glm/glm.hpp>

namespace RT {
	class Mesh {    // interface
        public:
		struct Vertex {
			glm::vec3 pos;
			glm::vec3 normal;
			glm::vec2 uv;
		};
                virtual ~Mesh() {};
                virtual void Init(float scale = 1.0f) = 0;
                virtual void PopulateVertices(float scale = 1.0f) = 0;
		virtual void PopulateNormals() = 0;
                // virtual void PopulateUVs() = 0;
                virtual void PopulateIdx() = 0;
		// TODO: Implement materials
		/* void setMaterial(); */

		std::vector<Vertex> Vertices;
		std::vector<GLuint> Idx;
        };

        class Cube : public Mesh {
        public:
		Cube() {};
		~Cube();
		void Init(float scale = 1.0f) override;
		void PopulateVertices(float scale = 1.0f) override;
		void PopulateNormals() override;
		void PopulateIdx() override;
		void PopulateIndexedVertices(float scale = 1.0f);
	};

        // class Cube : virtual public Mesh {
	// 	Cube();
	// 	~Cube();
	// };
}
