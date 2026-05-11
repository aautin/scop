#pragma once

// Project headers
#include "user.h"

// Light movements
void moveLightBackward(SUser* user, float delta);
void moveLightForward(SUser* user, float delta);
void moveLightLeft(SUser* user, float delta);
void moveLightRight(SUser* user, float delta);

// Camera movements
void moveCameraBackward(SUser* user, float delta);
void moveCameraForward(SUser* user, float delta);
void moveCameraLeft(SUser* user, float delta);
void moveCameraRight(SUser* user, float delta);

// Rotations
void rotateModelXPos(SUser* user, float delta);
void rotateModelXNeg(SUser* user, float delta);
void rotateModelYPos(SUser* user, float delta);
void rotateModelYNeg(SUser* user, float delta);
void rotateModelZPos(SUser* user, float delta);
void rotateModelZNeg(SUser* user, float delta);

// Application control
void closeApplication(SUser* user, float);
