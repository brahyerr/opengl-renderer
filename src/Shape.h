#include <vector>
#include <SDL_opengl.h>
#include <glm/glm.hpp>

namespace RT::Shape {
	static std::vector<GLuint> Idx;
	struct Vertex {
		glm::vec3 pos;
		glm::vec2 uv;
        }; 
	static std::vector<Vertex> Vertices;
        static void GenCircle(float radius, int vertCount) {
		const int TriCount = vertCount - 2;
		const float Angle = 360.0f / vertCount;
		// const float PI = glm::pi<float>();
                float theta, x, y;
		
		Vertices.clear();
		Idx.clear();
		Vertices.reserve(vertCount);
		Idx.reserve(vertCount);
		
                for (int i = 0; i < vertCount; i++) {
			theta = i * Angle;
			x = radius * cos(glm::radians(theta));
			y = radius * sin(glm::radians(theta));
                        Vertices.push_back(Vertex());
			Vertices[i].pos = (glm::vec3(x, y, 0.0f));
			Vertices[i].uv = (glm::vec2(0.0f + 1.0f * (x / radius + 1) * 0.5f, 0.0f + 1.0f * (y / radius + 1)*0.5f));
			// uv->push_back(glm::vec2(1.2 * ((cos(glm::radians(theta))+1) * 0.5), (1.2 * (cos((glm::radians(theta))-pi*1.5)+1) * 0.5)));
			// std::cout << "(" << uv[0][i].y << ", " << uv[0][i].y << ")" << std::endl;
                        if (i < TriCount) {
				Idx.push_back(0);
				Idx.push_back(i+1);
				Idx.push_back(i+2);
			};
                }
        }
	static void GenTri(float scale, float top, float right, float left) {
		// origin is aligned with the top y axis
		// so right is right from top, and left is left from top
		const float n_top = top * scale * 0.25;
		const float n_right = right * scale * 0.5;
		const float n_left = -left * scale * 0.5;
		Vertices.clear();
                Idx.clear();
		Vertices.reserve(3);
                Idx.reserve(3);
		
                Vertices.push_back(Vertex());
                Vertices.push_back(Vertex());
                Vertices.push_back(Vertex());
		
                Vertices[0].pos = (glm::vec3(0.0f, n_top, 0.0f));  // top
		Vertices[1].pos = (glm::vec3(n_right, -n_top, 0.0f));  // right
		Vertices[2].pos = (glm::vec3(n_left, -n_top, 0.0f));  // left
		Vertices[0].uv = (glm::vec2(0.5f, 1.0f));  // top
		Vertices[1].uv = (glm::vec2(1.0f, 0.0f));  // right
		Vertices[2].uv = (glm::vec2(0.0f, 0.0f));  // left
	  
		Idx.push_back(0);
		Idx.push_back(1);
		Idx.push_back(2);
        }
	static void GenQuad(float scale, float width, float height, float z) {
		const float n_width = width * scale * 0.5;
		const float n_height = height * scale * 0.5;
		Vertices.clear();
		Idx.clear();
		Vertices.reserve(4);
		Idx.reserve(6);

                for (int i = 0; i < 4; i++)
			Vertices.push_back(Vertex());
		
                Vertices[0].pos = glm::vec3(n_width, n_height, z);  // top right
		Vertices[1].pos = glm::vec3(n_width, -n_height, z);  // bot right
		Vertices[2].pos = glm::vec3(-n_width, -n_height, z);  // bot left
		Vertices[3].pos = glm::vec3(-n_width, n_height, z);   // top left
		
		Vertices[0].uv = glm::vec2(1.0f, 1.0f);  // top right
		Vertices[1].uv = glm::vec2(1.0f, 0.0f);  // bot right
	        Vertices[2].uv = glm::vec2(0.0f, 0.0f);  // bot left
                Vertices[3].uv = glm::vec2(0.0f, 1.0f);  // top left
	  
		Idx.push_back(0);
		Idx.push_back(1);
		Idx.push_back(2);
		Idx.push_back(2);
		Idx.push_back(3);
		Idx.push_back(0);
        }
	static void GenCube(float scale, float width, float height, float length) {
		// TODO: Fix vertex indices
		const float n_width = width * scale * 0.5;
                const float n_height = height * scale * 0.5;
                const float n_length = length * scale * 0.5;
		// const int uvOffset = 36;
		
		Vertices.clear();
		Idx.clear();
		Vertices.reserve(8);
		Idx.reserve(36);

                for (int i = 0; i < 16; i++)
			Vertices.push_back(Vertex());
		
		// Orthogonal to xz plane
		// Back
		Vertices[0].pos = (glm::vec3(n_width, n_height, -n_length));  // top right
		Vertices[1].pos = (glm::vec3(n_width, -n_height, -n_length));  // bot right
		Vertices[2].pos = (glm::vec3(-n_width, -n_height, -n_length));  // bot left
		Vertices[3].pos = (glm::vec3(-n_width, n_height, -n_length));   // top left
		
		// Front
		Vertices[4].pos = (glm::vec3(n_width, n_height, n_length));  // top right
		Vertices[5].pos = (glm::vec3(n_width, -n_height, n_length));  // bot right
		Vertices[6].pos = (glm::vec3(-n_width, -n_height, n_length));  // bot left
		Vertices[7].pos = (glm::vec3(-n_width, n_height, n_length));   // top left

		// Orthogonal to xy plane
		// Top
		Vertices[8].pos = (glm::vec3(n_width, n_height, -n_length));  // top right
		Vertices[9].pos = (glm::vec3(n_width, n_height, n_length));  // bot right
		Vertices[10].pos = (glm::vec3(-n_width, n_height, n_length));   // bot left
		Vertices[11].pos = (glm::vec3(-n_width, n_height, -n_length));   // top left

                // Bottom
		Vertices[12].pos = (glm::vec3(n_width, -n_height, -n_length));  // top right
		Vertices[13].pos = (glm::vec3(n_width, -n_height, n_length));  // bot right
		Vertices[14].pos = (glm::vec3(-n_width, -n_height, n_length));  // bot left
		Vertices[15].pos = (glm::vec3(-n_width, -n_height, -n_length));  // top left
		
		// Indicies
                // back
		Idx.push_back(0);
		Idx.push_back(1);
		Idx.push_back(2);
		Idx.push_back(2);
		Idx.push_back(3);
                Idx.push_back(0);
		// front
		Idx.push_back(7);
		Idx.push_back(6);
		Idx.push_back(5);
		Idx.push_back(5);
		Idx.push_back(4);
                Idx.push_back(7);
		// left
		Idx.push_back(7);
		Idx.push_back(6);
		Idx.push_back(2);
		Idx.push_back(2);
		Idx.push_back(3);
                Idx.push_back(7);
		// right
		Idx.push_back(0);
		Idx.push_back(1);
		Idx.push_back(5);
		Idx.push_back(5);
		Idx.push_back(4);
                Idx.push_back(0);
		// top
		Idx.push_back(8);
		Idx.push_back(9);
		Idx.push_back(10);
		Idx.push_back(10);
		Idx.push_back(11);
                Idx.push_back(8);
		// bot
		Idx.push_back(12);
		Idx.push_back(13);
		Idx.push_back(14);
		Idx.push_back(14);
		Idx.push_back(15);
                Idx.push_back(12);
		
		Vertices[0].uv = (glm::vec2(1.0f, 1.0f));  // top right
		Vertices[1].uv = (glm::vec2(1.0f, 0.0f));  // bot right
		Vertices[2].uv = (glm::vec2(0.0f, 0.0f));  // bot left
                Vertices[3].uv = (glm::vec2(0.0f, 1.0f));  // top left
				
		Vertices[4].uv = (glm::vec2(0.0f, 1.0f));  // top left
		Vertices[5].uv = (glm::vec2(0.0f, 0.0f));  // bot left
		Vertices[6].uv = (glm::vec2(1.0f, 0.0f));  // bot right
		Vertices[7].uv = (glm::vec2(1.0f, 1.0f));  // top right
		
		Vertices[8].uv = (glm::vec2(1.0f, 1.0f));  // top right
		Vertices[9].uv = (glm::vec2(1.0f, 0.0f));  // bot right
		Vertices[10].uv = (glm::vec2(0.0f, 0.0f));  // bot left
                Vertices[11].uv = (glm::vec2(0.0f, 1.0f));  // top left

		Vertices[12].uv = (glm::vec2(1.0f, 1.0f));  // top right
		Vertices[13].uv = (glm::vec2(1.0f, 0.0f));  // bot right
		Vertices[14].uv = (glm::vec2(0.0f, 0.0f));  // bot left
                Vertices[15].uv = (glm::vec2(0.0f, 1.0f));  // top left
	}
}
