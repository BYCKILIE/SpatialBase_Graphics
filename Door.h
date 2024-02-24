//
// Created by tudor on 2/14/2024.
//

#ifndef PG_PROJECT_DOOR_H
#define PG_PROJECT_DOOR_H

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types
#include <utility>

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "ModelReferences.h"
#include "CustomModel.h"

class Door : public CustomModel {

public:

    explicit Door(ModelReferences *r) : CustomModel(r) {
        setCF([this]() { loopAnimation(); });
    }

    void processTrigger() {
        if (animation) {
            return;
        }
        if (glm::distance(object.getModelCenter(), r->camera->cameraPosition) > 7.0f) {
            return;
        }
        if (r->keys[GLFW_KEY_E] && !pressed_e) {
            pressed_e = true;
            doorClosed = !doorClosed;
            animation = true;
        }
        else if (!r->keys[GLFW_KEY_E]) {
            pressed_e = false;
        }
    }

    void loopAnimation() {
        processTrigger();
        if (!animation)
            return;
        if (doorClosed) {
            r->model = glm::translate(r->model, glm::vec3(0.0f, doorSpeed * (*r->deltaTime), 0.0f));
            distance++;
        } else {
            r->model = glm::translate(r->model, glm::vec3(0.0f, -doorSpeed * (*r->deltaTime), 0.0f));
            distance++;
        }
        if (distance > 90) {
            animation = false;
            distance = 0;
            if (doorClosed) {
                r->model = glm::mat4(1.0f);
            }
        }
    }

private:

    GLfloat doorSpeed = 5.0f;

    bool pressed_e = false;
    bool animation = false;
    bool doorClosed = true;
    int distance = 0;

};


#endif //PG_PROJECT_DOOR_H
