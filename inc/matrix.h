#pragma once

// Project headers
#include "math3d.h"

struct SUser;

SVec3 cameraFront(float yaw, float pitch);
SMat4 getViewMatrix(const SUser& user);
