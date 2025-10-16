#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h" 
#include "util.h" 
#include <iostream>
#include <vector>
#include <random>
#include <algorithm> 
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 8.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

const int NUM_PARTICLES = 1000;
const float CLOUD_SIZE = 4.0f;
const float PARTICLE_SIZE_MIN = 0.2f;
const float PARTICLE_SIZE_MAX = 0.5f;

struct CloudParticle {
    glm::vec3 position;
    float size;
    glm::vec3 color;
};


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
std::vector<CloudParticle> generateCloud();
unsigned int setupCloudVAO();

int main() {
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Cloud Generation", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader cloudShader("../shaders/vertex.glsl", "../shaders/fragment.glsl");

    
    std::vector<CloudParticle> cloudParticles = generateCloud();

    
    unsigned int cloudVAO = setupCloudVAO();

    
    unsigned int cloudTexture = loadTexture(nullptr); 

    
    while (!glfwWindowShouldClose(window)) {
        
        processInput(window);

        
        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        
        cloudShader.use();
        cloudShader.setMat4("view", view);
        cloudShader.setMat4("projection", projection);

        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cloudTexture);
        cloudShader.setInt("cloudMask", 0);

        
        std::sort(cloudParticles.begin(), cloudParticles.end(),
            [](const CloudParticle& a, const CloudParticle& b) {
                float distA = glm::dot(a.position - cameraPos, a.position - cameraPos);
                float distB = glm::dot(b.position - cameraPos, b.position - cameraPos);
                return distA > distB; 
            }
        );

        
        glBindVertexArray(cloudVAO);
        for (const auto& particle : cloudParticles) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, particle.position);
            model = glm::scale(model, glm::vec3(particle.size));
            cloudShader.setMat4("model", model);
            cloudShader.setVec3("particleColor", particle.color);

            
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
        glBindVertexArray(0);

        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    
    glDeleteVertexArrays(1, &cloudVAO);
    glfwTerminate();
    return 0;
}

std::vector<CloudParticle> generateCloud() {
    std::vector<CloudParticle> particles;
    std::random_device rd;
    std::mt19937 gen(rd());

    
    std::uniform_real_distribution<> radiusDist(0.0f, CLOUD_SIZE * 0.5f);
    std::uniform_real_distribution<> angleDist(0.0f, glm::two_pi<float>());
    std::uniform_real_distribution<> heightDist(-CLOUD_SIZE * 0.2f, CLOUD_SIZE * 0.2f);
    std::uniform_real_distribution<> sizeDist(PARTICLE_SIZE_MIN, PARTICLE_SIZE_MAX);

    for (int i = 0; i < NUM_PARTICLES; i++) {
        CloudParticle particle;

        
        float radius = radiusDist(gen);
        float angle = angleDist(gen);
        float height = heightDist(gen);

        particle.position.x = radius * cos(angle);
        particle.position.z = radius * sin(angle);
        particle.position.y = height + (CLOUD_SIZE * 0.2f); 

        
        particle.size = sizeDist(gen);

        
        float colorVar = 0.8f + (static_cast<float>(rand()) / RAND_MAX) * 0.2f;
        particle.color = glm::vec3(colorVar, colorVar, colorVar);

        particles.push_back(particle);
    }

    return particles;
}

unsigned int setupCloudVAO() {
    unsigned int cloudVAO, cloudVBO;

    
    float quadVertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f,
         0.5f,  0.5f, 0.0f
    };

    glGenVertexArrays(1, &cloudVAO);
    glGenBuffers(1, &cloudVBO);

    glBindVertexArray(cloudVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cloudVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    return cloudVAO;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}
