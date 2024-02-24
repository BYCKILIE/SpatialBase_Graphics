//
// Created by tudor on 2/15/2024.
//

#ifndef PG_PROJECT_BOARD_H
#define PG_PROJECT_BOARD_H

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types
#include <utility>
#include <thread>
#include <Windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "ModelReferences.h"
#include "CustomModel.h"

class Board : public CustomModel {

    bool animation = true;
    bool insert = false;
    bool pressed_e = false;
    bool *emergency;

    std::thread th[2];

    float animationScore = 0.0f;

public:
    Board(ModelReferences *r, bool *emergency) : CustomModel(r), emergency(emergency) {
        setCF([this]() { script(); });
    }

private:

    void script() {

        processTrigger();

        if (insert) {
            insertAnimation();
            if (animationScore > 5) {
                th[0] = std::thread(&Board::startEffects, this);
                unDraw();
            }
            return;
        }

        if (animation) {
            loopAnimation();
        } else {
            carryObject();

        }
    }

    void loopAnimation() {
        r->model = glm::translate(r->model, object.getModelCenter());
        r->model = glm::rotate(r->model, glm::radians((float)(70 * (*r->deltaTime))), glm::vec3(0.0f, 1.0f, 0.0f));
        r->model = glm::translate(r->model, -object.getModelCenter());
    }

    void carryObject() {
        r->model = glm::translate(glm::mat4(1.0f), glm::vec3(-0.45f, -0.4f, -1.0f));
        r->model = glm::rotate(r->model, glm::radians(-17.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        r->model = glm::rotate(r->model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        r->model = glm::inverse(r->camera->getViewMatrix()) * r->model;
        r->model = glm::translate(r->model, -object.getModelCenter());
    }

    void processTrigger() {
        if (r->keys[GLFW_KEY_E] && !pressed_e) {
            pressed_e = true;
            if (animation) {
                if (glm::distance(r->model * glm::vec4(object.getModelCenter(), 1.0f), glm::vec4(r->camera->cameraPosition, 1.0f)) < 5.0f) {
                    animation = false;
                }
            } else {
                if (glm::distance(glm::vec3(16.7f, 4.6f, -28.8f), r->camera->cameraPosition) < 7.0f) {
                    insert = true;
                } else {
                    animation = true;
                    recalibrateObject();
                }
            }
        }
        else if (!r->keys[GLFW_KEY_E]) {
            pressed_e = false;
        }
    }

    void recalibrateObject() {
        r->model = glm::translate(glm::mat4(1.0f), glm::vec3(-0.45f, -0.4f, -1.0f));
        r->model = glm::rotate(r->model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        r->model = glm::inverse(r->camera->getViewMatrix()) * r->model;
        r->model = glm::translate(r->model, -object.getModelCenter());
    }

    void insertAnimation() {
        r->model = glm::translate(glm::mat4(1.0f), glm::vec3(-0.0f, -0.4f - animationScore, -1.0f));
        r->model = glm::rotate(r->model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        r->model = glm::inverse(r->camera->getViewMatrix()) * r->model;
        r->model = glm::translate(r->model, -object.getModelCenter());
        animationScore += 1.5f * (*r->deltaTime);
    }

    void startEffects() {
        playSound("affirmative");
        playSound("begin");
        playSound("processing");
        playSound("incomingtransmission");
        playSound("enterauthorizationcode");
        playSound("inputok");
        playSound("inputfailed");
        playSound("verified");
        playSound("transfer");
        *emergency = true;
        playSound("complete");
        playSound("processing");
        playSound("warning");
        playSound("alarm");
        playSound("alarm");
    }

    void playSound(const std::string& fileName) {
        std::string fullPath = "sounds/" + fileName + ".wav";
        PlaySound(TEXT(fullPath.c_str()), nullptr, SND_FILENAME | SND_SYNC);
    }
};

#endif //PG_PROJECT_BOARD_H
