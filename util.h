//
// Created by avasilic on 9/30/2025.
//

#ifndef GRAPHICS_HOMEWORK_1_UTIL_H
#define GRAPHICS_HOMEWORK_1_UTIL_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <random>

// Include GLM for matrices and vectors
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

// Window parameters
static int winWidth = 800, winHeight = 800;
static int VERSION = 4;
// Camera parameters
static float cameraX = 0.0f, cameraY = 3.0f, cameraZ = 10.0f;
static float rotationAngle = 0.0f;

// Fog parameters
static float fogColor[3] = {0.7f, 0.7f, 0.8f}; // Light blue-gray fog
static float fogDensity = 0.15f;
static int fogType = 0;  // 0 = linear, 1 = exp, 2 = exp2

// Mouse interaction
static double centerX = -0.75, centerY = 0.0;
static double scale = 1.5;
static double dragStartX = 0, dragStartY = 0;
static double dragCenterX = 0, dragCenterY = 0;
static bool dragging = false;
static double aspect = 1.0;

// Particle system parameters
struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec4 color;
    float size;
    float life;      // Remaining life of the particle
    float maxLife;   // Maximum life of the particle
};

const int MAX_PARTICLES = 1000;
static vector<Particle> particles(MAX_PARTICLES);
static GLuint particleTexture;

// Function to initialize particles for cloud
inline void initParticles() {
    std::random_device rd;
    std::mt19937 gen(rd());

    // Cloud dimensions
    std::uniform_real_distribution<float> xDist(-3.0f, 3.0f);
    std::uniform_real_distribution<float> yDist(2.0f, 4.0f);
    std::uniform_real_distribution<float> zDist(-3.0f, 3.0f);

    std::uniform_real_distribution<float> sizeDist(0.2f, 0.5f);
    std::uniform_real_distribution<float> lifeDist(10.0f, 20.0f);

    for (int i = 0; i < MAX_PARTICLES; i++) {
        float life = lifeDist(gen);
        particles[i].position = glm::vec3(xDist(gen), yDist(gen), zDist(gen));
        particles[i].velocity = glm::vec3(0.01f * xDist(gen) / 3.0f, 0.0f, 0.01f * zDist(gen) / 3.0f); // Slow movement
        particles[i].color = glm::vec4(0.9f, 0.9f, 0.9f, 0.7f); // White-ish with transparency
        particles[i].size = sizeDist(gen);
        particles[i].life = life;
        particles[i].maxLife = life;
    }
}

// Function to update particles for animation
inline void updateParticles(float deltaTime) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> xDist(-3.0f, 3.0f);
    std::uniform_real_distribution<float> yDist(2.0f, 4.0f);
    std::uniform_real_distribution<float> zDist(-3.0f, 3.0f);
    std::uniform_real_distribution<float> lifeDist(10.0f, 20.0f);

    for (int i = 0; i < MAX_PARTICLES; i++) {
        particles[i].life -= deltaTime;

        // Reset dead particles
        if (particles[i].life <= 0.0f) {
            float life = lifeDist(gen);
            particles[i].position = glm::vec3(xDist(gen), yDist(gen), zDist(gen));
            particles[i].velocity = glm::vec3(0.01f * xDist(gen) / 3.0f, 0.0f, 0.01f * zDist(gen) / 3.0f);
            particles[i].life = life;
            particles[i].maxLife = life;
        }

        // Update position
        particles[i].position += particles[i].velocity * deltaTime;

        // Ensure particles stay within the cloud boundary
        if (particles[i].position.x < -5.0f || particles[i].position.x > 5.0f ||
            particles[i].position.z < -5.0f || particles[i].position.z > 5.0f ||
            particles[i].position.y < 1.0f || particles[i].position.y > 5.0f) {

            particles[i].velocity = -particles[i].velocity;
        }
    }
}

// Function to load a texture
inline GLuint loadTexture(const char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // For this example, we'll create a simple cloud-like texture programmatically
    const int width = 64, height = 64;
    unsigned char* data = new unsigned char[width * height * 4];

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    // Generate a simple cloud-like texture
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float dx = x / (float)width - 0.5f;
            float dy = y / (float)height - 0.5f;
            float distance = sqrt(dx*dx + dy*dy);

            // Create a radial gradient with some noise
            float alpha = std::max(0.0f, 1.0f - distance * 2.0f);
            alpha *= (0.7f + 0.3f * dist(gen)); // Add some noise

            data[(y * width + x) * 4 + 0] = 255; // R
            data[(y * width + x) * 4 + 1] = 255; // G
            data[(y * width + x) * 4 + 2] = 255; // B
            data[(y * width + x) * 4 + 3] = (unsigned char)(alpha * 255); // A
        }
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    delete[] data;

    return textureID;
}

// Function to display fog controls as ImGui window
inline void displayFogControls() {
    // This is a placeholder - in a real implementation, you would use ImGui or a similar library
    // to create a UI for controlling fog parameters

    // For now, just print current fog parameters to console
    cout << "Fog type: " << (fogType == 0 ? "Linear" : (fogType == 1 ? "Exponential" : "Exponential Squared")) << endl;
    cout << "Fog density: " << fogDensity << endl;
    cout << "Fog color: (" << fogColor[0] << ", " << fogColor[1] << ", " << fogColor[2] << ")" << endl;
}

inline GLFWwindow *window_preset() {
    // Initialize GLFW
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, VERSION);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, VERSION);

    // Tell GLFW we are using the CORE profile
    // So that means we only have the modern functions
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
    auto *window = glfwCreateWindow(800, 800, "OpenGL Fog Demo", nullptr, nullptr);

    // Error check if the window fails to create
    if (window == nullptr) {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        exit(1);
    }

    // Introduce the window into the current context
    glfwMakeContextCurrent(window);

    // Enable vsync
    glfwSwapInterval(1);

    //Load GLAD so it configures OpenGL
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        cerr << "Failed to initialize GLAD" << "\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    };

    const GLubyte *version = glGetString(GL_VERSION);
    cout << "OpenGL version: " << version << "\n";

    return window;
}

inline string load_shader_source(const char *filePath) {
    const ifstream file(filePath, ios::in | ios::binary);
    if (!file) {
        cerr << "Failed to open shader file: " << filePath << endl;
        return "";
    }
    ostringstream contents;
    contents << file.rdbuf();
    return contents.str();
}


#endif //GRAPHICS_HOMEWORK_1_UTIL_H
