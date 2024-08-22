#include <glm/fwd.hpp>
#include <vector>
#include <array>
#include <SDL_opengl.h>
#include <glm/glm.hpp>

#define EQ_TRI_RATIO 0.86602540f

namespace RT {
	namespace Light {
		static std::vector<glm::vec3> Vertices;
	}
	namespace Mesh {
		struct Vertex {
			glm::vec3 pos;
			glm::vec2 uv;
		}; 
		struct Triangle {
			Triangle(float height, float width) : Height(height), Width(width) {}
			float Height, Width;
		};
		static std::vector<GLuint> Idx;
		static std::vector<Vertex> Vertices;
	}; // namespace Mesh

        // https://schneide.blog/2016/07/15/generating-an-icosphere-in-c/
	// https://github.com/anishagartia/Icosahedron_OpenGL/blob/master/icosahedron.cc#L112
	namespace Icosahedron
	{
		const float X=.525731112119133606f;
		const float Z=.850650808352039932f;
		const float N=0.0f;

                static const std::array<glm::vec3, 12> vertices =
		{
			glm::vec3(-X,N,Z), glm::vec3(X,N,Z), glm::vec3(-X,N,-Z), glm::vec3(X,N,-Z),
			glm::vec3(N,Z,X), glm::vec3(N,Z,-X), glm::vec3(N,-Z,X), glm::vec3(N,-Z,-X),
			glm::vec3(Z,X,N), glm::vec3(-Z, X, N), glm::vec3(Z,-X,N), glm::vec3(-Z,-X, N)
		};
		
                static const std::array<glm::vec2, 12> uv =
		{
			glm::vec2(0,0), glm::vec2(0,0), glm::vec2(0,0), glm::vec2(0,0),
			glm::vec2(0,0), glm::vec2(0,0), glm::vec2(0,0), glm::vec2(0,0),
			glm::vec2(0,0), glm::vec2(0,0), glm::vec2(0,0), glm::vec2(0,0)
		};
 
		static const std::array<GLuint, 60> indices=
		{
			0,4,1,0,9,4,9,5,4,4,5,8,4,8,1,
			8,10,1,8,3,10,5,3,8,5,2,3,2,7,3,
			7,10,3,7,6,10,7,11,6,11,0,6,0,1,6,
			6,1,10,9,0,11,9,11,2,9,2,5,7,2,11
		};
                // static const std::array<float, 36> vertices =
		// {
		// 	-X,N,Z, X,N,Z, -X,N,-Z, X,N,-Z,
		// 	N,Z,X, N,Z,-X, N,-Z,X, N,-Z,-X,
		// 	Z,X,N, -Z, X, N, Z,-X,N, -Z,-X, N
		// };
 
		// static const std::array<GLuint, 60> indices=
		// {
		// 	0,4,1,0,9,4,9,5,4,4,5,8,4,8,1,
		// 	8,10,1,8,3,10,5,3,8,5,2,3,2,7,3,
		// 	7,10,3,7,6,10,7,11,6,11,0,6,0,1,6,
		// 	6,1,10,9,0,11,9,11,2,9,2,5,7,2,11
		// };
		// static const std::array<GLuint, 3> indices[20]=
		// {
		// 	{0,4,1},{0,9,4},{9,5,4},{4,5,8},{4,8,1},
		// 	{8,10,1},{8,3,10},{5,3,8},{5,2,3},{2,7,3},
		// 	{7,10,3},{7,6,10},{7,11,6},{11,0,6},{0,1,6},
		// 	{6,1,10},{9,0,11},{9,11,2},{9,2,5},{7,2,11}
		// };
	};
	
	using namespace Mesh;
        namespace Shape {
		static void Normalize(glm::vec3 &vec) {
			float s = 1.0f / sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
			vec.x = vec.x*s;
			vec.y = vec.y*s;
			vec.z = vec.z*s;
		};
		static void GenCircle(float radius, int vertCount) {
			int TriCount = vertCount - 2;
			float Angle = 360.0f / vertCount;
			// float PI = glm::pi<float>();
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
			float n_width = width * scale * 0.5;
			float n_height = height * scale * 0.5;
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
			float n_width = width * scale * 0.5;
			float n_height = height * scale * 0.5;
			float n_length = length * scale * 0.5;
			// int uvOffset = 36;
		
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

		static void SubdivideTriangle(const glm::vec3 &v1, const glm::vec3 &v2, const glm::vec3 &v3, int level) {
			// Assumes Vertices and Idx have been cleared/reserved in advanced
			// I may need to change how my vertex struct works
			if (level == 0) return;
			else {
				glm::vec3 v12, v23, v13;
				v12 = glm::vec3((v1.x + v2.x)/2, (v1.y + v2.y)/2, 0.0f);
				v23 = glm::vec3((v2.x + v3.x)/2, (v2.y + v3.y)/2, 0.0f);
				v13 = glm::vec3((v1.x + v3.x)/2, (v1.y + v3.y)/2, 0.0f);
				
				// float s;
				// s = 1.0f / sqrt(v12.x * v12.x + v12.y * v12.y + v12.z * v12.z);
				// v12 = glm::vec3(v12.x * s, v12.y * s, v12.z * s);
				
				// s = 1.0f / sqrt(v23.x * v23.x + v23.y * v23.y + v23.z * v23.z);
				// v23 = glm::vec3(v23.x * s, v23.y * s, v23.z * s);
				
				// s = 1.0f / sqrt(v13.x * v13.x + v13.y * v13.y + v13.z * v13.z);
				// v13 = glm::vec3(v13.x * s, v13.y * s, v13.z * s);

				Normalize(v12);
				Normalize(v23);
                                Normalize(v13);

                                int size = Vertices.size();
				Vertices.reserve(size + 3);
				for (int i = 0; i < 3; i++)
					Vertices.push_back(Vertex());
				Vertices[size].pos =   (v12);
				Vertices[size+1].pos = (v23);
				Vertices[size+2].pos = (v13);

                                // Recursive calls
				SubdivideTriangle(v1, v12, v13, level - 1);        // left
				SubdivideTriangle(v12, v2, v23, level - 1);        // right
				SubdivideTriangle(v13, v23, v3, level - 1);        // top
				SubdivideTriangle(v12, v23, v13, level - 1);   // middle (how does this work?)
			}
                }

                static void GenIcosahedron(float scale) {
			Vertices.clear();
			Idx.clear();
			Vertices.reserve(12);
			Idx.reserve(60);   
			for (int i = 0; i < 12; i++) {
				Vertices.push_back(Vertex());
				Vertices[i].pos = Icosahedron::vertices[i];
				Vertices[i].uv = glm::vec2(0.0f, 0.0f);
                        }
                        for (int i = 0; i < 60; i++) {
				Idx.push_back(Icosahedron::indices[i]);
			}
                }

                // static void GenIcosphere(int level) {
		// 	GenIcosahedron(1.0f);
		// 	for (int i = 0; i < 12; i++) {
		// }

		static void GenIcosphere(float scale, float radius, int level) {
			const float height = scale * EQ_TRI_RATIO;
			const float width = scale;
			const float n_top =  height * 0.5;  // v3
			const float n_right = width * 0.5;  // v2
			const float n_bot = -n_top;
			const float n_left = -n_right;      // v1
			glm::vec3 v1, v2, v3;
			v1 = glm::vec3(n_left, n_bot, 0.0f);
			v2 = glm::vec3(n_right, n_bot, 0.0f);
			v3 = glm::vec3(0.0f, n_top, 0.0f);
			
			// Vertices.clear();
			// Idx.clear();
			// Vertices.reserve(3);  // change to correct reserve
			// Idx.reserve(3);       // change to correct reserve

			// GenIcosahedron(1.0f);
			for (int i = 0; i < 3; i++)
				Vertices.push_back(Vertex());

			Vertices[0].pos = (v1);  // left
			Vertices[1].pos = (v2);  // right
			Vertices[2].pos = (v3);  // top
			Vertices[0].uv = (glm::vec2(0.0f, 0.0f));  // left
			Vertices[1].uv = (glm::vec2(1.0f, 0.0f));  // right
			Vertices[2].uv = (glm::vec2(0.5f, 1.0f));  // top
			SubdivideTriangle(v1, v2, v3, level);
		}
	};
}
