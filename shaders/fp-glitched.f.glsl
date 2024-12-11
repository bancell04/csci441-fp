#version 410 core

// Uniform inputs
uniform sampler2D textureMap;
uniform bool useTexture;
uniform float time;     
uniform bool useLight;


// Lighting uniforms
uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform vec3 spotlightPos;
uniform vec3 spotlightColor;
uniform float spotlightCutOff;
uniform float spotlightOuterCutOff;
uniform vec3 spotlightDir;

// Varying inputs
layout(location = 1) in vec2 textCoordinate;
layout(location = 0) in vec3 matColor;

layout(location = 2) in vec3 transNormalVector;
layout(location = 3) in vec3 viewVector;

layout(location = 4) in vec3 fspotDir;
layout(location = 5) in float spotlightDist;

layout(location = 6) in vec4 fragPosition;

// Outputs
out vec4 fragColorOut;              // Color to apply to this fragment

// Random function for glitch effect
float random(vec2 pos) {
    return fract(sin(dot(pos.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

void main() {
    // ******** DIRECTIONAL LIGHT ******** //
    vec3 lightVector = normalize(-lightDirection);
    vec3 reflectionVector = reflect(-lightVector, transNormalVector);
    vec3 diffuse = lightColor * max(dot(lightVector, transNormalVector), 0.0);
    vec3 ambient = lightColor * 0.15;
    vec3 spectral = lightColor * pow(max(dot(viewVector, reflectionVector), 0.0), 16.0);
    vec3 directLight = diffuse + spectral + ambient;

    // *********** SPOTLIGHT ************* // 
    float spotDiff = max(dot(transNormalVector, fspotDir), 0.0);
    vec3 spotlightDiffuse = spotDiff * spotlightColor;
    vec3 spotReflectDir = reflect(-fspotDir, transNormalVector);
    float spotSpec = pow(max(dot(viewVector, spotReflectDir), 0.0), 32.0);
    vec3 spotlightSpecular = spotSpec * spotlightColor;
    float theta = dot(normalize(fspotDir), normalize(-spotlightDir));
    float epsilon = spotlightCutOff - spotlightOuterCutOff;
    float intensity = clamp((theta - spotlightOuterCutOff) / epsilon, 0.0, 1.0);
    vec3 spotLight = ((spotlightDiffuse + spotlightSpecular) * intensity) / 
                     (1 + (0.09 * spotlightDist) + 0.032 * pow((spotlightDist), 2));


    vec3 color = directLight + spotLight;



    if (useTexture) {
        // Sample the texture without altering the coordinates
        vec4 texel = texture(textureMap, textCoordinate);

        // Add a random noise to the color channels
        float glitchNoiseR = texel.x + random(vec2(textCoordinate.x, time)) * 0.1; // Random shift for red
        float glitchNoiseG = texel.y + random(vec2(textCoordinate.y, time)) * 0.1; // Random shift for green
        float glitchNoiseB = texel.z + random(vec2(textCoordinate.x + textCoordinate.y, time)) * 0.1; // Random shift for blue

        // Adjust the texture color with the glitch noise
        vec3 glitchedColor = vec3(glitchNoiseR, glitchNoiseG, glitchNoiseB);

        // Combine with material color and output
        fragColorOut = vec4(glitchedColor, texel.a);
    } else {
        // Apply random color shift to non-textured objects
        float glitchNoiseR = random(vec2(fragPosition.x, time)) * 0.1; // Use object position and time
        float glitchNoiseG = random(vec2(fragPosition.y, time)) * 0.1;
        float glitchNoiseB = random(vec2(fragPosition.z, time)) * 0.1;

        vec3 glitchedColor = matColor.rgb + vec3(glitchNoiseR, glitchNoiseG, glitchNoiseB);

        // Output altered color for non-textured objects
        if (useLight) {
            fragColorOut = vec4(color * glitchedColor, 1.0);
        } else {
            fragColorOut = vec4(glitchedColor, 1.0);
        }
    }

}
