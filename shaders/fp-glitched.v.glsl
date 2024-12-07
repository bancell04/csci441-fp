#version 410 core

// Uniform inputs
uniform mat4 mvpMatrix;                 // Precomputed Model-View-Projection Matrix
uniform mat3 normalMatrix;              // Normal matrix for transforming normals
uniform vec3 cameraPos;
uniform vec3 materialColor;             // Material color for the object

// Directional light uniforms
// Spotlight uniforms
uniform vec3 spotlightPos;
// Point light uniforms
uniform vec3 pointlightPos;

// Glitch effect uniform
uniform float time;                     // Time for animated effects

// Attribute inputs
layout(location = 0) in vec3 vPos;      // Position of vertex in object space
layout(location = 2) in vec2 textCoord;
in vec3 vNormal;                        // Vertex normal

// Varying outputs
layout(location = 0) out vec3 matColor;
layout(location = 1) out vec2 textCoordinate;

layout(location = 2) out vec3 transNormalVector;
layout(location = 3) out vec3 viewVector;

layout(location = 4) out vec3 fspotDir;
layout(location = 5) out float spotlightDist;

layout(location = 6) out vec3 fpointDir;
layout(location = 7) out float pointlightDist;
layout(location = 8) out vec4 fragPosition;

// Pseudo-random displacement function
float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

// Random displacement for glitch effect
vec3 glitchDisplacement(vec3 position) {
    float displacementAmount = 0.05; 
    float noiseX = random(position.yz + vec2(time)) * displacementAmount;
    float noiseY = random(position.xz - vec2(time)) * displacementAmount;
    float noiseZ = random(position.xy + vec2(time * 0.5)) * displacementAmount;
    return position + vec3(noiseX, noiseY, noiseZ);
}

void main() {
    // Apply random displacement for glitch effect
    vec3 glitchedPos = glitchDisplacement(vPos);

    // Transform & output the vertex in clip space
    gl_Position = mvpMatrix * vec4(glitchedPos, 1.0);
    fragPosition = mvpMatrix * vec4(vPos, 1.0);
    // Normal transformations
    transNormalVector = normalize(normalMatrix * vNormal);
    viewVector = normalize(cameraPos - glitchedPos);

    // Spotlight transformations
    fspotDir = normalize(spotlightPos - glitchedPos);
    spotlightDist = distance(glitchedPos, spotlightPos);

    // Point light transformations
    fpointDir = normalize(pointlightPos - glitchedPos);
    pointlightDist = distance(glitchedPos, pointlightPos);

    // Material color and texture coordinates
    matColor = materialColor;
    textCoordinate = textCoord;
}
