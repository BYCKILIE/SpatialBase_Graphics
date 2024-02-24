//
// Created by tudor on 2/14/2024.
//

#ifndef PG_PROJECT_CUSTOMMODEL_H
#define PG_PROJECT_CUSTOMMODEL_H

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "CustomModel.h"
#include "ModelReferences.h"

#include <functional>

class CustomModel {

public:

    explicit CustomModel(ModelReferences *modelReferences);

    void load(std::string path);

    void drawModel();

    ~CustomModel() {
        delete r;
    }

protected:

    ModelReferences* r;

    void setCF(std::function<void()> fun);

    void unDraw();

    gps::Model3D object{};

private:

    bool isSet = false;
    bool draw = true;
    std::function<void()> customFunction;

};


#endif //PG_PROJECT_CUSTOMMODEL_H
