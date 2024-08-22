#include "Mesh.h"
#include <glm/geometric.hpp>
#include <sys/types.h>

namespace RT {
	Cube::~Cube() {
		Vertices.clear();
		Idx.clear();
		// delete this;
	}
        void Cube::Init(float scale) {
		// PopulateVertices(scale);
		// PopulateVertNormals();
		// PopulateIdx();
        }
        void Cube::PopulateVertices(float scale) {
		float n = 1.0f * scale * 0.5f;
                Vertices.clear();
                Vertices.reserve(36);
		
		using namespace glm;
                Vertices = {
			{vec3(-n,-n,-n), vec3(0.0f,0.0f,-1.0f), vec2(0.0f,0.0f)},
			{vec3(n,-n,-n), vec3(0.0f,0.0f,-1.0f), vec2(1.0f,0.0f)},
			{vec3(n,n,-n), vec3(0.0f,0.0f,-1.0f), vec2(1.0f,1.0f)},
			{vec3(n,n,-n), vec3(0.0f,0.0f,-1.0f), vec2(1.0f,1.0f)},
			{vec3(-n,n,-n), vec3(0.0f,0.0f,-1.0f), vec2(0.0f,1.0f)},
			{vec3(-n,-n,-n), vec3(0.0f,0.0f,-1.0f), vec2(0.0f,0.0f)},

			{vec3(-n,-n,n), vec3(0.0f,0.0f,1.0f), vec2(0.0f,0.0f)},
			{vec3(n,-n,n), vec3(0.0f,0.0f,1.0f), vec2(1.0f,0.0f)},
			{vec3(n,n,n), vec3(0.0f,0.0f,1.0f), vec2(1.0f,1.0f)},
			{vec3(n,n,n), vec3(0.0f,0.0f,1.0f), vec2(1.0f,1.0f)},
			{vec3(-n,n,n), vec3(0.0f,0.0f,1.0f), vec2(0.0f,1.0f)},
			{vec3(-n,-n,n), vec3(0.0f,0.0f,1.0f), vec2(0.0f,0.0f)},

			{vec3(-n,n,n), vec3(-1.0f,0.0f,0.0f), vec2(1.0f,0.0f)},
			{vec3(-n,n,-n), vec3(-1.0f,0.0f,0.0f), vec2(1.0f,1.0f)},
			{vec3(-n,-n,-n), vec3(-1.0f,0.0f,0.0f), vec2(0.0f,1.0f)},
			{vec3(-n,-n,-n), vec3(-1.0f,0.0f,0.0f), vec2(0.0f,1.0f)},
			{vec3(-n,-n,n), vec3(-1.0f,0.0f,0.0f), vec2(0.0f,0.0f)},
			{vec3(-n,n,n), vec3(-1.0f,0.0f,0.0f), vec2(1.0f,0.0f)},

			{vec3(n,n,n), vec3(1.0f,0.0f,0.0f), vec2(1.0f,0.0f)},
			{vec3(n,n,-n), vec3(1.0f,0.0f,0.0f), vec2(1.0f,1.0f)},
			{vec3(n,-n,-n), vec3(1.0f,0.0f,0.0f), vec2(0.0f,1.0f)},
			{vec3(n,-n,-n), vec3(1.0f,0.0f,0.0f), vec2(0.0f,1.0f)},
			{vec3(n,-n,n), vec3(1.0f,0.0f,0.0f), vec2(0.0f,0.0f)},
			{vec3(n,n,n), vec3(1.0f,0.0f,0.0f), vec2(1.0f,0.0f)},

			{vec3(-n,-n,-n), vec3(0.0f,-1.0f,0.0f), vec2(0.0f,1.0f)},
			{vec3(n,-n,-n), vec3(0.0f,-1.0f,0.0f), vec2(1.0f,1.0f)},
			{vec3(n,-n,n), vec3(0.0f,-1.0f,0.0f), vec2(1.0f,0.0f)},
			{vec3(n,-n,n), vec3(0.0f,-1.0f,0.0f), vec2(1.0f,0.0f)},
			{vec3(-n,-n,n), vec3(0.0f,-1.0f,0.0f), vec2(0.0f,0.0f)},
			{vec3(-n,-n,-n), vec3(0.0f,-1.0f,0.0f), vec2(0.0f,1.0f)},

			{vec3(-n,n,-n), vec3(0.0f,1.0f,0.0f), vec2(0.0f,1.0f)},
			{vec3(n,n,-n), vec3(0.0f,1.0f,0.0f), vec2(1.0f,1.0f)},
			{vec3(n,n,n), vec3(0.0f,1.0f,0.0f), vec2(1.0f,0.0f)},
			{vec3(n,n,n), vec3(0.0f,1.0f,0.0f), vec2(1.0f,0.0f)},
			{vec3(-n,n,n), vec3(0.0f,1.0f,0.0f), vec2(0.0f,0.0f)},
			{vec3(-n,n,-n), vec3(0.0f,1.0f,0.0f), vec2(0.0f,1.0f)}
		};
	}
	void Cube::PopulateIndexedVertices(float scale) {
		float n = 1.0f * scale * 0.5f;
		// int uvOffset = 36;
		
		Vertices.clear();
		Vertices.reserve(8);
		for (int i = 0; i < 8; i++) {
			Vertices.push_back(Mesh::Vertex());
		}
		
		// Orthogonal to xz plane
		// Back
		Vertices[0].pos = (glm::vec3(n, n, -n));  // top right
		Vertices[1].pos = (glm::vec3(n, -n, -n));  // bot right
		Vertices[2].pos = (glm::vec3(-n, -n, -n));  // bot left
		Vertices[3].pos = (glm::vec3(-n, n, -n));   // top left
		
		// Front
		Vertices[4].pos = (glm::vec3(n, n, n));  // top right
		Vertices[5].pos = (glm::vec3(n, -n, n));  // bot right
		Vertices[6].pos = (glm::vec3(-n, -n, n));  // bot left
		Vertices[7].pos = (glm::vec3(-n, n, n));   // top left

		Vertices[0].uv = (glm::vec2(1.0f, 1.0f));  // top right
		Vertices[1].uv = (glm::vec2(1.0f, 0.0f));  // bot right
		Vertices[2].uv = (glm::vec2(0.0f, 0.0f));  // bot left
		Vertices[3].uv = (glm::vec2(0.0f, 1.0f));  // top left
				
		Vertices[4].uv = (glm::vec2(0.0f, 1.0f));  // top left
		Vertices[5].uv = (glm::vec2(0.0f, 0.0f));  // bot left
		Vertices[6].uv = (glm::vec2(1.0f, 0.0f));  // bot right
		Vertices[7].uv = (glm::vec2(1.0f, 1.0f));  // top right

		PopulateIdx();
	}
		
	void Cube::PopulateIdx() {
		// Indices
		Idx.clear();
		Idx.reserve(36);
		// back
		Idx.push_back(0);
		Idx.push_back(1);
		Idx.push_back(2);
		Idx.push_back(2);
		Idx.push_back(3);
		Idx.push_back(0);
		// front
		Idx.push_back(4);
		Idx.push_back(5);
		Idx.push_back(6);
		Idx.push_back(6);
		Idx.push_back(7);
		Idx.push_back(4);
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
		Idx.push_back(0);
		Idx.push_back(4);
		Idx.push_back(7);
		Idx.push_back(7);
		Idx.push_back(3);
		Idx.push_back(0);
		// bot
		Idx.push_back(5);
		Idx.push_back(1);
		Idx.push_back(2);
		Idx.push_back(2);
		Idx.push_back(6);
		Idx.push_back(5);
	}
	void Cube::PopulateNormals() {
		// glm::vec3 v1, v2;
                // u_int8_t count = 0;
		// Normals.clear(); Normals.reserve(Pos.size());
		// for (std::vector<Vertex>::size_type i = Pos.size()-1; i != 1; i--) {
		// 	switch (count) {
		// 	case 0:
		// 		v1 = Pos[i] - Pos[i-1];
		// 	case 1:
		// 		v2 = Pos[i] - Pos[i-1];
		// 		Normals[i] = glm::normalize(glm::cross(v1, v2));
		// 		Normals[i-1] = glm::normalize(glm::cross(v1, v2));
		// 		count = 0;
		// 	}
		// 	count++;
		// }
		// for (std::vector<Vertex>::size_type i = Vertices.size(); i != 1; i--) {
		// 	// v12 = Verticies[i].pos - Vertices[i-1].pos;
		// 	v12 = glm::normalize(glm::cross(Vertices[i].pos, Vertices[i-1].pos));
		// 	Vertices[i].normal = glm::vec3(v12);
                // }
		// v12 = glm::normalize(glm::cross(Vertices.end()->pos, Vertices.begin()->pos));
		// Vertices.begin()->normal = glm::vec3(v12);
	}
}
