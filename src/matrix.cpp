#include "matrix.h"

// Project headers
#include "user.h"

SVec3 cameraFront(float yaw, float pitch)
{
	// Convert to radians
	float yawRad = radians(yaw);
	float pitchRad = radians(pitch);

	// Compute front vector
	SVec3 front;
	front.x = cos(yawRad) * cos(pitchRad);
	front.y = sin(pitchRad);
	front.z = sin(yawRad) * cos(pitchRad);
	return normalize(front);
}

SMat4 getViewMatrix(const SUser& user)
{
	SVec3 front = cameraFront(user.cameraYaw, user.cameraPitch);
	return lookAt(user.cameraPosition, user.cameraPosition + front, user.cameraUp);
}
