#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fileReader.hpp>
#include <cmath>
#include <shader.hpp>
#include <camera.hpp>
#include <model.hpp>

#include <material.hpp>
#include <light.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double, double);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

bool orthographic = false;
glm::mat4 ortho = glm::ortho(
		-4.0f, 4.f, -3.f, 3.0f, 0.1f, 100.f);

Camera camera(glm::vec3(0.0, 0.0, 3.0));

float lastX = SCR_WIDTH / 2.0f, lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float deltaTime=0.0, lastFrame = 0.0;

Shader* shaderGlobal;
int currentModel = 0;
int currentLightDist = 0;
std::vector<Model> modelList; 

Light light = {
	.position=glm::vec3(1.2, 1.0, 2.0),
	.ambient=glm::vec3(1.0f),
	.diffuse=glm::vec3(1.0f),
	.specular=glm::vec3(1.0f),
	.distance = LIGHT_DISTANCES[currentLightDist]
};

int main() {
	

	if (!glfwInit()) return -1;
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "learn opengl", NULL, NULL);
	
	if (window == NULL){
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glEnable(GL_DEPTH_TEST);

	std::cout << "======================================" << std::endl;
	std::cout << "   CG - Grau A | Leitor 3D OpenGL   " << std::endl;
	std::cout << "======================================" << std::endl;
	std::cout << std::endl;
	std::cout << "  CAMERA" << std::endl;
	std::cout << "  W/S/A/D       Mover a camera" << std::endl;
	std::cout << "  Mouse         Rotacionar a camera" << std::endl;
	std::cout << "  Scroll        Zoom (FOV)" << std::endl;
	std::cout << "  P             Perspectiva / Ortografica" << std::endl;
	std::cout << std::endl;
	std::cout << "  OBJETOS" << std::endl;
	std::cout << "  NumPad +/-    Selecionar objeto" << std::endl;
	std::cout << "  NumPad 7      Modo: Translacao" << std::endl;
	std::cout << "  NumPad 4      Modo: Rotacao" << std::endl;
	std::cout << "  NumPad 1      Modo: Escala por eixo" << std::endl;
	std::cout << "  NumPad 0      Modo: Escala uniforme" << std::endl;
	std::cout << "  NumPad 8/9    Eixo X: diminuir / aumentar" << std::endl;
	std::cout << "  NumPad 5/6    Eixo Y: diminuir / aumentar" << std::endl;
	std::cout << "  NumPad 2/3    Eixo Z: diminuir / aumentar" << std::endl;
	std::cout << "  NumPad .      Reset escala e rotacao" << std::endl;
	std::cout << "  NumPad /      Toggle wireframe" << std::endl;
	std::cout << std::endl;
	std::cout << "  MATERIAIS E LUZ" << std::endl;
	std::cout << "  N/M           Ciclar materiais do objeto" << std::endl;
	std::cout << "  K/L           Distancia da luz (+ / -)" << std::endl;
	std::cout << std::endl;
	std::cout << "  ESC           Fechar" << std::endl;
	std::cout << "======================================" << std::endl;
	std::cout << std::endl;
	
	//stbi_set_flip_vertically_on_load(true);
	
	Shader shader("./shaders/vertex.glsl", "./shaders/fragment.glsl", "./shaders/geometry.glsl");
	Shader lightShader("./shaders/lightVertex.glsl", "./shaders/lightFragment.glsl");
	shaderGlobal = &shader;
	shader.use();

	shader.setVec3f("wireframeColor", glm::vec3(0.0f, 1.0f, 0.0f));
	shader.setFloat("wireframeWidth", 0.005f);
	shader.setBool("wireframe", false);
	
	Model lightCube = Model("./assets/Modelos3D/Cube.obj", MAT_JADE, true, light.position);

	modelList.push_back(Model("./assets/Modelos3D/Cube.obj"));
  modelList.push_back(Model("./assets/Modelos3D/Suzanne.obj", MAT_GOLD));
	
	while(!glfwWindowShouldClose(window)){
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		shader.use();
		
		glm::mat4 projection = (orthographic) ? ortho : 
			glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.f);
		glm::mat4 view = camera.GetViewMatrix();
		shader.setMatrix4f("projection", projection);
		shader.setMatrix4f("view", view);
		shader.setVec3f("viewPos", camera.Position);

		for(int i = 0; i < modelList.size(); i++) {
			modelList[i].Draw(shader, light);
		}
		
		lightShader.use();
		lightShader.setMatrix4f("projection", projection);
		lightShader.setMatrix4f("view", view);
		lightCube.Draw(lightShader, light);
		

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}

void processInput(GLFWwindow *window) {
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}

	
	/*
		NumPad Controls
		- (select last Model from modelList) + (select next Model from modelList)
		7 (set Operation TRANSLATE to current Model) 8 9 (Sub X Add X) 
		4 (set Operation ROTATE to current Model)    5 6 (Sub Y Add Y)
		1 (set Operation SCALE to current Model)     2 3 (Sub Z Add Z)
		0 (set Operation SCALE_SIMETRICALLY to current Model)
		. (reset Scaling and Rotation of current Model)
		/ (set Wireframe ON and OFF)
		NM Cycle through materials on the currentModel
		KL Increase Decrease Light Distance
	*/
	
	if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS) {
		modelList[currentModel].moveModel(X_AXIS, -deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_KP_9) == GLFW_PRESS) {
		modelList[currentModel].moveModel(X_AXIS, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS) {
		modelList[currentModel].moveModel(Y_AXIS, -deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS) {
		modelList[currentModel].moveModel(Y_AXIS, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS) {
		modelList[currentModel].moveModel(Z_AXIS, -deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_KP_3) == GLFW_PRESS) {
		modelList[currentModel].moveModel(Z_AXIS, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_KP_7) == GLFW_PRESS) {
		modelList[currentModel].setOperation(TRANSLATE);
	}
	if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS) {
		modelList[currentModel].setOperation(ROTATE);
	}
	if (glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS) {
		modelList[currentModel].setOperation(SCALE);
	}
	if (glfwGetKey(window, GLFW_KEY_KP_0) == GLFW_PRESS) {
		modelList[currentModel].setOperation(SCALE_SIMETRICAL);
	}

	if (glfwGetKey(window, GLFW_KEY_KP_DECIMAL) == GLFW_PRESS) {
		modelList[currentModel].resetModelScaleAndRotation();
	}

}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
	static bool wireframe = false;
	if (key == GLFW_KEY_KP_ADD && action == GLFW_PRESS){
		currentModel = std::min(++currentModel, static_cast<int>(modelList.size() - 1));
		modelList[currentModel].setOperation(TRANSLATE);
		std::cout << currentModel << std::endl;
	}
	if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_PRESS){
		currentModel = std::max(0, --currentModel);
		modelList[currentModel].setOperation(TRANSLATE);
		std::cout << currentModel << std::endl;
	}
	if (key == GLFW_KEY_KP_DIVIDE && action == GLFW_PRESS){
		wireframe ^= 1;
		shaderGlobal->use();
		shaderGlobal->setBool("wireframe", wireframe);
	}
	if (key == GLFW_KEY_P && action == GLFW_PRESS){
		orthographic ^= 1;
	}
	if (key == GLFW_KEY_M && action == GLFW_PRESS){
		modelList[currentModel].cycleMaterial(true);
	}
	if (key == GLFW_KEY_N && action == GLFW_PRESS){
		modelList[currentModel].cycleMaterial(false);
	}
	if (key == GLFW_KEY_L && action == GLFW_PRESS){
		currentLightDist = std::min(++currentLightDist, static_cast<int>(LIGHT_DISTANCES.size() - 1));
		light.distance = LIGHT_DISTANCES[currentLightDist];
	}
	if (key == GLFW_KEY_K && action == GLFW_PRESS){
		currentLightDist = std::max(0, --currentLightDist);
		light.distance = LIGHT_DISTANCES[currentLightDist];
	}
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn){
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);
	
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

