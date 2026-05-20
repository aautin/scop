#include "glfwHandling.h"

// STL headers
#include <algorithm>
#include <functional>

// Project headers
#include "matrix.h"
#include "transformation.h"

void keyReleaseHandler(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	auto user = static_cast<SUser*>(glfwGetWindowUserPointer(window));

	const std::map<int, std::function<void()>> keyBindings = {
		{GLFW_KEY_M, [user](){ user->useTexture = !user->useTexture; }},
	};

	if (action == GLFW_RELEASE)
	{
		for (const auto& binding : keyBindings)
		{
			if (binding.first == key)
			{
				binding.second();
				break;
			}
		}
	}
}

void handlePressedKeys(GLFWwindow* window) {
	auto user = static_cast<SUser*>(glfwGetWindowUserPointer(window));
	
	using ActionFunction = std::function<void(SUser*, float)>;
    static const std::map<std::set<int>, std::pair<ActionFunction, float>> keyBindings = {
        {{GLFW_KEY_L, GLFW_KEY_DOWN},  {moveLightBackward, 0.1f}},
        {{GLFW_KEY_L, GLFW_KEY_UP},    {moveLightForward, 0.1f}},
        {{GLFW_KEY_L, GLFW_KEY_LEFT},  {moveLightLeft, 0.1f}},
        {{GLFW_KEY_L, GLFW_KEY_RIGHT}, {moveLightRight, 0.1f}},

        {{GLFW_KEY_S},                 {moveCameraBackward, 0.1f}},
        {{GLFW_KEY_W},                 {moveCameraForward, 0.1f}},
        {{GLFW_KEY_A},                 {moveCameraLeft, 0.1f}},
        {{GLFW_KEY_D},                 {moveCameraRight, 0.1f}},

        {{GLFW_KEY_X, GLFW_KEY_UP},    {rotateModelXPos, radians(5.0f)}},
        {{GLFW_KEY_X, GLFW_KEY_DOWN},  {rotateModelXNeg, radians(5.0f)}},
        {{GLFW_KEY_Y, GLFW_KEY_UP},    {rotateModelYPos, radians(5.0f)}},
        {{GLFW_KEY_Y, GLFW_KEY_DOWN},  {rotateModelYNeg, radians(5.0f)}},
        {{GLFW_KEY_Z, GLFW_KEY_UP},    {rotateModelZPos, radians(5.0f)}},
        {{GLFW_KEY_Z, GLFW_KEY_DOWN},  {rotateModelZNeg, radians(5.0f)}},

        {{GLFW_KEY_ESCAPE},           {closeApplication, 0.0f}},
    };

    for (const auto& binding : keyBindings)
	{
        const auto& keys     = binding.first;
        const auto& function = binding.second.first;
        const auto& arg      = binding.second.second;

        if (std::all_of(keys.begin(), keys.end(), [&](int key)
            { return glfwGetKey(window, key) == GLFW_PRESS; }))
		{
            for (int key = GLFW_KEY_1; key <= GLFW_KEY_9; ++key)
            {
                //
                // Select the object to rotate with the number keys
                //
                if (glfwGetKey(window, key) == GLFW_PRESS)
                {
                        user->selectedRotationMatrix = nullptr;
                        bool found = false;

                        //
                        // Browse in all the object and select the one corresponding to the pressed number key
                        //
                        size_t objectIndex = key - GLFW_KEY_1;
                        size_t currentIndex = 0;
                        for (auto& [fileName, file] : user->files)
                        {
                            for (auto& [objectName, object] : file.objects)
                            {
                                if (object.materialGroups.empty())
                                {
                                    continue;
                                }

                                if (currentIndex == objectIndex)
                                {
                                    user->selectedRotationMatrix = &object.rotation;
                                    found = true;
                                    break;
                                }
                                ++currentIndex;
                            }

                            if (found)
                            {
                                break;
                            }
                        }
                        break;
                }
            }
            function(user, arg);
        }
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    static std::optional<double> lastMouseX = std::nullopt;
    static std::optional<double> lastMouseY = std::nullopt;

    SUser* user = static_cast<SUser*>(glfwGetWindowUserPointer(window));

    if (!lastMouseX.has_value() || !lastMouseY.has_value()) {
        lastMouseX = xpos;
        lastMouseY = ypos;
        return;
    }

    double xoffset = xpos - lastMouseX.value();
    double yoffset = lastMouseY.value() - ypos; // Reversed to match screen coordinates

    lastMouseX = xpos;
    lastMouseY = ypos;

    // Update yaw and pitch with sensitivity
    float sensitivity = 0.1f;
    user->cameraYaw += static_cast<float>(xoffset) * sensitivity;
    user->cameraPitch += static_cast<float>(yoffset) * sensitivity; // Note: yoffset is reversed

    // Clamp pitch to avoid flipping
    if (user->cameraPitch > 89.0f)
        user->cameraPitch = 89.0f;
    if (user->cameraPitch < -89.0f)
        user->cameraPitch = -89.0f;

    // Optional: Print debug info
    // std::cout << "Yaw: " << user->cameraYaw << ", Pitch: " << user->cameraPitch << std::endl;
}

void errorCallback(int code, const char* description)
{
    std::cerr << "Error " << code << ": " << description << std::endl;
}

void compileShaderErrors(GLuint shader, const char* type)
{
	GLint hasCompiled;
	char infoLog[1024];
	
	glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
	if (hasCompiled == GL_FALSE)
	{
		glGetShaderInfoLog(shader, 1024, NULL, infoLog);
		std::cerr << "SHADER_COMPILATION_ERROR for:" << type << "\n" << infoLog << std::endl;
	}
}

void compileShaderProgramErrors(GLuint shaderProgram)
{
	GLint hasCompiled;
	char infoLog[1024];

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &hasCompiled);
	if (hasCompiled == GL_FALSE)
	{
		glGetProgramInfoLog(shaderProgram, 1024, NULL, infoLog);
		std::cerr << "SHADER_LINKING_ERROR for shader program\n" << infoLog << std::endl;
	}
}
