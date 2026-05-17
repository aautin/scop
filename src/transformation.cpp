#include "transformation.h"

// Project headers
#include "matrix.h"
#include "glfwHandling.h"

void moveLightBackward(SUser* user, float delta)
{
    user->lightPosition.z += delta;
}

void moveLightForward(SUser* user, float delta)
{
    user->lightPosition.z -= delta;
}

void moveLightLeft(SUser* user, float delta)
{
    user->lightPosition.x -= delta;
}

void moveLightRight(SUser* user, float delta)
{
    user->lightPosition.x += delta;
}

void moveCameraBackward(SUser* user, float delta)
{
    auto front = cameraFront(user->cameraYaw, user->cameraPitch);
    user->cameraPosition -= delta * front;
}

void moveCameraForward(SUser* user, float delta)
{
    auto front = cameraFront(user->cameraYaw, user->cameraPitch);
    user->cameraPosition += delta * front;
}

void moveCameraLeft(SUser* user, float delta)
{
    auto front = cameraFront(user->cameraYaw, user->cameraPitch);
    user->cameraPosition -= delta * normalize(cross(front, user->cameraUp));
}

void moveCameraRight(SUser* user, float delta)
{
    auto front = cameraFront(user->cameraYaw, user->cameraPitch);
    user->cameraPosition += delta * normalize(cross(front, user->cameraUp));
}

void rotateModelXPos(SUser* user, float delta)
{
    if (user->selectedRotationMatrix == nullptr)
    {
        return;
    }

    *user->selectedRotationMatrix = rotate(*user->selectedRotationMatrix, delta, {1.0f, 0.0f, 0.0f});
}

void rotateModelXNeg(SUser* user, float delta)
{
    if (user->selectedRotationMatrix == nullptr)
    {
        return;
    }

    *user->selectedRotationMatrix = rotate(*user->selectedRotationMatrix, delta, {-1.0f, 0.0f, 0.0f});
}

void rotateModelYPos(SUser* user, float delta)
{
    if (user->selectedRotationMatrix == nullptr)
    {
        return;
    }

    *user->selectedRotationMatrix = rotate(*user->selectedRotationMatrix, delta, {0.0f, 1.0f, 0.0f});
}

void rotateModelYNeg(SUser* user, float delta)
{
    if (user->selectedRotationMatrix == nullptr)
    {
        return;
    }

    *user->selectedRotationMatrix = rotate(*user->selectedRotationMatrix, delta, {0.0f, -1.0f, 0.0f});
}

void rotateModelZPos(SUser* user, float delta)
{
    if (user->selectedRotationMatrix == nullptr)
    {
        return;
    }

    *user->selectedRotationMatrix = rotate(*user->selectedRotationMatrix, delta, {0.0f, 0.0f, 1.0f});
}

void rotateModelZNeg(SUser* user, float delta)
{
    if (user->selectedRotationMatrix == nullptr)
    {
        return;
    }

    *user->selectedRotationMatrix = rotate(*user->selectedRotationMatrix, delta, {0.0f, 0.0f, -1.0f});
}

void closeApplication(SUser* user, float)
{
    glfwSetWindowShouldClose(glfwGetCurrentContext(), true);
}
