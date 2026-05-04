#version 330 core

// Input from vertex buffer (VAO)
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aNormal;

// MVP matrices to
uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

// Output to fragment shader
out vec3 vPosition;
out vec3 vColor;
out vec2 vTexCoord;
out vec3 vNormal;

void main()
{
	gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(aPos, 1.0);
	
	vPosition = vec3(uModelMatrix * vec4(aPos, 1.0));
	vColor = aColor;
	vTexCoord = aTexCoord;
	vNormal = mat3(transpose(inverse(uModelMatrix))) * aNormal;
}
