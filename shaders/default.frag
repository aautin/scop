#version 330 core

// Input from vertex shader
in vec3 vPosition;
in vec3 vColor;
in vec2 vTexCoord;
in vec3 vNormal;

// Texture properties
uniform sampler2D uTexture;
uniform int uUseTexture;

// Transition properties (for bonus)
uniform float uTransition;

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

    //
    // Base color (from texture or vertex color)
    //
    vec4 baseColor = uUseTexture == 1 ? texture(uTexture, vTexCoord) : vec4(vColor, 1.0);

    //
    // Mix it with the other mode if transition is active (for bonus)
    //
    if (uTransition > 0.0 && uTransition < 1.0)
    {
        vec4 otherColor = uUseTexture == 1 ? vec4(vColor, 1.0) : texture(uTexture, vTexCoord);
        baseColor = mix(baseColor, otherColor, uTransition);
    }

    //
    // Apply dissolve (opacity)
    //
    baseColor.a *= uD;

    //
    // Ambient diffuse and specular calculations
    //
    vec3 ambient = uKa * uLightColor;

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = uKd * diff * uLightColor;

    vec3 specular = vec3(0.0);
    if (uNs > 0.0) { // Avoid pow(x, 0) issues
        vec3 halfVector = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfVector), 0.0), uNs);
        specular = uKs * spec * uLightColor;
    }

    //
    // Calculate final lighting based on the illumination model
    //
    vec3 lighting;
    switch (uIllum)
    {
        case 0:
        {
            lighting = vec3(1.0);
            break;
        }
        case 1:
        {
            lighting = ambient + diffuse;
            break;
        }
        default:
        {
            lighting = ambient + diffuse + specular;
            break;
        }
    }
    FragColor = vec4(baseColor.rgb * lighting, baseColor.a);
}