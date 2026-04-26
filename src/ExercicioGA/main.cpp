#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <sstream>
#include <iomanip>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fileReader.hpp>
#include <cmath>
#include <shader.hpp>
#include <camera.hpp>
#include <model.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double, double);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void updateWindowTitle(GLFWwindow* window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0, 0.0, 5.0));
float lastX = SCR_WIDTH / 2.0f, lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0, lastFrame = 0.0;

glm::vec3 lightPos(3.0f, 3.0f, 3.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
glm::vec3 ka(0.2f, 0.2f, 0.2f);
glm::vec3 kd(0.6f, 0.6f, 0.6f);
glm::vec3 ks(0.4f, 0.4f, 0.4f);
float shininess = 32.0f;

std::vector<glm::vec3> objectColors = {
    glm::vec3(0.8f, 0.2f, 0.2f),
    glm::vec3(0.2f, 0.8f, 0.2f),
    glm::vec3(0.2f, 0.2f, 0.8f),
    glm::vec3(0.8f, 0.8f, 0.2f),
    glm::vec3(0.8f, 0.2f, 0.8f),
};

bool isPerspective = true;
std::string currentOpName = "Translacao";

Shader* shaderGlobal;
int currentModel = 0;
std::vector<Model> modelList;

void updateWindowTitle(GLFWwindow* window) {
    std::ostringstream title;
    title << std::fixed << std::setprecision(2);
    title << "CG - Grau A | Obj:" << currentModel
          << " | Op:" << currentOpName
          << " | Proj:" << (isPerspective ? "Persp" : "Orto")
          << " | ka=" << ka.x << " kd=" << kd.x << " ks=" << ks.x
          << " sh=" << (int)shininess;
    glfwSetWindowTitle(window, title.str().c_str());
}

int main() {
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CG - Grau A", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glEnable(GL_DEPTH_TEST);

    Shader shader("./shaders/vertex.glsl", "./shaders/fragment.glsl", "./shaders/geometry.glsl");
    shaderGlobal = &shader;
    shader.use();
    shader.setVec3f("wireframeColor", glm::vec3(0.0f, 1.0f, 0.0f));
    shader.setFloat("wireframeWidth", 0.005f);
    shader.setBool("wireframe", false);
    shader.setVec3f("lightPos", lightPos);
    shader.setVec3f("lightColor", lightColor);
    shader.setVec3f("ka", ka);
    shader.setVec3f("kd", kd);
    shader.setVec3f("ks", ks);
    shader.setFloat("shininess", shininess);

    // === MODELOS DA CENA - adicione ou troque aqui ===
    modelList.push_back(Model("./assets/Modelos3D/Cube.obj"));
    modelList.push_back(Model("./assets/Modelos3D/Suzanne.obj"));
    modelList.push_back(Model("./modelos/LulaMolusco/scene.gltf"));
    // modelList.push_back(Model("./modelos/bunny/bunny.obj"));

    // Posiciona os modelos lado a lado
    float spacing = 1.5f;
    float startX = -spacing * ((int)modelList.size() - 1) / 2.0f;
    for (int i = 0; i < (int)modelList.size(); i++)
        modelList[i].moveModel(X_AXIS, startX + i * spacing);

    std::cout << "=== CG Grau A - Controles ===" << std::endl;
    std::cout << "WASD + Mouse: camera FPS" << std::endl;
    std::cout << "NumPad +/-: selecionar objeto" << std::endl;
    std::cout << "NumPad 7/4/1/0: Translacao/Rotacao/Escala/EscalaUniforme" << std::endl;
    std::cout << "NumPad 8/9 5/6 2/3: eixos X/Y/Z" << std::endl;
    std::cout << "NumPad /: wireframe | NumPad .: reset" << std::endl;
    std::cout << "P: Perspectiva/Ortografica" << std::endl;
    std::cout << "I/K: ka | O/L: kd | U/J: ks | N/M: shininess" << std::endl;

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        updateWindowTitle(window);

        glClearColor(0.15f, 0.15f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();

        glm::mat4 projection;
        if (isPerspective)
            projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.f);
        else
            projection = glm::ortho(-5.0f, 5.0f, -4.0f, 4.0f, 0.1f, 100.f);

        shader.setMatrix4f("projection", projection);
        shader.setMatrix4f("view", camera.GetViewMatrix());
        shader.setVec3f("viewPos", camera.Position);
        shader.setVec3f("ka", ka);
        shader.setVec3f("kd", kd);
        shader.setVec3f("ks", ks);
        shader.setFloat("shininess", shininess);

        for (int i = 0; i < (int)modelList.size(); i++) {
            shader.setVec3f("objectColor", objectColors[i % objectColors.size()]);
            modelList[i].Draw(shader);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS) modelList[currentModel].moveModel(X_AXIS, -deltaTime);
    if (glfwGetKey(window, GLFW_KEY_KP_9) == GLFW_PRESS) modelList[currentModel].moveModel(X_AXIS,  deltaTime);
    if (glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS) modelList[currentModel].moveModel(Y_AXIS, -deltaTime);
    if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS) modelList[currentModel].moveModel(Y_AXIS,  deltaTime);
    if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS) modelList[currentModel].moveModel(Z_AXIS, -deltaTime);
    if (glfwGetKey(window, GLFW_KEY_KP_3) == GLFW_PRESS) modelList[currentModel].moveModel(Z_AXIS,  deltaTime);
    float step = 0.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) ka = glm::clamp(ka + glm::vec3(step), glm::vec3(0.0f), glm::vec3(1.0f));
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) ka = glm::clamp(ka - glm::vec3(step), glm::vec3(0.0f), glm::vec3(1.0f));
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) kd = glm::clamp(kd + glm::vec3(step), glm::vec3(0.0f), glm::vec3(1.0f));
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) kd = glm::clamp(kd - glm::vec3(step), glm::vec3(0.0f), glm::vec3(1.0f));
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) ks = glm::clamp(ks + glm::vec3(step), glm::vec3(0.0f), glm::vec3(1.0f));
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) ks = glm::clamp(ks - glm::vec3(step), glm::vec3(0.0f), glm::vec3(1.0f));
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) shininess = std::min(256.0f, shininess + 20.0f * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) shininess = std::max(1.0f,   shininess - 20.0f * deltaTime);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    static bool wireframe = false;
    if (key == GLFW_KEY_KP_ADD && action == GLFW_PRESS) {
        currentModel = std::min(++currentModel, (int)modelList.size()-1);
        modelList[currentModel].setOperation(TRANSLATE); currentOpName = "Translacao";
    }
    if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_PRESS) {
        currentModel = std::max(0, --currentModel);
        modelList[currentModel].setOperation(TRANSLATE); currentOpName = "Translacao";
    }
    if (key == GLFW_KEY_KP_DIVIDE  && action == GLFW_PRESS) { wireframe ^= 1; shaderGlobal->setBool("wireframe", wireframe); }
    if (key == GLFW_KEY_KP_7       && action == GLFW_PRESS) { modelList[currentModel].setOperation(TRANSLATE);        currentOpName = "Translacao"; }
    if (key == GLFW_KEY_KP_4       && action == GLFW_PRESS) { modelList[currentModel].setOperation(ROTATE);           currentOpName = "Rotacao"; }
    if (key == GLFW_KEY_KP_1       && action == GLFW_PRESS) { modelList[currentModel].setOperation(SCALE);            currentOpName = "Escala"; }
    if (key == GLFW_KEY_KP_0       && action == GLFW_PRESS) { modelList[currentModel].setOperation(SCALE_SIMETRICAL); currentOpName = "EscalaUnif"; }
    if (key == GLFW_KEY_KP_DECIMAL && action == GLFW_PRESS) modelList[currentModel].resetModelScaleAndRotation();
    if (key == GLFW_KEY_P          && action == GLFW_PRESS) isPerspective = !isPerspective;
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn), ypos = static_cast<float>(yposIn);
    if (firstMouse) { lastX = xpos; lastY = ypos; firstMouse = false; }
    camera.ProcessMouseMovement(xpos - lastX, lastY - ypos);
    lastX = xpos; lastY = ypos;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}


