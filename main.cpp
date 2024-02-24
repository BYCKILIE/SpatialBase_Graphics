#if defined (__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#define GL_SILENCE_DEPRECATION
#else
#define GLEW_STATIC

#include <GL/glew.h>

#endif

#include    <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "ModelReferences.h"
#include "CustomModel.h"

#include "Door.h"

#include "SkyBox.hpp"
#include "Board.h"

#include <iostream>
#include <chrono>

// window
gps::Window mainWindow;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix, normalMatrixOut;

// light parameters
glm::vec3 lightDir, lightDirOut;
glm::vec3 lightColor;
glm::vec3 lightColorOut;

// shader uniform locations
GLint modelLoc, modelLocOut;
GLint viewLoc, viewLocOut;
GLint projectionLoc, projectionLocOut;
GLint normalMatrixLoc, normalMatrixLocOut;
GLint lightDirLoc, lightDirLocOut;
GLint lightColorLoc, lightColorLocOut;

GLfloat fogDensity;
GLint fogDensityLoc;
glm::vec4 fogColor;
GLint fogColorLoc;

float deltaTime;

// camera
gps::Camera camera(
        glm::vec3(-12.7f, 0.5f, -52.0f),
        glm::vec3(0.0f, 0.0f, -10.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));

GLfloat cameraSpeed = 7.5f;

GLfloat cameraSensitivity = 4.5;
float pitch, yaw = -90.0f;

GLfloat angle;

GLboolean pressedKeys[1024];

// models
std::vector<CustomModel *> models;

// shaders
gps::Shader shaderProgram;
gps::Shader mapOutShader;

gps::SkyBox mySkyBox;
gps::Shader skyboxShader;

bool emergency = false;
glm::vec3 blackLight = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 redLight = glm::vec3(1.0f, 0.0f, 0.0f);

GLenum glCheckError_(const char *file, int line) {
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
            default:
                break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}

#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow *window, int width, int height) {
    fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
    //TODO
}

void keyboardCallback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        } else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
}

void processMovement() {
    if (pressedKeys[GLFW_KEY_W]) {
        camera.move(gps::MOVE_FORWARD, cameraSpeed * deltaTime);

        view = camera.getViewMatrix();

        shaderProgram.useShaderProgram();

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        mapOutShader.useShaderProgram();

        glUniformMatrix4fv(viewLocOut, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_S]) {
        camera.move(gps::MOVE_BACKWARD, cameraSpeed * deltaTime);

        view = camera.getViewMatrix();

        shaderProgram.useShaderProgram();

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        mapOutShader.useShaderProgram();

        glUniformMatrix4fv(viewLocOut, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_A]) {
        camera.move(gps::MOVE_LEFT, cameraSpeed * deltaTime);

        view = camera.getViewMatrix();

        shaderProgram.useShaderProgram();

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        mapOutShader.useShaderProgram();

        glUniformMatrix4fv(viewLocOut, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_D]) {
        camera.move(gps::MOVE_RIGHT, cameraSpeed * deltaTime);

        view = camera.getViewMatrix();

        shaderProgram.useShaderProgram();

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        mapOutShader.useShaderProgram();

        glUniformMatrix4fv(viewLocOut, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }
    if (pressedKeys[GLFW_KEY_N]) {
        angle -= 1.0f;
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        normalMatrixOut = glm::mat3(glm::inverseTranspose(view * model));
    }
    if (pressedKeys[GLFW_KEY_M]) {
        angle += 1.0f;
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        normalMatrixOut = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_F]) {
        fogDensity += 0.002f;
        if (fogDensity >= 0.3f)
            fogDensity = 0.3f;
        shaderProgram.useShaderProgram();
        glUniform1fv(fogDensityLoc, 1, &fogDensity);
    }
    if (pressedKeys[GLFW_KEY_G]) {
        fogDensity -= 0.002f;
        if (fogDensity <= 0.0f)
            fogDensity = 0.0f;
        shaderProgram.useShaderProgram();
        glUniform1fv(fogDensityLoc, 1, &fogDensity);
    }
}

bool firstMove = true;
double xPrev = 0, yPrev = 0;

void mouseCallback(GLFWwindow *window, const double xpos, const double ypos) {
    if (firstMove) {
        xPrev = xpos;
        yPrev = ypos;
        firstMove = false;
    }

    yaw += static_cast<float>((xpos - xPrev) * cameraSensitivity * deltaTime);
    pitch += static_cast<float>((yPrev - ypos) * cameraSensitivity * deltaTime);

    xPrev = xpos;
    yPrev = ypos;
}

void processRotation() {
    camera.rotate(pitch, yaw);
    view = camera.getViewMatrix();
    shaderProgram.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    mapOutShader.useShaderProgram();
    glUniformMatrix4fv(viewLocOut, 1, GL_FALSE, glm::value_ptr(view));

    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
}

bool k_switch = false;
char polygonMode = 0;

void processPolygonMode() {
    if (pressedKeys[GLFW_KEY_K] && !k_switch) {
        k_switch = true;
        if (polygonMode > 1) {
            polygonMode = 0;
        } else {
            polygonMode++;
        }
    } else if (!pressedKeys[GLFW_KEY_K]) {
        k_switch = false;
    }
    if (polygonMode == 0) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    } else if (polygonMode == 1) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }
}

void initOpenGLWindow() {
    mainWindow.Create("OpenGL Project Core");
}

void setWindowCallbacks() {
    glfwSetWindowSizeCallback(mainWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(mainWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(mainWindow.getWindow(), mouseCallback);
}

void initOpenGLState() {
    glViewport(0, 0, mainWindow.getWindowDimensions().width, mainWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE); // cull face
    glCullFace(GL_BACK); // cull back face
    glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initModels() {
    std::vector<std::string> references;
    references.emplace_back("models/scene/mapIn.obj");
    models.push_back(new CustomModel(
            new ModelReferences(&deltaTime, &shaderProgram, modelLoc, normalMatrixLoc, &normalMatrix)));
    references.emplace_back("models/scene/mapOut.obj");
    models.push_back(new CustomModel(
            new ModelReferences(&deltaTime, &mapOutShader, modelLocOut, normalMatrixLocOut, &normalMatrixOut)));
    references.emplace_back("models/scene/doorIn.obj");
    models.push_back(new Door(
            new ModelReferences(&deltaTime, &shaderProgram, modelLoc, normalMatrixLoc, &normalMatrix, pressedKeys,
                                &camera)));
    references.emplace_back("models/scene/doorOut.obj");
    models.push_back(new Door(
            new ModelReferences(&deltaTime, &mapOutShader, modelLocOut, normalMatrixLocOut, &normalMatrixOut, pressedKeys,
                                &camera)));
    references.emplace_back("models/circuitBoard/usbFlash.obj");
    models.push_back(new Board(
            new ModelReferences(&deltaTime, &shaderProgram, modelLoc, normalMatrixLoc, &normalMatrix,
                                lightColorLoc, fogDensityLoc, pressedKeys, &camera), &emergency));

    for (int i = 0; i < references.size(); ++i) {
        models.at(i)->load(references.at(i));
    }
}

void initShaders() {
    shaderProgram.loadShader(
            "Shaders/basic.vert",
            "Shaders/basic.frag");

    mapOutShader.loadShader(
            "Shaders/outShader.vert",
            "Shaders/outShader.frag");

    skyboxShader.loadShader(
            "shaders/skyboxShader.vert",
            "shaders/skyboxShader.frag");
}

void initSkyBox() {
    std::vector<const GLchar *> faces;
    faces.push_back("models/skybox/right.png");
    faces.push_back("models/skybox/left.png");
    faces.push_back("models/skybox/top.png");
    faces.push_back("models/skybox/bottom.png");
    faces.push_back("models/skybox/back.png");
    faces.push_back("models/skybox/front.png");
    mySkyBox.Load(faces);
}

void initUniformValues() {
    model = glm::mat4(1.0f);
    view = camera.getViewMatrix();
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    projection = glm::perspective(glm::radians(45.0f),
                                  (float) mainWindow.getWindowDimensions().width /
                                  (float) mainWindow.getWindowDimensions().height,
                                  0.1f, 1000.0f);
    lightDir = glm::vec3(1.0f, 1.0f, 1.0f);
    lightDirOut = glm::vec3(0.0f, 1.0f, 1.0f);
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
    lightColorOut = glm::vec3(1.0f, 1.0f, 1.0f); //white light

    fogColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
}

void initOutUniforms() {
    mapOutShader.useShaderProgram();

    modelLocOut = glGetUniformLocation(mapOutShader.shaderProgram, "model");

    viewLocOut = glGetUniformLocation(mapOutShader.shaderProgram, "view");
    glUniformMatrix4fv(viewLocOut, 1, GL_FALSE, glm::value_ptr(view));

    normalMatrixLocOut = glGetUniformLocation(mapOutShader.shaderProgram, "normalMatrix");

    projectionLocOut = glGetUniformLocation(mapOutShader.shaderProgram, "projection");
    glUniformMatrix4fv(projectionLocOut, 1, GL_FALSE, glm::value_ptr(projection));

    lightDirLocOut = glGetUniformLocation(mapOutShader.shaderProgram, "lightDir");
    glUniform3fv(lightDirLocOut, 1, glm::value_ptr(lightDirOut));

    lightColorLocOut = glGetUniformLocation(mapOutShader.shaderProgram, "lightColor");
    glUniform3fv(lightColorLocOut, 1, glm::value_ptr(lightColorOut));
}

void initUniforms() {
    shaderProgram.useShaderProgram();

    modelLoc = glGetUniformLocation(shaderProgram.shaderProgram, "model");

    viewLoc = glGetUniformLocation(shaderProgram.shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    normalMatrixLoc = glGetUniformLocation(shaderProgram.shaderProgram, "normalMatrix");

    projectionLoc = glGetUniformLocation(shaderProgram.shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    fogDensityLoc = glGetUniformLocation(shaderProgram.shaderProgram, "fogDensity");
    glUniform1fv(fogDensityLoc, 1, &fogDensity);

    lightDirLoc = glGetUniformLocation(shaderProgram.shaderProgram, "lightDir");
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

    lightColorLoc = glGetUniformLocation(shaderProgram.shaderProgram, "lightColor");
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    fogColorLoc = glGetUniformLocation(shaderProgram.shaderProgram, "fogColor");
    glUniform4fv(fogColorLoc, 1, glm::value_ptr(fogColor));
}

void setRedLight() {
    shaderProgram.useShaderProgram();
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(redLight));
}

void setBlackLight() {
    shaderProgram.useShaderProgram();
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(blackLight));
}

GLfloat animationStatus = 0;
void emergencyAnimation() {
    if (animationStatus < 15) {
        fogDensity += 0.002f * deltaTime;
        if (fogDensity >= 0.3f)
            fogDensity = 0.3f;
        shaderProgram.useShaderProgram();
        glUniform1fv(fogDensityLoc, 1, &fogDensity);
        setBlackLight();
    } else {
        setRedLight();
    }
    animationStatus += 2 * deltaTime;
}

void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (emergency) {
        emergencyAnimation();
    }

    for (auto object: models) {
        object->drawModel();
    }

    skyboxShader.useShaderProgram();
    mySkyBox.Draw(skyboxShader, view, projection);
}

void cleanup() {
    mainWindow.Delete();
    //cleanup code for your own data

    for (auto object: models) {
        delete object;
    }
}

int main(int argc, const char *argv[]) {

    try {
        initOpenGLWindow();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
    initShaders();
    initSkyBox();
    initUniformValues();
    initUniforms();
    initOutUniforms();
    setWindowCallbacks();
    initModels();

    auto lastFrameTime = std::chrono::high_resolution_clock::now();

    glCheckError();
    // application loop
    while (!glfwWindowShouldClose(mainWindow.getWindow())) {
        auto currentFrameTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> frameDuration = currentFrameTime - lastFrameTime;
        deltaTime = frameDuration.count();
        lastFrameTime = currentFrameTime;

        processMovement();
        processRotation();

        processPolygonMode();

        renderScene();

        glfwPollEvents();
        glfwSwapBuffers(mainWindow.getWindow());

        glCheckError();
    }

    cleanup();

    return EXIT_SUCCESS;
}
