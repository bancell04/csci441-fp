#version 410 core

// uniform inputs
uniform mat4 mvpMatrix;                 // precomputed Model-View-Projection Matrix
uniform mat4 modelViewMtx;
uniform mat3 normalMatrix;              // normal matrix for transforming normals
uniform vec3 cameraPos;
uniform vec3 materialColor;             // material color for the object
// direction light uniforms
// spotlight uniforms
uniform vec3 spotlightPos;

// attribute inputs
layout(location = 0) in vec3 vPos;      // position of vertex in object space
layout(location = 2) in vec2 textCoord;
in vec3 vNormal;                        // vertex normal
// varying outputs
layout(location = 0) out vec3 matColor;
layout(location = 1) out vec2 textCoordinate;

layout(location = 2) out vec3 transNormalVector;
layout(location = 3) out vec3 viewVector;

layout(location = 4) out vec3 fspotDir;
layout(location = 5) out float spotlightDist;


void main() {
    // transform & output the vertex in clip space
    gl_Position = mvpMatrix * vec4(vPos, 1.0);

    transNormalVector = normalize(normalMatrix * vNormal);
    viewVector = normalize(cameraPos - vPos);

    vec3 viewPos = vec3(modelViewMtx * vec4(vPos, 1.0));
    vec3 spotlightViewPos = vec3(modelViewMtx * vec4(spotlightPos, 1.0));

    fspotDir = normalize(spotlightViewPos - viewPos);
    spotlightDist = distance(viewPos, spotlightViewPos);

    matColor = materialColor;
    // Pass texture coordinate to fragment shader
    textCoordinate = textCoord;
}
