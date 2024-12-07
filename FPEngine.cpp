#include "FPEngine.h"

#include <CSCI441/objects.hpp>

#include <glm/gtc/type_ptr.hpp>  // for glm::value_ptr()

#include <ctime>
#include <iostream>
#include <stb_image.h>

//*************************************************************************************
//
// Helper Functions

#ifndef M_PI
#define M_PI 3.14159265f
#endif

/// \desc Simple helper function to return a random number between 0.0f and 1.0f.
GLfloat getRand()
{
    return (GLfloat)rand() / (GLfloat)RAND_MAX;
}

//*************************************************************************************
//
// Public Interface

FPEngine::FPEngine()
    : CSCI441::OpenGLEngine(4, 1,
                            640, 480,
                            "FP - 8 Flags")
{
    for (auto& _key : _keys) _key = GL_FALSE;

    _mousePosition = glm::vec2(MOUSE_UNINITIALIZED, MOUSE_UNINITIALIZED);
    _leftMouseButtonState = GLFW_RELEASE;
}

FPEngine::~FPEngine()
{
    delete _pArcballCam;
}

void FPEngine::handleKeyEvent(GLint key, GLint action)
{
    if (key != GLFW_KEY_UNKNOWN)
        if (key == GLFW_KEY_COMMA || key == GLFW_KEY_PERIOD || key == GLFW_KEY_SPACE || key == GLFW_KEY_F)
        {
            _keys[key] = (action == GLFW_PRESS);
        }
        else
        {
            _keys[key] = ((action == GLFW_PRESS) || (action == GLFW_REPEAT));
        }

    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        // quit!
        case GLFW_KEY_Q:
        case GLFW_KEY_ESCAPE:
            setWindowShouldClose();
            break;

        default: break; // suppress CLion warning
        }
    }
}

void FPEngine::handleMouseButtonEvent(GLint button, GLint action)
{
    // if the event is for the left mouse button
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        // update the left mouse button's state
        _leftMouseButtonState = action;
    }
}

void FPEngine::handleCursorPositionEvent(glm::vec2 currMousePosition)
{
    // if mouse hasn't moved in the window, prevent camera from flipping out
    if (_mousePosition.x == MOUSE_UNINITIALIZED)
    {
        _mousePosition = currMousePosition;
    }

    if (_leftMouseButtonState == GLFW_PRESS)
    {
        if (cameraIndex == 0)
        {
            // ARCBALL
            if (_keys[GLFW_KEY_LEFT_SHIFT])
            {
                float yChange = (_mousePosition.y - currMousePosition.y) * 0.005f;
                // zoom in and out
                if (yChange > 0)
                {
                    _pArcballCam->moveForward(_cameraSpeed.x);
                }
                else
                {
                    _pArcballCam->moveBackward(_cameraSpeed.x);
                }
            }
            else
            {
                GLfloat dTheta = ((GLfloat)currMousePosition.x - (GLfloat)_mousePosition.x) * .005f; // TODO #5
                GLfloat dPhi = ((GLfloat)currMousePosition.y - (GLfloat)_mousePosition.y) * .005f; // TODO #6
                // rotate the camera by the distance the mouse moved
                _pArcballCam->rotate(dTheta, dPhi);
                // std::cout << _pArcballCam->getPhi() << std::endl;
                // std::cout << _pArcballCam->getTheta() << std::endl;
            }
        }
        else
        {
            // FREECAM
            GLfloat dTheta = ((GLfloat)currMousePosition.x - (GLfloat)_mousePosition.x) * .005f; // TODO #5
            GLfloat dPhi = ((GLfloat)currMousePosition.y - (GLfloat)_mousePosition.y) * .005f; // TODO #6
            // rotate the camera by the distance the mouse moved
            _pFreeCam->rotate(dTheta, -dPhi);
        }
    }


    // update the mouse position
    _mousePosition = currMousePosition;
}

//*************************************************************************************
//
// Engine Setup

void FPEngine::mSetupGLFW()
{
    CSCI441::OpenGLEngine::mSetupGLFW();

    // set our callbacks
    glfwSetKeyCallback(mpWindow, a3_engine_keyboard_callback);
    glfwSetMouseButtonCallback(mpWindow, a3_engine_mouse_button_callback);
    glfwSetCursorPosCallback(mpWindow, a3_engine_cursor_callback);
}

void FPEngine::mSetupOpenGL()
{
    glEnable(GL_DEPTH_TEST); // enable depth testing
    glDepthFunc(GL_LESS); // use less than depth test

    glEnable(GL_BLEND); // enable blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // use one minus blending equation

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // clear the frame buffer to black
}

void FPEngine::mSetupShaders()
{

    _regularShaderProgram = new CSCI441::ShaderProgram("shaders/fp-std.v.glsl", "shaders/fp-std.f.glsl");
    // query uniform locations
    _regularShaderUniformLocations.mvpMatrix = _regularShaderProgram->getUniformLocation("mvpMatrix");
    _regularShaderUniformLocations.time = _regularShaderProgram->getUniformLocation("time");
    // TODO #12A - texture map
    _regularShaderUniformLocations.useTexture = _regularShaderProgram->getUniformLocation("useTexture");
    _regularShaderUniformLocations.materialColor = _regularShaderProgram->getUniformLocation("materialColor");
    _regularShaderUniformLocations.normalMatrix = _regularShaderProgram->getUniformLocation("normalMatrix");
    _regularShaderUniformLocations.cameraPos = _regularShaderProgram->getUniformLocation("cameraPos");
    // LIGHT
    // directional
    _regularShaderUniformLocations.lightDirection = _regularShaderProgram->getUniformLocation("lightDirection");
    _regularShaderUniformLocations.lightColor = _regularShaderProgram->getUniformLocation("lightColor");
    // spotlight
    _regularShaderUniformLocations.spotlightDir = _regularShaderProgram->getUniformLocation("spotlightDir");
    _regularShaderUniformLocations.spotlightPos = _regularShaderProgram->getUniformLocation("spotlightPos");
    _regularShaderUniformLocations.spotlightColor = _regularShaderProgram->getUniformLocation("spotlightColor");
    _regularShaderUniformLocations.spotlightOuterCutOff = _regularShaderProgram->getUniformLocation("spotlightOuterCutOff");
    _regularShaderUniformLocations.spotlightCutOff = _regularShaderProgram->getUniformLocation("spotlightCutOff");
    
    // point light
    _regularShaderUniformLocations.pointlightPos = _regularShaderProgram->getUniformLocation("pointlightPos");
    _regularShaderUniformLocations.pointlightColor = _regularShaderProgram->getUniformLocation("pointlightColor");
    // query attribute locations
    _regularShaderAttributeLocations.vPos = _regularShaderProgram->getAttributeLocation("vPos");
    _regularShaderAttributeLocations.vNormal = _regularShaderProgram->getAttributeLocation("vNormal");
    // TODO #12B - texture coordinate
    _regularShaderAttributeLocations.texCoord = _regularShaderProgram->getAttributeLocation("textCoord");
    // set static uniforms
    // TODO #13 - set uniform
    _regularShaderProgram->setProgramUniform("textureMap", 0);


    // hook up the CSCI441 object library to our shader program - MUST be done after the shader is used and before the objects are drawn
    // if we have multiple shaders the flow would be:
    //      1) shader->useProgram()
    //      2) CSCI441::setVertexAttributeLocations()
    //      3) CSCI441::draw*()
    // but this lab only has one shader program ever in use, so we are safe to assign these values just once
    //
    // OR the alternative is to ensure that all of your shader programs use the
    // same attribute locations for the vertex position, normal, and texture coordinate
    // TODO #17 - set attribute
    CSCI441::setVertexAttributeLocations(_regularShaderAttributeLocations.vPos,
                                         _regularShaderAttributeLocations.vNormal,
                                         _regularShaderAttributeLocations.texCoord);



    /* ######## GLITCHED SHADER ######## */
    _glitchedShaderProgram = new CSCI441::ShaderProgram("shaders/fp-glitched.v.glsl", "shaders/fp-glitched.f.glsl");
    // query uniform locations
    _glitchedShaderUniformLocations.mvpMatrix = _glitchedShaderProgram->getUniformLocation("mvpMatrix");
    _glitchedShaderUniformLocations.time = _glitchedShaderProgram->getUniformLocation("time");
    // TODO #12A - texture map
    _glitchedShaderUniformLocations.useTexture = _glitchedShaderProgram->getUniformLocation("useTexture");
    _glitchedShaderUniformLocations.materialColor = _glitchedShaderProgram->getUniformLocation("materialColor");
    _glitchedShaderUniformLocations.normalMatrix = _glitchedShaderProgram->getUniformLocation("normalMatrix");
    _glitchedShaderUniformLocations.cameraPos = _glitchedShaderProgram->getUniformLocation("cameraPos");
    // LIGHT
    // directional
    _glitchedShaderUniformLocations.lightDirection = _glitchedShaderProgram->getUniformLocation("lightDirection");
    _glitchedShaderUniformLocations.lightColor = _glitchedShaderProgram->getUniformLocation("lightColor");
    // spotlight
    _glitchedShaderUniformLocations.spotlightDir = _glitchedShaderProgram->getUniformLocation("spotlightDir");
    _glitchedShaderUniformLocations.spotlightPos = _glitchedShaderProgram->getUniformLocation("spotlightPos");
    _glitchedShaderUniformLocations.spotlightColor = _glitchedShaderProgram->getUniformLocation("spotlightColor");
    _glitchedShaderUniformLocations.spotlightOuterCutOff = _glitchedShaderProgram->getUniformLocation("spotlightOuterCutOff");
    _glitchedShaderUniformLocations.spotlightCutOff = _glitchedShaderProgram->getUniformLocation("spotlightCutOff");
    
    // point light
    _glitchedShaderUniformLocations.pointlightPos = _glitchedShaderProgram->getUniformLocation("pointlightPos");
    _glitchedShaderUniformLocations.pointlightColor = _glitchedShaderProgram->getUniformLocation("pointlightColor");
    // query attribute locations
    _glitchedShaderAttributeLocations.vPos = _glitchedShaderProgram->getAttributeLocation("vPos");
    _glitchedShaderAttributeLocations.vNormal = _glitchedShaderProgram->getAttributeLocation("vNormal");
    // TODO #12B - texture coordinate
    _glitchedShaderAttributeLocations.texCoord = _glitchedShaderProgram->getAttributeLocation("textCoord");
    // set static uniforms
    // TODO #13 - set uniform
    _glitchedShaderProgram->setProgramUniform("textureMap", 0);


    // hook up the CSCI441 object library to our shader program - MUST be done after the shader is used and before the objects are drawn
    // if we have multiple shaders the flow would be:
    //      1) shader->useProgram()
    //      2) CSCI441::setVertexAttributeLocations()
    //      3) CSCI441::draw*()
    // but this lab only has one shader program ever in use, so we are safe to assign these values just once
    //
    // OR the alternative is to ensure that all of your shader programs use the
    // same attribute locations for the vertex position, normal, and texture coordinate
    // TODO #17 - set attribute
    CSCI441::setVertexAttributeLocations(_glitchedShaderAttributeLocations.vPos,
                                         _glitchedShaderAttributeLocations.vNormal,
                                         _glitchedShaderAttributeLocations.texCoord);

    
    shaderIndex = 0;


    _shaderPrograms[0] = _regularShaderProgram;
    _shaderPrograms[1] = _glitchedShaderProgram;
}

void FPEngine::mSetupBuffers()
{
    _createGroundBuffers();
    _generateEnvironment();

}

void FPEngine::_createGroundBuffers()
{
    // TODO #8: expand our struct
    struct Vertex
    {
        glm::vec3 position;
        float xNorm, yNorm, zNorm;
    };

    // TODO #9: add normal data
    Vertex groundQuad[4] = {
        {{-1.0f, 0.0f, -1.0f}, 0, 1, 0},
        {{1.0f, 0.0f, -1.0f}, 0, 1, 0},
        {{-1.0f, 0.0f, 1.0f}, 0, 1, 0},
        {{1.0f, 0.0f, 1.0f}, 0, 1, 0}
    };


    GLushort indices[4] = {0, 1, 2, 3};

    _numGroundPoints = 4;

    glGenVertexArrays(1, &_groundVAO);
    glBindVertexArray(_groundVAO);

    GLuint vbods[2]; // 0 - VBO, 1 - IBO
    glGenBuffers(2, vbods);
    glBindBuffer(GL_ARRAY_BUFFER, vbods[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundQuad), groundQuad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(_shaderAttributeLocations[shaderIndex]->vPos);
    glVertexAttribPointer(_shaderAttributeLocations[shaderIndex]->vPos, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)nullptr);

    glEnableVertexAttribArray(_shaderAttributeLocations[shaderIndex]->vNormal);
    glVertexAttribPointer(_shaderAttributeLocations[shaderIndex]->vNormal, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void*)nullptr);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbods[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void FPEngine::_createSkyBox()
{
    // // TODO #8: expand our struct
    struct VertexNormalTextured
    {
        glm::vec3 position;
        glm::vec3 normal;
        // TODO #14 - add texture coordinate
        glm::vec2 texCoord;
    };

    VertexNormalTextured skyboxWall[4]{
        {{-100.0f, -100.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}}, // bottom left
        {{100.0f, -100.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}}, // bottom right
        {{-100.0f, 100.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}}, // top left
        {{100.0f, 100.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}} // top right
    };


    GLushort indices[4] = {0, 1, 2, 3};
    _numGroundPoints = 4;

    glGenVertexArrays(1, &_groundVAO);
    glBindVertexArray(_groundVAO);


    GLuint vbods[2]; // 0 - VBO, 1 - IBO
    glGenBuffers(2, vbods);
    glBindBuffer(GL_ARRAY_BUFFER, vbods[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxWall), skyboxWall, GL_STATIC_DRAW);
    glEnableVertexAttribArray(_shaderAttributeLocations[shaderIndex]->vPos);
    glVertexAttribPointer(_shaderAttributeLocations[shaderIndex]->vPos, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormalTextured),
                          (void*)nullptr);

    glEnableVertexAttribArray(_shaderAttributeLocations[shaderIndex]->vNormal);
    glVertexAttribPointer(_shaderAttributeLocations[shaderIndex]->vNormal, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormalTextured),
                          (void*)(sizeof(glm::vec3)));

    glEnableVertexAttribArray(_shaderAttributeLocations[shaderIndex]->texCoord);
    glVertexAttribPointer(_shaderAttributeLocations[shaderIndex]->texCoord, 2, GL_FLOAT, GL_FALSE,
                          sizeof(VertexNormalTextured), (void*)(2 * sizeof(glm::vec3)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbods[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}


GLuint FPEngine::_loadAndRegisterTexture(const char* FILENAME)
{
    // our handle to the GPU
    GLuint textureHandle = 0;

    // enable setting to prevent image from being upside down
    stbi_set_flip_vertically_on_load(true);

    // will hold image parameters after load
    GLint imageWidth, imageHeight, imageChannels;
    // load image from file
    GLubyte* data = stbi_load(FILENAME, &imageWidth, &imageHeight, &imageChannels, 0);

    // if data was read from file
    if (data)
    {
        const GLint STORAGE_TYPE = (imageChannels == 4 ? GL_RGBA : GL_RGB);

        // TODO #01 - generate a texture handle
        glGenTextures(1, &textureHandle);

        // TODO #02 - bind it to be active
        glBindTexture(GL_TEXTURE_2D, textureHandle);

        // set texture parameters
        // TODO #03 - mag filter
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // TODO #04 - min filter
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        // TODO #05 - wrap s
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

        // TODO #06 - wrap t
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // TODO #07 - transfer image data to the GPU
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            STORAGE_TYPE,
            imageWidth,
            imageHeight,
            0,
            STORAGE_TYPE,
            GL_UNSIGNED_BYTE,
            data
        );


        fprintf(stdout, "[INFO]: %s texture map read in with handle %d\n", FILENAME, textureHandle);

        // release image memory from CPU - it now lives on the GPU
        stbi_image_free(data);
    }
    else
    {
        // load failed
        fprintf(stderr, "[ERROR]: Could not load texture map \"%s\"\n", FILENAME);
    }

    // return generated texture handle
    return textureHandle;
}

void FPEngine::mSetupTextures()
{
    // TODO #09 - load textures
    _texHandles[TEXTURE_ID::SKYBOX] = _loadAndRegisterTexture("assets/textures/space.jpg");
}

void FPEngine::_generateEnvironment()
{
    //******************************************************************
    // parameters to make up our grid size and spacing, feel free to
    // play around with this
    const GLfloat GRID_WIDTH = WORLD_SIZE * 1.8f;
    const GLfloat GRID_LENGTH = WORLD_SIZE * 1.8f;
    const GLfloat GRID_SPACING_WIDTH = 1.0f;
    const GLfloat GRID_SPACING_LENGTH = 1.0f;
    // precomputed parameters based on above
    const GLfloat LEFT_END_POINT = -GRID_WIDTH / 2.0f - 5.0f;
    const GLfloat RIGHT_END_POINT = GRID_WIDTH / 2.0f + 5.0f;
    const GLfloat BOTTOM_END_POINT = -GRID_LENGTH / 2.0f - 5.0f;
    const GLfloat TOP_END_POINT = GRID_LENGTH / 2.0f + 5.0f;
    //******************************************************************

}

void FPEngine::mSetupScene()
{

    _pArcballCam = new CSCI441::ArcballCam();
    _pArcballCam->setLookAtPoint(glm::vec3(10, 10, 10));
    _pArcballCam->setTheta(0);
    _pArcballCam->setPhi(-M_PI / 1.8f);
    _pArcballCam->recomputeOrientation();
    _cameraSpeed = glm::vec2(0.1f, 0.05f);
    cameras[0] = _pArcballCam;

    _pMapCam = new CSCI441::FreeCam();
    _pMapCam->setPosition(glm::vec3(10.0f, 2.0f, 3.0f)); // give the camera a scenic starting point
    _pMapCam->setTheta(-M_PI / 3.0f); // and a nice view
    _pMapCam->setPhi(M_PI / 2);
    _pMapCam->recomputeOrientation();
    _pMapCam->recomputeOrientation();

    _pFreeCam = new CSCI441::FreeCam();
    _pFreeCam->setPosition(glm::vec3(10.0f, 5.0f, 3.0f)); // give the camera a scenic starting point
    _pFreeCam->setTheta(-M_PI / 3.0f); // and a nice view
    _pFreeCam->setPhi(M_PI / 2.8f);
    _pFreeCam->recomputeOrientation();
    cameras[1] = _pFreeCam;



    // Directional Light
    glm::vec3 lightDirection = glm::vec3(-1, -1, -1);
    glm::vec3 lightColor = glm::vec3(1, 1, 1);


    for (int i = 0; i <= 1; i++) {
        glProgramUniform3fv(
            _shaderPrograms[i]->getShaderProgramHandle(),
            _shaderUniformLocations[i]->lightColor,
            1,
            glm::value_ptr(lightColor)
        );


        glProgramUniform3fv(
            _shaderPrograms[i]->getShaderProgramHandle(),
            _shaderUniformLocations[i]->lightDirection,
            1,
            glm::value_ptr(lightDirection)
        );


        // Point light
        glProgramUniform3fv(
            _shaderPrograms[i]->getShaderProgramHandle(),
            _shaderUniformLocations[i]->pointlightColor,
            1,
            glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f))
        );

        glProgramUniform3fv(
            _shaderPrograms[i]->getShaderProgramHandle(),
            _shaderUniformLocations[i]->pointlightPos,
            1,
            glm::value_ptr(glm::vec3(1.0f, 2.0f, 1.0f))
        );

        //spotlight
        glProgramUniform3fv(
            _shaderPrograms[i]->getShaderProgramHandle(),
            _shaderUniformLocations[i]->spotlightPos,
            1,
            glm::value_ptr(glm::vec3(50.0f, 2.0f, 50.0f))
        );
        glProgramUniform3fv(
            _shaderPrograms[i]->getShaderProgramHandle(),
            _shaderUniformLocations[i]->spotlightDir,
            1,
            glm::value_ptr(glm::vec3(0.0f, -1, 0.0f))
        );
        glProgramUniform3fv(
            _shaderPrograms[i]->getShaderProgramHandle(),
            _shaderUniformLocations[i]->spotlightColor,
            1,
            glm::value_ptr(glm::vec3(0.5f, 0.0f, 0.5f))
        );
        float innerCutoffAngle = 10.0f; // inner cutoff in degrees
        float outerCutoffAngle = 15.0f; // outer cutoff in degrees
        glUniform1f(_shaderUniformLocations[i]->spotlightCutOff, cos(glm::radians(innerCutoffAngle)));
        glUniform1f(_shaderUniformLocations[i]->spotlightOuterCutOff, cos(glm::radians(outerCutoffAngle)));
    }
}

//*************************************************************************************
//
// Engine Cleanup

void FPEngine::mCleanupShaders()
{
    fprintf(stdout, "[INFO]: ...deleting Shaders.\n");
    delete _regularShaderProgram;
    delete _glitchedShaderProgram;
}

void FPEngine::mCleanupBuffers()
{
    fprintf(stdout, "[INFO]: ...deleting VAOs....\n");
    CSCI441::deleteObjectVAOs();
    glDeleteVertexArrays(1, &_groundVAO);

    fprintf(stdout, "[INFO]: ...deleting VBOs....\n");
    CSCI441::deleteObjectVBOs();

    fprintf(stdout, "[INFO]: ...deleting models..\n");

}


//*************************************************************************************
//
// Rendering / Drawing Functions - this is where the magic happens!

void FPEngine::_renderScene(glm::mat4 viewMtx, glm::mat4 projMtx) const
{
    // use our texture shader program
    _shaderPrograms[shaderIndex]->useProgram();

    _shaderPrograms[shaderIndex]->setProgramUniform(_shaderUniformLocations[shaderIndex]->time, static_cast<float>(glfwGetTime()));

    _shaderPrograms[shaderIndex]->setProgramUniform(_shaderUniformLocations[shaderIndex]->useTexture, 1); // Use texture for skybox
    glBindTexture(GL_TEXTURE_2D, _texHandles[TEXTURE_ID::SKYBOX]);
    glm::mat4 modelMtx = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 10.0f));
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    _shaderPrograms[shaderIndex]->setProgramUniform(_shaderUniformLocations[shaderIndex]->materialColor, glm::vec3(1.0f, 0.0f, 0.0f));

    CSCI441::drawSolidCubeTextured(100);


    _shaderPrograms[shaderIndex]->setProgramUniform(_shaderUniformLocations[shaderIndex]->useTexture, 0); // Don't use texture
    //// BEGIN DRAWING THE GROUND PLANE ////
    // draw the ground plane
    glm::mat4 groundModelMtx = glm::scale(glm::mat4(1.0f), glm::vec3(WORLD_SIZE, 1.0f, WORLD_SIZE));
    _computeAndSendMatrixUniforms(groundModelMtx, viewMtx, projMtx);

    glm::vec3 groundColor(0.3f, 0.8f, 0.2f);
    _shaderPrograms[shaderIndex]->setProgramUniform(_shaderUniformLocations[shaderIndex]->materialColor, groundColor);

    glm::vec3 cameraPosition = cameras[cameraIndex]->getPosition();
    glUniform3fv(_shaderUniformLocations[shaderIndex]->cameraPos, 1, glm::value_ptr(cameraPosition));

    glBindVertexArray(_groundVAO);
    glDrawElements(GL_TRIANGLE_STRIP, _numGroundPoints, GL_UNSIGNED_SHORT, (void*)0);
    //// END DRAWING THE GROUND PLANE ////

    //// BEGIN DRAWING THE BUILDINGS ////

}

void FPEngine::_updateScene()
{
    // switch cams
    if (_keys[GLFW_KEY_SPACE])
    {
        cameraIndex += 1;
        if (cameraIndex == 2)
        {
            cameraIndex = 0;
        }

        _pArcballCam->recomputeOrientation();
        _pFreeCam->recomputeOrientation();

        _keys[GLFW_KEY_SPACE] = false;
    }

    if (_keys[GLFW_KEY_F]) {
        if (firstPerson) {
            firstPerson = false;
        } else {
            firstPerson = true;
        }
        _keys[GLFW_KEY_F] = false;
    }

    if (_keys[GLFW_KEY_1]) {
        shaderIndex++;
        shaderIndex = shaderIndex % 2;

        _keys[GLFW_KEY_1] = false;
    }

    // move cart forward
    if (_keys[GLFW_KEY_W] || _keys[GLFW_KEY_UP]) {
        if (cameraIndex == 1) {
            cameras[cameraIndex]->moveForward(0.5f);
        }
    }

    _pMapCam->recomputeOrientation();


    // move cart backward
    if (_keys[GLFW_KEY_S] || _keys[GLFW_KEY_DOWN]) {
        if (cameraIndex == 1) {
            cameras[cameraIndex]->moveBackward(0.5f);
        }
    }
}

void FPEngine::run()
{
    //  This is our draw loop - all rendering is done here.  We use a loop to keep the window open
    //	until the user decides to close the window and quit the program.  Without a loop, the
    //	window will display once and then the program exits.
    while (!glfwWindowShouldClose(mpWindow))
    {
        // check if the window was instructed to be closed
        glDrawBuffer(GL_BACK); // work with our back frame buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // clear the current color contents and depth buffer in the window

        // Get the size of our framebuffer.  Ideally this should be the same dimensions as our window, but
        // when using a Retina display the actual window can be larger than the requested window.  Therefore,
        // query what the actual size of the window we are rendering to is.
        GLint framebufferWidth, framebufferHeight;
        glfwGetFramebufferSize(mpWindow, &framebufferWidth, &framebufferHeight);

        // update the viewport - tell OpenGL we want to render to the whole window
        glViewport(0, 0, framebufferWidth, framebufferHeight);
        // draw everything to the window
        _renderScene(cameras[cameraIndex]->getViewMatrix(), cameras[cameraIndex]->getProjectionMatrix());

        if (firstPerson) {
            glClear(GL_DEPTH_BUFFER_BIT);
            glViewport(framebufferWidth - 200, framebufferHeight - 200, 200, 200);
            _renderScene(_pMapCam->getViewMatrix(), _pMapCam->getProjectionMatrix());
        }


        _updateScene();
        glfwSwapBuffers(mpWindow); // flush the OpenGL commands and make sure they get rendered!

        glfwPollEvents(); // check for any events and signal to redraw screen
    }
}

//*************************************************************************************
//
// Private Helper Functions

void FPEngine::_computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const
{
    // precompute the Model-View-Projection matrix on the CPU
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    // then send it to the shader on the GPU to apply to every vertex
    _shaderPrograms[shaderIndex]->setProgramUniform(_shaderUniformLocations[shaderIndex]->mvpMatrix, mvpMtx);


    // TODO #7: compute and send the normal matrix
    glm::mat3 normalMtx = glm::mat3(glm::transpose(glm::inverse(modelMtx)));
    _shaderPrograms[shaderIndex]->setProgramUniform(_shaderUniformLocations[shaderIndex]->normalMatrix, normalMtx);

}

//*************************************************************************************
//
// Callbacks

void a3_engine_keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto engine = (FPEngine*)glfwGetWindowUserPointer(window);

    // pass the key and action through to the engine
    engine->handleKeyEvent(key, action);
}

void a3_engine_cursor_callback(GLFWwindow* window, double x, double y)
{
    auto engine = (FPEngine*)glfwGetWindowUserPointer(window);

    // pass the cursor position through to the engine
    engine->handleCursorPositionEvent(glm::vec2(x, y));
}

void a3_engine_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    auto engine = (FPEngine*)glfwGetWindowUserPointer(window);

    // pass the mouse button and action through to the engine
    engine->handleMouseButtonEvent(button, action);
}
