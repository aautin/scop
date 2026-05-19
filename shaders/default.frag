#version 330 core

// Input from vertex shader
in vec3 vPosition;
in vec3 vColor;
in vec2 vTexCoord;
in vec3 vNormal;

// Texture properties
uniform sampler2D uTexture;
uniform int uUseTexture;

// Material properties
uniform vec3  uKa;    // Ambient color
uniform vec3  uKd;    // Diffuse color
uniform vec3  uKs;    // Specular color
uniform float uNs;    // Shininess
uniform float uNi;    // Index of refraction (unused for illum 0-2)
uniform float uD;     // Dissolve (opacity)
uniform int   uIllum; // Illumination model

// Light properties
uniform vec3 uLightPosition;
uniform vec3 uLightColor;

// Camera position
uniform vec3 uCameraPosition;

// Final pixel
out vec4 FragColor;

void main() {
    vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(uLightPosition - vPosition);
    vec3 viewDir = normalize(uCameraPosition - vPosition);

    // Base color (from texture or vertex color)
    vec4 baseColor = uUseTexture == 1 ? texture(uTexture, vTexCoord) : vec4(vColor, 1.0);

    // Apply dissolve (opacity)
    baseColor.a *= uD;

    // Illumination model handling
    vec3 lighting;

    // Ambient light
    vec3 ambient = uKa * uLightColor;

    // Diffuse light
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = uKd * diff * uLightColor;

    // Specular light
    vec3 specular = vec3(0.0);
    if (uNs > 0.0) { // Avoid pow(x, 0) issues
        vec3 halfVector = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfVector), 0.0), uNs);
        specular = uKs * spec * uLightColor;
    }
    
    switch (uIllum)
    {
        case 0: // No lighting (color only)
        {
            lighting = vec3(1.0);
            break;
        }
        case 1: // Diffuse only (no specular)
        {
            lighting = ambient + diffuse;
            break;
        }
        default: // Phong shading for value 2 and unsupported illum values
        {
            lighting = ambient + diffuse + specular;
            break;
        }
    }
    FragColor = vec4(baseColor.rgb * lighting, baseColor.a);
}