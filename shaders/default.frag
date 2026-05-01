#version 330 core

in vec3 vColor;
in vec2 vTexCoord;

out vec4 FragColor;

uniform sampler2D uTexture;
uniform int uUseTexture;

uniform vec3  uKa;    // Ambient color
uniform vec3  uKd;    // Diffuse color
uniform vec3  uKs;    // Specular color
uniform float uNs;    // Shininess
uniform float uNi;    // Index of refraction
uniform float ud;     // Dissolve (opacity)
uniform int   uIllum; // Illumination model

void main()
{
    if (uUseTexture == 1)
        FragColor = texture(uTexture, vTexCoord);
    else
        FragColor = vec4(vColor, 1.0);
};
