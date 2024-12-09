#ifndef LAB05_LAB05_ENGINE_H
#define LAB05_LAB05_ENGINE_H

#include <CSCI441/ArcBallCam.hpp>
#include <CSCI441/FreeCam.hpp>
#include <CSCI441/OpenGLEngine.hpp>
#include <CSCI441/ShaderProgram.hpp>

#include <vector>

class FPEngine final : public CSCI441::OpenGLEngine {
public:
    FPEngine();
    ~FPEngine() final;

    void run() final;

    /// \desc handle any key events inside the engine
    /// \param key key as represented by GLFW_KEY_ macros
    /// \param action key event action as represented by GLFW_ macros
    void handleKeyEvent(GLint key, GLint action);

    /// \desc handle any mouse button events inside the engine
    /// \param button mouse button as represented by GLFW_MOUSE_BUTTON_ macros
    /// \param action mouse event as represented by GLFW_ macros
    void handleMouseButtonEvent(GLint button, GLint action);

    /// \desc handle any cursor movement events inside the engine
    /// \param currMousePosition the current cursor position
    void handleCursorPositionEvent(glm::vec2 currMousePosition);

    /// \desc value off-screen to represent mouse has not begun interacting with window yet
    static constexpr GLfloat MOUSE_UNINITIALIZED = -9999.0f;

private:
    void mSetupGLFW() final;
    void mSetupOpenGL() final;
    void mSetupShaders() final;
    void mSetupBuffers() final;
    void mSetupTextures() final;
    void mSetupScene() final;

    void mCleanupBuffers() final;
    void mCleanupShaders() final;

    /// \desc draws everything to the scene from a particular point of view
    /// \param viewMtx the current view matrix for our camera
    /// \param projMtx the current projection matrix for our camera
    void _renderScene(glm::mat4 viewMtx, glm::mat4 projMtx) const;
    /// \desc handles moving our FreeCam as determined by keyboard input
    void _updateScene();

    /// \desc tracks the number of different keys that can be present as determined by GLFW
    static constexpr GLuint NUM_KEYS = GLFW_KEY_LAST;
    /// \desc boolean array tracking each key state.  if true, then the key is in a pressed or held
    /// down state.  if false, then the key is in a released state and not being interacted with
    GLboolean _keys[NUM_KEYS];

    /// \desc last location of the mouse in window coordinates
    glm::vec2 _mousePosition;
    /// \desc current state of the left mouse button
    GLint _leftMouseButtonState;

    /// \desc the static fixed camera in our world
    CSCI441::ArcballCam* _pArcballCam;
    CSCI441::FreeCam* _pFreeCam;
    CSCI441::FreeCam* _pMapCam;
    int cameraIndex = 0;
    CSCI441::Camera* cameras[2] = {
        _pArcballCam,
        _pFreeCam
    };

    /// \desc pair of values to store the speed the camera can move/rotate.
    /// \brief x = forward/backward delta, y = rotational delta
    glm::vec2 _cameraSpeed;
    bool firstPerson = true;
    /// \desc our plane model


    /// \desc the size of the world (controls the ground size and locations of buildings)
    static constexpr GLfloat WORLD_SIZE = 55.0f;
    /// \desc VAO for our ground
    GLuint _groundVAO;
    /// \desc the number of points that make up our ground object
    GLsizei _numGroundPoints;

    /// \desc creates the ground VAO
    void _createGroundBuffers();

    void _createSkyBox();

    /// \desc smart container to store information specific to each building we wish to draw
    struct BuildingData {
        /// \desc transformations to position and size the building
        glm::mat4 modelMatrix;
        /// \desc color to draw the building
        glm::vec3 color;
    };
    /// \desc information list of all the buildings to draw
    std::vector<BuildingData> _buildings;

    /// \desc generates building information to make up our scene
    void _generateEnvironment();


    /// \desc precomputes the matrix uniforms CPU-side and then sends them
    /// to the GPU to be used in the shader for each vertex.  It is more efficient
    /// to calculate these once and then use the resultant product in the shader.
    /// \param modelMtx model transformation matrix
    /// \param viewMtx camera view matrix
    /// \param projMtx camera projection matrix
    void _computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;



    //***************************************************************************
    // Texture Information

    // TODO #08-START this step has been done for you, but check out how it is implemented

    /// \desc total number of textures in our scene
    static constexpr GLuint NUM_TEXTURES = 1;
    /// \desc used to index through our texture array to give named access
    enum TEXTURE_ID {
        /// \desc metal texture
        SKYBOX = 0,
        DIRT = 1,
        /// \desc Mines logo texture
    };
    /// \desc texture handles for our textures
    GLuint _texHandles[NUM_TEXTURES];

    // TODO #08-END this step has been done for you, but check out how it is implemented

    /// \desc loads an image into CPU memory and registers it with the GPU
    /// \note sets the texture parameters and sends the data to the GPU
    /// \param FILENAME external image filename to load
    static GLuint _loadAndRegisterTexture(const char* FILENAME);

    //***************************************************************************
    // Shader Program Information


    /// \desc shader program that performs texturing
    // Define the structs once
    struct shaderUniformLocations {
        GLint mvpMatrix;
        GLint texMap;
        GLint materialColor;
        GLint normalMatrix;
        GLint cameraPos;
        GLint useTexture;
        // Light uniforms
        GLint lightDirection;
        GLint lightColor;
        GLint spotlightPos;
        GLint spotlightDir;
        GLint spotlightColor;
        GLint spotlightCutOff;
        GLint spotlightOuterCutOff;
        GLint pointlightPos;
        GLint pointlightColor;
        GLfloat time;
    };

    struct shaderAttributeLocations {
        GLint vPos;
        GLint vNormal;
        GLint texCoord;
    };

    CSCI441::ShaderProgram* _regularShaderProgram;
    shaderUniformLocations _regularShaderUniformLocations;
    shaderAttributeLocations _regularShaderAttributeLocations;

    CSCI441::ShaderProgram* _glitchedShaderProgram;
    shaderUniformLocations _glitchedShaderUniformLocations;
    shaderAttributeLocations _glitchedShaderAttributeLocations;

    int shaderIndex;

    CSCI441::ShaderProgram* _shaderPrograms[2] = {
        _regularShaderProgram,
        _glitchedShaderProgram
    };

    shaderAttributeLocations* _shaderAttributeLocations[2] = {
        &_regularShaderAttributeLocations,
        &_glitchedShaderAttributeLocations
    };

    shaderUniformLocations* _shaderUniformLocations[2] = {
        &_regularShaderUniformLocations,
        &_glitchedShaderUniformLocations
    };


};


void a3_engine_keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods );
void a3_engine_cursor_callback(GLFWwindow *window, double x, double y );
void a3_engine_mouse_button_callback(GLFWwindow *window, int button, int action, int mods );

#endif// LAB05_LAB05_ENGINE_H
