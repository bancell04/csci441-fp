#include "SirByzler.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <CSCI441/objects.hpp>
#include <CSCI441/OpenGLUtils.hpp>

SirByzler::SirByzler( GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation, GLint materialColorUniformLocation ) {
    _propAngle = 0.0f;
    _propAngleRotationSpeed = _PI / 16.0f;

    _shaderProgramHandle                            = shaderProgramHandle;
    _shaderProgramUniformLocations.mvpMtx           = mvpMtxUniformLocation;
    _shaderProgramUniformLocations.normalMtx        = normalMtxUniformLocation;
    _shaderProgramUniformLocations.materialColor    = materialColorUniformLocation;

    _rotatePlaneAngle = _PI / 2.0f;

    _colorBody = glm::vec3( 0.0f, 0.0f, 1.0f );
    _scaleBody = glm::vec3( 2.0f, 0.5f, 1.0f );

    _colorWing = glm::vec3( 1.0f, 0.0f, 0.0f );
    _scaleWing = glm::vec3( 1.5f, 0.5f, 1.0f );
    _rotateWingAngle = _PI / 2.0f;

    _colorNose = glm::vec3( 0.0f, 1.0f, 0.0f );
    _rotateNoseAngle = _PI / 2.0f;

    _colorProp = glm::vec3( 1.0f, 1.0f, 1.0f );
    _scaleProp = glm::vec3( .8f, .7, 0.025f );
    _transProp = glm::vec3( 0.1f, -0.1f, 0.0f );

    _colorTail = glm::vec3( 1.0f, 1.0f, 0.0f );

    _colorWheel = glm::vec3(0.9f, 0.9f, 0.9f);
    _scaleWheel = glm::vec3(0.1f, 0.5f, 0.1f);
}

void SirByzler::drawPlane( glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) {
    modelMtx = glm::rotate( modelMtx, -_rotatePlaneAngle, CSCI441::Y_AXIS );
    modelMtx = glm::rotate( modelMtx, _rotatePlaneAngle, CSCI441::Z_AXIS );
    _drawPlaneBody(modelMtx, viewMtx, projMtx);        // the body of our plane
    _drawWheel(true, true, modelMtx, viewMtx, projMtx);
    _drawWheel(true, false, modelMtx, viewMtx, projMtx);
    _drawWheel(false, true, modelMtx, viewMtx, projMtx);
    _drawWheel(false, false, modelMtx, viewMtx, projMtx);
    _drawPlaneWing(true, modelMtx, viewMtx, projMtx);  // the left wing
    _drawPlaneWing(false, modelMtx, viewMtx, projMtx); // the right wing
    _drawPlanePropeller(true, modelMtx, viewMtx, projMtx);   // the propeller
    _drawPlanePropeller(false, modelMtx, viewMtx, projMtx);   // the propeller
    _drawPlaneTail(modelMtx, viewMtx, projMtx);        // the tail
    
}

void SirByzler::flyForward() {
    _propAngle -= _propAngleRotationSpeed;
    if( _propAngle > _2PI ) _propAngle += _2PI;
}

void SirByzler::flyBackward() {
    _propAngle += _propAngleRotationSpeed;
    if( _propAngle < 0.0f ) _propAngle -= _2PI;
}

void SirByzler::_drawPlaneBody(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    modelMtx = glm::scale( modelMtx, _scaleBody );

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, glm::value_ptr(_colorBody));

    CSCI441::drawSolidCube( 0.1f );
}

void SirByzler::_drawWheel(bool isFrontWheel, bool isLeftWheel, glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    modelMtx = glm::scale(modelMtx, _scaleWheel);
    modelMtx = glm::translate(modelMtx, glm::vec3((isFrontWheel ? -1.f : 1.f), -0.1f, (isLeftWheel ? 0.5f : -0.5f)));
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, glm::value_ptr(_colorWheel));

    CSCI441::drawSolidCube(0.2f);
}

void SirByzler::_drawPlaneWing(bool isLeftWing, glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    modelMtx = glm::scale( modelMtx, _scaleWing );
    modelMtx = glm::rotate( modelMtx, (isLeftWing ? -1.f : 1.f) * _rotateWingAngle, CSCI441::X_AXIS );

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, glm::value_ptr(_colorWing));

    CSCI441::drawSolidCone( 0.05f, 0.2f, 16, 16 );
}

void SirByzler::_drawPlaneNose(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    modelMtx = glm::rotate( modelMtx, _rotateNoseAngle, CSCI441::Z_AXIS );

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, glm::value_ptr(_colorNose));

    CSCI441::drawSolidCone( 0.025f, 0.3f, 16, 16 );
}

void SirByzler::_drawPlanePropeller(bool isFront, glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    glm::mat4 modelMtx1 = glm::translate( modelMtx, _transProp );
    modelMtx1 = glm::rotate( modelMtx1, float(M_PI/2.0f), CSCI441::Y_AXIS );
    if (isFront) {
        modelMtx1 = glm::translate(modelMtx1, glm::vec3(0.0f, 0.0f, -0.2f));
    }
    modelMtx1 = glm::rotate( modelMtx1, _propAngle, CSCI441::X_AXIS );
    modelMtx1 = glm::scale( modelMtx1, _scaleProp );

    _computeAndSendMatrixUniforms(modelMtx1, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, glm::value_ptr(_colorProp));

    CSCI441::drawSolidCube( 0.1f );

    glm::mat4 modelMtx2 = glm::translate( modelMtx, _transProp );
    if (isFront) {
        modelMtx2 = glm::translate(modelMtx2, glm::vec3(-0.2f, 0.0f, 0.0f));
    }
    modelMtx2 = glm::rotate( modelMtx2, float(M_PI/2.0f), CSCI441::Y_AXIS );
    modelMtx2 = glm::rotate( modelMtx2, _PI_OVER_2 + _propAngle, CSCI441::X_AXIS );
    modelMtx2 = glm::scale( modelMtx2, _scaleProp );

    _computeAndSendMatrixUniforms(modelMtx2, viewMtx, projMtx);

    CSCI441::drawSolidCube( 0.1f );
}

void SirByzler::_drawPlaneTail(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    modelMtx = glm::translate( modelMtx, glm::vec3(-0.05f, 0.0f, 0.0f));
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, glm::value_ptr(_colorTail));
    CSCI441::drawSolidCylinder(0.01f, 0.01f, 0.15f, 20, 20);

    modelMtx = glm::translate( modelMtx, glm::vec3(0.0f, 0.15f, 0.0f));
    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);
    glProgramUniform3fv(_shaderProgramHandle, _shaderProgramUniformLocations.materialColor, 1, glm::value_ptr(_colorProp));
    CSCI441::drawSolidSphere(0.015f, 20, 20);
}

void SirByzler::_computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    // precompute the Model-View-Projection matrix on the CPU
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    // then send it to the shader on the GPU to apply to every vertex
    glProgramUniformMatrix4fv( _shaderProgramHandle, _shaderProgramUniformLocations.mvpMtx, 1, GL_FALSE, glm::value_ptr(mvpMtx) );

    glm::mat3 normalMtx = glm::mat3( glm::transpose( glm::inverse( modelMtx )));
    glProgramUniformMatrix3fv( _shaderProgramHandle, _shaderProgramUniformLocations.normalMtx, 1, GL_FALSE, glm::value_ptr(normalMtx) );
}
