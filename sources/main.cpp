#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "debug.h"
#include "shader.h"
#include "camera.h"
#include "model.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// Window settings
const unsigned int SCR_WIDTH  = 1280;
const unsigned int SCR_HEIGHT = 720;

// Debug tools
Debug debug;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
float lastX     = SCR_WIDTH / 2.0f;
float lastY     = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool blocked    = false;

// Light
bool spotLight = true;

// Timing
double deltaTime = 0.0f;
double lastFrame = 0.0f;

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  GLFWwindow *window =
      glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  // GLFW capture mouse
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // GLAD: load all OpenGL function pointers
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  GLint nrAttributes;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
  std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes
            << std::endl;

  // Configure global opengl state
  glEnable(GL_DEPTH_TEST);

  // Compile shaders
  Shader lightingShader("6.multiple_lights.vs", "6.multiple_lights.fs");
  Shader lampShader("6.lamp.vs", "6.lamp.fs");

  // Load models
  // Model ourModel("resources/objects/nanosuit/nanosuit.obj");
  Model block("resources/objects/block_spec/block_spec.obj");
  // Model ourModel("resources/objects/log/log.obj");
  // Model ourModel("resources/objects/diamond/diamond.obj");
  // Model ourModel("resources/objects/test/de_dust2.obj");
  Model ourModel("resources/objects/de_dust2/de_dust2.obj");
  Model lamp("resources/objects/lamp/lamp.obj");

  // set up vertex data (and buffer(s)) and configure vertex attributes
  // positions all containers
  glm::vec3 cubePositions[] = {glm::vec3(0.0f, 0.0f, 0.0f),
                               glm::vec3(2.0f, 5.0f, -15.0f),
                               glm::vec3(-1.5f, -2.2f, -2.5f),
                               glm::vec3(-3.8f, -2.0f, -12.3f),
                               glm::vec3(2.4f, -0.4f, -3.5f),
                               glm::vec3(-1.7f, 3.0f, -7.5f),
                               glm::vec3(1.3f, -2.0f, -2.5f),
                               glm::vec3(1.5f, 2.0f, -2.5f),
                               glm::vec3(1.5f, 0.2f, -1.5f),
                               glm::vec3(-1.3f, 1.0f, -1.5f)};
  // positions of the point lights
  glm::vec3 pointLightPositions[] = {glm::vec3(0.7f, 0.2f, 2.0f),
                                     glm::vec3(2.3f, -3.3f, -4.0f),
                                     glm::vec3(-4.0f, 2.0f, -12.0f),
                                     glm::vec3(0.0f, 0.0f, -3.0f)};

  // shader configuration
  lightingShader.use();
  lightingShader.setInt("material.diffuse", 0);
  lightingShader.setInt("material.specular", 1);

  // Main loop
  while (!glfwWindowShouldClose(window)) {
    // Time logic
    double currentFrame = glfwGetTime();
    deltaTime           = currentFrame - lastFrame;
    lastFrame           = currentFrame;

    // Input
    processInput(window);

    // Debug tools
    debug.process();

    // Render
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // be sure to activate shader when setting uniforms/drawing objects
    lightingShader.use();
    lightingShader.setVec3("viewPos", camera.Position);
    lightingShader.setFloat("material.shininess", 32.0f);

    // view/projection transformations
    glm::mat4 projection =
        glm::perspective(glm::radians(camera.Zoom),
                         (float)SCR_WIDTH / (float)SCR_HEIGHT,
                         0.1f,
                         100.0f);
    lightingShader.setMat4("projection", projection);

    glm::mat4 view = camera.GetViewMatrix();
    lightingShader.setMat4("view", view);

    // directional light
    lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
    // point light 1
    lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
    lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
    lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
    lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
    lightingShader.setFloat("pointLights[0].constant", 1.0f);
    lightingShader.setFloat("pointLights[0].linear", 0.09f);
    lightingShader.setFloat("pointLights[0].quadratic", 0.032f);
    // point light 2
    lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
    lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
    lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
    lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
    lightingShader.setFloat("pointLights[1].constant", 1.0f);
    lightingShader.setFloat("pointLights[1].linear", 0.09f);
    lightingShader.setFloat("pointLights[1].quadratic", 0.032f);
    // point light 3
    lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
    lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
    lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
    lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
    lightingShader.setFloat("pointLights[2].constant", 1.0f);
    lightingShader.setFloat("pointLights[2].linear", 0.09f);
    lightingShader.setFloat("pointLights[2].quadratic", 0.032f);
    // point light 4
    lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
    lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
    lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
    lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
    lightingShader.setFloat("pointLights[3].constant", 1.0f);
    lightingShader.setFloat("pointLights[3].linear", 0.09f);
    lightingShader.setFloat("pointLights[3].quadratic", 0.032f);
    // spotLight

    lightingShader.setVec3("spotLight.position", camera.Position);
    lightingShader.setVec3("spotLight.direction", camera.Front);
    lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
    lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
    lightingShader.setFloat("spotLight.constant", 1.0f);
    lightingShader.setFloat("spotLight.linear", 0.09f);
    lightingShader.setFloat("spotLight.quadratic", 0.032f);
    lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    lightingShader.setFloat("spotLight.outerCutOff",
                            glm::cos(glm::radians(15.0f)));
    lightingShader.setFloat("spotLight.enabled", spotLight);

    // render containers
    for (unsigned int i = 0; i < 10; i++) {
      // calculate the model matrix for each object and pass it to shader before
      // drawing
      glm::mat4 model = glm::mat4(1.0f);
      model =
          glm::translate(model, cubePositions[i]); // translate it down so it's
                                                   // at the center of the scene
      model = glm::scale(
          model,
          glm::vec3(
              0.4f)); // it's a bit too big for our scene, so scale it down
      float angle = 20.0f * i;
      model =
          glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
      lightingShader.setMat4("model", model);

      // block.Draw(lightingShader);
    }

    // we now draw as many light bulbs as we have point lights.
    for (unsigned int i = 0; i < 4; i++) {
      // also draw the lamp object(s)
      lampShader.use();
      lampShader.setMat4("projection", projection);
      lampShader.setMat4("view", view);

      glm::mat4 model = glm::mat4(1.0f);
      model           = glm::translate(model, pointLightPositions[i]);
      model = glm::scale(model, glm::vec3(0.4f)); // Make it a smaller cube
      lampShader.setMat4("model", model);

      lamp.Draw(lampShader);
    }

    // don't forget to enable shader before setting uniforms
    lightingShader.use();

    // render the loaded model
    glm::mat4 model = glm::mat4(1.0f);
    model           = glm::translate(
        model,
        glm::vec3(
            0.0f,
            -1.75f,
            -2.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(
        model,
        glm::vec3(0.01f,
                  0.01f,
                  0.01f)); // it's a bit too big for our scene, so scale it down
    model =
        glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    lightingShader.setMat4("model", model);

    ourModel.Draw(lightingShader);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
void processInput(GLFWwindow *window) {
  static int polygonModeOld = GLFW_RELEASE;
  int polygonModeNew        = glfwGetKey(window, GLFW_KEY_KP_0);
  if (polygonModeNew == GLFW_RELEASE && polygonModeOld == GLFW_PRESS) {
    debug.polygonMode = !debug.polygonMode;
  }
  polygonModeOld = polygonModeNew;

  static int spotLightModeOld = GLFW_RELEASE;
  int spotLightModeNew        = glfwGetKey(window, GLFW_KEY_L);
  if (spotLightModeNew == GLFW_RELEASE && spotLightModeOld == GLFW_PRESS) {
    spotLight = !spotLight;
  }
  spotLightModeOld = spotLightModeNew;

  static int cursorModeOld = GLFW_RELEASE;
  int cursorModeNew        = glfwGetKey(window, GLFW_KEY_I);
  if (cursorModeNew == GLFW_RELEASE && cursorModeOld == GLFW_PRESS) {
    blocked = !blocked;
  }
  cursorModeOld = cursorModeNew;

  if (blocked) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  } else {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.ProcessKeyboard(FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.ProcessKeyboard(BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.ProcessKeyboard(LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  // make sure the viewport matches the new window dimensions; note that width
  // and height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  if (blocked) {
    return;
  }

  if (firstMouse) {
    lastX      = (float)xpos;
    lastY      = (float)ypos;
    firstMouse = false;
  }

  float xoffset = (float)xpos - lastX;
  float yoffset =
      lastY - (float)ypos; // reversed since y-coordinates go from bottom to top

  lastX = (float)xpos;
  lastY = (float)ypos;

  camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  camera.ProcessMouseScroll((float)yoffset);
}