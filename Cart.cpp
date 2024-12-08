#include "Cart.h"

#include <glm/gtc/matrix_transform.hpp>

#include <CSCI441/OpenGLUtils.hpp>
#include <CSCI441/objects.hpp>

#include <cstdlib>

Cart::Cart()
    : _rotation(0.0f), _location(glm::vec3(0,0,0)), _direction(glm::vec3(1,0,0)),
      RADIUS(0.5f),
      SPEED(0.1f),
      COLOR(glm::vec3(_genRandColor(), _genRandColor(), _genRandColor())) {

}

Cart::Cart(glm::vec3 location, glm::vec3 direction, GLfloat radius )
    : _rotation(0.0f), _location(location), _direction(direction),
      RADIUS(radius),
      SPEED(0.1f),
      COLOR(glm::vec3(_genRandColor(), _genRandColor(), _genRandColor())) {
    _direction = glm::normalize( _direction );
}

glm::vec3 Cart::getLocation() const { return _location; }
void Cart::setLocationX(GLfloat x) { _location.x = x; }
void Cart::setLocationZ(GLfloat z) { _location.z = z; }
glm::vec3 Cart::getDirection() const { return _direction; }
void Cart::setDirection(glm::vec3 newDirection) { _direction = glm::normalize(newDirection); }

void Cart::draw( CSCI441::ShaderProgram *shaderProgram, GLint mvpUniformLocation, GLint colorUniformLocation, glm::mat4 modelMtx, glm::mat4 projViewMtx, CSCI441::ModelLoader* _pCartModel) {
    modelMtx = glm::translate( modelMtx, _location );
    modelMtx = glm::translate( modelMtx, glm::vec3(0, RADIUS, 0 ) );
    modelMtx = glm::scale( modelMtx, glm::vec3( 0.2, 0.2, 0.2 ) );

    glm::vec3 rotationAxis = glm::cross( _direction, CSCI441::Y_AXIS );
    modelMtx = glm::rotate( modelMtx, _rotation, rotationAxis );

    glm::mat4 mvpMatrix = projViewMtx * modelMtx;
    shaderProgram->setProgramUniform( mvpUniformLocation, mvpMatrix );

    shaderProgram->setProgramUniform(colorUniformLocation, COLOR );


    if(_pCartModel != nullptr) {
        if( !_pCartModel->draw(shaderProgram->getShaderProgramHandle() ) ) {
            fprintf(stderr, "[ERROR]: Could not draw OBJ Model\n");
        }
    }
}

void Cart::moveForward() {
    _location += _direction * SPEED;
    _rotation -= SPEED;
    if( _rotation < 0.0f ) {
        _rotation += 6.28f;
    }
}

void Cart::moveBackward() {
    _location -= _direction * SPEED;
    _rotation += SPEED;
    if( _rotation > 6.28f ) {
        _rotation -= 6.28f;
    }
}

GLfloat Cart::_genRandColor() {
    return (GLfloat)(rand() * 100 % 50) / 100.0f + 0.5f;
}