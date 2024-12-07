#version 410 core

// uniform inputs

// varying inputs
uniform sampler2D textureMap;
layout(location = 1) in vec2 textCoordinate;
layout(location = 0) in vec3 matColor;
uniform bool useTexture;



// lighting stuff
// uniforms
uniform vec3 lightDirection;
uniform vec3 lightColor;
// spotlight uniforms
uniform vec3 spotlightPos;
uniform vec3 spotlightDir;
uniform vec3 spotlightColor;
uniform float spotlightCutOff;
uniform float spotlightOuterCutOff;
// point light uniforms
uniform vec3 pointlightPos;
uniform vec3 pointlightColor;


layout(location = 2) in vec3 transNormalVector;
layout(location = 3) in vec3 viewVector;

layout(location = 4) in vec3 fspotDir;
layout(location = 5) in float spotlightDist;

layout(location = 6) in vec3 fpointDir;
layout(location = 7) in float pointlightDist;


// outputs
out vec4 fragColorOut;                  // color to apply to this fragment


void main() {
    // ******** DIRECTIONAL LIGHT ******** //
    
    // Compute light vector for directional light
    vec3 lightVector = normalize(-lightDirection);
    // Transform normal vector
    // Calculate reflection vector for specular lighting
    vec3 reflectionVector = reflect(-lightVector, transNormalVector);
    // Diffuse calculation for directional light
    vec3 diffuse = lightColor * max(dot(lightVector, transNormalVector), 0.0);
    // Ambient lighting for directional light
    vec3 ambient = lightColor * 0.15 ;
    // Specular lighting for directional light
    vec3 spectral = lightColor * pow(max(dot(viewVector, reflectionVector), 0.0), 16.0);
    // Combine directional light and spotlight contributions

    vec3 directLight = (diffuse + spectral + ambient) ;

    // *********** SPOTLIGHT ************* // 
    // Spotlight diffuse and specular calculations
    float spotDiff = max(dot(transNormalVector, fspotDir), 0.0);
    vec3 spotlightDiffuse = spotDiff * spotlightColor;

    vec3 spotReflectDir = reflect(-fspotDir, transNormalVector);
    float spotSpec = pow(max(dot(viewVector, spotReflectDir), 0.0), 32.0);
    vec3 spotlightSpecular = spotSpec * spotlightColor;
    // Spotlight intensity based on cutoff angles
    float theta = dot(fspotDir, normalize(-spotlightDir));
    float epsilon = spotlightCutOff - spotlightOuterCutOff;
    float intensity = clamp((theta - spotlightOuterCutOff) / epsilon, 0.0, 1.0);
    // Combine spotlight results with intensity
    vec3 spotLight = ((spotlightDiffuse + spotlightSpecular) * intensity) / (1 + (0.09*spotlightDist) + 0.032*pow((spotlightDist), 2));


    // ******** POINT LIGHT ******** //
    vec3 pointDiffuse = pointlightColor * max(dot(fpointDir, transNormalVector), 0.0);
    vec3 pointReflectionVector = reflect(-fpointDir, transNormalVector);
    vec3 pointSpectral = pointlightColor * pow(max(dot(viewVector, pointReflectionVector), 0.0), 16.0);
    vec3 pointLight = (pointDiffuse + pointSpectral) / (1 + 0.8*pointlightDist + 0.01*pointlightDist*pointlightDist);


    vec3 color = directLight + spotLight + pointLight;
    // if textured:
    if (useTexture) {
        vec4 texel = texture(textureMap, textCoordinate);
        fragColorOut = /*vec4(color, 1.0) **/ texel;
        
    } else {
        // pass the interpolated color through as output
        fragColorOut = vec4(color * matColor, 1.0);
    }
}