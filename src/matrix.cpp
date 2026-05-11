#include "matrix.h"

glm::vec3 cameraFront(float yaw, float pitch)
{
	// Convert to radians
	float yawRad = glm::radians(yaw);
	float pitchRad = glm::radians(pitch);

	// Compute front vector
	glm::vec3 front;
	front.x = cos(yawRad) * cos(pitchRad);
	front.y = sin(pitchRad);
	front.z = sin(yawRad) * cos(pitchRad);
	return glm::normalize(front);
}

glm::mat4 getViewMatrix(const SUser& user)
{
	glm::vec3 front = cameraFront(user.cameraYaw, user.cameraPitch);
	glm::vec3 right = glm::normalize(glm::cross(front, user.cameraUp));

    return glm::lookAt(user.cameraPosition, user.cameraPosition + front, user.cameraUp);
}
