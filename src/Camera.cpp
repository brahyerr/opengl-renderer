#include "Camera.h"
#include "Clock.h"
#include <glm/ext/matrix_transform.hpp>
#include <iostream>

namespace RT {
	Camera::Camera(glm::vec3 pos, glm::vec3 up, float yaw, float pitch)
		: Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), Sensitivity(SENSITIVITY), Zoom(ZOOM) {
		Pos = pos;
		WorldUp = up;
		Yaw = yaw;
                Pitch = pitch;
		updateVectors();
	}

        Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
		: Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), Zoom(ZOOM) {
		Pos = glm::vec3(posX, posY, posZ);
                WorldUp = glm::vec3(upX, upY, upZ);
                Yaw = yaw;
		Pitch = pitch;
		updateVectors();
        }

        glm::mat4 Camera::GetViewMatrix() {
		return glm::lookAt(Pos, Pos + Front, Up);
	}

        float Camera::GetZoom() {
		return Zoom;
	}

	void Camera::SetZoom(float zoom) {
		Zoom = zoom;
	}

        void Camera::updateVectors() {
		glm::vec3 frontRaw;
		frontRaw.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		frontRaw.y = sin(glm::radians(Pitch));
		frontRaw.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
                Front = glm::normalize(frontRaw);
		
		// normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Front, Right));
	}
	void Camera::ProcessKeyInput(Camera_Movement direction, float deltaTime) {
		if (direction == FORWARD)
			Pos += Front * MovementSpeed * deltaTime;
		else if (direction == BACKWARD)
			Pos -= Front * MovementSpeed * deltaTime;
		if (direction == LEFT)
			Pos -= Right * MovementSpeed * deltaTime;
		else if (direction == RIGHT)
			Pos += Right * MovementSpeed * deltaTime;
		// printf("dir: %i, pos: (%f, %f, %f)\n", direction, Pos.x, Pos.y, Pos.z);
	}
	
	void Camera::ProcessMouseInput(int x, int y, float deltaTime) {
		Yaw += Sensitivity * x * deltaTime;
                Pitch -= Sensitivity * y * deltaTime;
		// if(Yaw >= 360.0f) Yaw -= 360.0f;
		// else if(Yaw <= -360.0f) Yaw += 360.0f;
		if(Pitch > 89.0f)
			Pitch =  89.0f;
		if(Pitch < -89.0f)
			Pitch = -89.0f;
		// printf("yaw: %f, pitch: %f\n", Yaw, Pitch);
		updateVectors();
	}
}
