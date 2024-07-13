#include <cmath>
#include <cstddef>
#include <ctime>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace RT {
	enum Camera_Movement {
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT
	};
		
	const float YAW = -90.0f;
	const float PITCH = 0.0f;
	const float SPEED = 2.5f;
	const float SENSITIVITY = 2.0f;
	const float ZOOM = -45.0f;

	class Camera {
        public:
		// initialize with vectors
		Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f),
		       glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
		       float yaw = YAW, float pitch = PITCH);
		
		// initialize with scalars
		Camera(float posX, float posY, float posZ,
		       float upX, float upY, float upZ,
		       float yaw, float pitch);

		void updateVectors();
		void ProcessKeyInput(Camera_Movement direction, float deltaTime);
		void ProcessMouseInput(int x, int y, float deltaTime);
		void SetZoom(float zoom);
		
		float GetZoom();
		glm::mat4 GetViewMatrix();
	private:
		glm::vec3 Pos, Front, WorldUp, Up, Right; // Up and Right are derived
		float Yaw, Pitch, MovementSpeed, Sensitivity, Zoom;
	};
}
