#pragma once

// STL headers
#include <set>
#include <string>
#include <chrono>

// Project headers
#include "math3d.h"
#include "types.h"
#include "objFile.h"

struct SUser
{
	SFilesMap    files;
	SVerticesVec vertices;

	
	bool  useTexture = false;
	SMat4* selectedRotationMatrix = nullptr;
	SMat4* selectedTranslationMatrix = nullptr;

	// Camera
	SVec3 cameraPosition = {0.0f, 0.0f, 3.0f};
	SVec3 cameraUp       = {0.0f, 1.0f, 0.0f};

	float cameraYaw   = -90.0f;
	float cameraPitch =  0.0f;

	// Light
	SColor lightColor    = {1.0f, 1.0f, 1.0f};
	SVec3  lightPosition = {0.0f, 0.0f, 0.0f};

	// Matrices
	SMat4 projectionMatrix = perspective(radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

	// Simple timer
	bool timerRunning = false;
	std::chrono::steady_clock::time_point timerStart;
	double timerElapsedSeconds = 0.0;

	void timerStartNow()
	{
		timerStart = std::chrono::steady_clock::now();
		timerRunning = true;
	}

	void timerStop()
	{
		if (!timerRunning) return;
		auto now = std::chrono::steady_clock::now();
		timerElapsedSeconds = std::chrono::duration<double>(now - timerStart).count();
		timerRunning = false;
	}

	double timerElapsed()
	{
		if (timerRunning) {
			auto now = std::chrono::steady_clock::now();
			return std::chrono::duration<double>(now - timerStart).count();
		}
		return timerElapsedSeconds;
	}
};
