//
// Created by tudor on 2/14/2024.
//

#include "CustomModel.h"

CustomModel::CustomModel(ModelReferences *modelReferences) : r(modelReferences){}

void CustomModel::load(std::string path) {
    object.LoadModel(path);
}

void CustomModel::setCF(std::function<void()> fun) {
    customFunction = std::move(fun);
    isSet = true;
}

void CustomModel::drawModel() {
    if (!draw) {
        return;
    }

    if (isSet) {
        customFunction();
    }

    r->shader->useShaderProgram();

    glUniformMatrix4fv(r->modelLoc, 1, GL_FALSE, glm::value_ptr(r->model));

    glUniformMatrix3fv(r->normalMatLoc, 1, GL_FALSE, glm::value_ptr(*r->normalMat));

    object.Draw(*r->shader);
}

void CustomModel::unDraw() {
    draw = false;
}
