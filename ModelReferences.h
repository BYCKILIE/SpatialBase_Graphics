//
// Created by tudor on 2/14/2024.
//

#ifndef PG_PROJECT_MODELREFERENCES_H
#define PG_PROJECT_MODELREFERENCES_H

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"

class ModelReferences {

public:
    ModelReferences(float* deltaTime, gps::Shader *shader, GLint modelLoc, GLint normalMatLoc, glm::mat3 *normalMat)
    : deltaTime(deltaTime), shader(shader), modelLoc(modelLoc), model(glm::mat4(1.0f)), normalMatLoc(normalMatLoc), normalMat(normalMat) {}

    ModelReferences(float* deltaTime, gps::Shader *shader, GLint modelLoc, GLint normalMatLoc, glm::mat3 *normalMat, GLboolean *keys, gps::Camera *camera)
            : deltaTime(deltaTime), shader(shader), modelLoc(modelLoc), model(glm::mat4(1.0f)), normalMatLoc(normalMatLoc), normalMat(normalMat), keys(keys),
            camera(camera){}

    ModelReferences(float* deltaTime, gps::Shader *shader, GLint modelLoc, GLint normalMatLoc, glm::mat3 *normalMat, GLint lightColorLoc, GLint fogDensityLoc, GLboolean *keys, gps::Camera *camera)
            : deltaTime(deltaTime), shader(shader), modelLoc(modelLoc), model(glm::mat4(1.0f)), normalMatLoc(normalMatLoc), normalMat(normalMat),
            lightColorLoc(lightColorLoc), fogDensityLoc(fogDensityLoc), keys(keys), camera(camera){}

    float* deltaTime;
    gps::Shader *shader;
    GLint modelLoc;
    glm::mat4 model{};
    GLint normalMatLoc;
    glm::mat3 *normalMat;
    GLboolean *keys;
    gps::Camera *camera;

    GLint lightColorLoc;
    GLint fogDensityLoc;
};


#endif //PG_PROJECT_MODELREFERENCES_H
