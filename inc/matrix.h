#pragma once

// Project headers
#include "types.h"
#include "user.h"

glm::vec3 cameraFront(float yaw, float pitch);
glm::mat4 getViewMatrix(const SUser& user);
