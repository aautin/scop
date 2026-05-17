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
uniform float uNi;    // Index of refraction
uniform float uD;     // Dissolve (opacity)
uniform int   uIllum; // Illumination model

// Light properties
uniform vec3 uLightPosition;
uniform vec3 uLightColor;

// Camera position
uniform vec3 uCameraPosition;

// Final pixel
out vec4 FragColor;

void main()
{
    vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(uLightPosition - vPosition);
    vec3 viewDir = normalize(uCameraPosition - vPosition);

    //
    // Ambient = Ambient color * Light color
    //
    vec3 ambient = uKa * uLightColor;
    vec3 diffuse = uKd * max(dot(normal, lightDir), 0.0) * uLightColor;
    vec3 specular = uKs * pow(max(dot(normal, normalize(lightDir + viewDir)), 0.0), uNs) * uLightColor;

    //
    // Base color and lighting calculations
    //
    vec4 baseColor = uUseTexture == 1 ? texture(uTexture, vTexCoord) : vec4(vColor, 1.0);
    vec3 lighting = ambient + diffuse + specular;

    FragColor = vec4(baseColor.rgb * lighting, baseColor.a * uD);
};
