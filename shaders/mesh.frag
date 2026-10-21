#version 460 core

in vec3 v_fragPos;
in vec3 v_normal;
in vec2 v_uv;

out vec4 o_color;

// Render mode:
// 0 = Blinn-Phong lighting
// 1 = Normal visualizer
// 2 = Unlit base color / UV debug
uniform int u_renderMode;

// Lighting uniforms
uniform vec3 u_lightPos;
uniform vec3 u_lightColor;
uniform vec3 u_viewPos;
uniform vec3 u_objectColor;
uniform float u_ambientStrength;
uniform float u_specularStrength;
uniform float u_shininess;

void main()
{
    vec3 norm = normalize(v_normal);

    if (u_renderMode == 1) {
        // Map normal [-1, 1] to [0, 1] RGB
        vec3 normalColor = norm * 0.5 + 0.5;
        o_color = vec4(normalColor, 1.0);
        return;
    }

    if (u_renderMode == 2) {
        // Unlit flat color
        o_color = vec4(u_objectColor, 1.0);
        return;
    }

    // Default: Blinn-Phong Lighting
    // Ambient
    vec3 ambient = u_ambientStrength * u_lightColor;

    // Diffuse
    vec3 lightDir = normalize(u_lightPos - v_fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * u_lightColor;

    // Specular (Blinn-Phong halfway vector)
    vec3 viewDir = normalize(u_viewPos - v_fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), u_shininess);
    vec3 specular = u_specularStrength * spec * u_lightColor;

    vec3 result = (ambient + diffuse + specular) * u_objectColor;
    o_color = vec4(result, 1.0);
}
