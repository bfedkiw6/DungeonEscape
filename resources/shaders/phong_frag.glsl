#version 410 core
// Note: OpenGL may warn about unbound textures in samplers.
// This is safe to ignore - we use texture_flags to conditionally sample.

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 camera_pos;

// Color
uniform vec3 ambient; // Ambient color/texture (Ka)
uniform vec3 diffuse; // Diffuse color/texture (Kd)
uniform vec3 specular; // Specular color/texture (Ks)


// Texture samplers
uniform sampler2D diffuse_tex;
uniform bool has_diffuse_tex;

uniform sampler2D specular_tex;
uniform bool has_specular_tex;

// Specular exponent
uniform float shininess;
uniform float opacity;

// Heightmapping
uniform sampler2D height_tex;
uniform bool has_height_tex;
uniform float height_scale;


// Light types - matching src/Light.h
const int LIGHT_TYPE_DIRECTIONAL = 1;
const int LIGHT_TYPE_POINT       = 2;
const int MAX_LIGHTS = 40;

// Light structure - matching src/Light.h
struct Light {
    int type;
    vec3 position;
    vec3 color;
    vec3 direction;
    vec3 attenuation; // constant, linear, quadratic
};

uniform Light[MAX_LIGHTS] lights;
uniform int num_lights;

uniform vec3 ambient_light;


vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    float height = texture(height_tex, texCoords).r;
    return texCoords - viewDir.xy * (height * height_scale);
}


void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(camera_pos - FragPos);

    // Get material colors
    vec3 materialAmbient = ambient;
    vec3 materialDiffuse = diffuse;
    vec3 materialSpecular = specular;

    // only sample textures if they're bound
    // if (has_diffuse_tex) {
    //    vec3 diffuseTex = texture(diffuse_tex, TexCoord).rgb;
    //    materialAmbient = diffuseTex * ambient;
    //    materialDiffuse = diffuse * diffuseTex;
    //}
    vec2 texCoords = TexCoord;

    // approximate view direction
    vec3 viewDir = normalize(camera_pos - FragPos);

    if (has_height_tex) {
          texCoords = ParallaxMapping(texCoords, viewDir);
    }

    texCoords = clamp(texCoords, 0.001, 0.999);

    if (has_diffuse_tex) {
          vec3 diffuseTex = texture(diffuse_tex, texCoords).rgb;
          materialAmbient = diffuseTex * ambient;
          materialDiffuse = diffuse * diffuseTex;
    }

    if (has_specular_tex) {
        materialSpecular = specular * texture(specular_tex, TexCoord).rgb;
    }

    // Start with ambient lighting
    //vec3 result = materialAmbient * ambient_light;
    vec3 result = materialDiffuse * ambient_light;

    // Add contribution from each light
    for (int i = 0; i < num_lights; i++) {
        Light light = lights[i];
        vec3 lightDir;
        float attenuation = 1.0;

        if (light.type == LIGHT_TYPE_DIRECTIONAL) {
            // Directional light - use direction directly
            lightDir = normalize(-light.direction);
        }
        else if (light.type == LIGHT_TYPE_POINT) {
            // Point light - calculate direction from position
            lightDir = normalize(light.position - FragPos);

            // Calculate attenuation
            float distance = length(light.position - FragPos);
            attenuation = 1.0 / (light.attenuation.x +
                                 light.attenuation.y * distance +
                                 light.attenuation.z * distance * distance);
        }
        else {
            continue; // Skip unknown light types
        }

        // Diffuse component
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuseContribution = light.color * diff * materialDiffuse * attenuation;

        // Specular component (Blinn-Phong)
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
        vec3 specularContribution = light.color * spec * materialSpecular * attenuation;

        result += diffuseContribution + specularContribution;
    }

    // Fog
    float dist = length(camera_pos - FragPos);
    float fogAmount = clamp(1.0 - exp(-dist * 0.025), 0.0, 0.3); // Exponential fog
    vec3 fogColor = vec3(0.06, 0.045, 0.09);
    result = mix(result, fogColor, fogAmount);
    // Volumetric Glow
    for (int i = 0; i < num_lights; i++) {
        if (lights[i].type == LIGHT_TYPE_POINT) {
            float light_dist = length(lights[i].position - FragPos);
            float glow = exp(-light_dist * 0.25); // Exponential glow
            result += lights[i].color * glow * 0.10;
        }
    }

    result = clamp(result, 0.0, 1.0);
    FragColor = vec4(result, opacity);
}

