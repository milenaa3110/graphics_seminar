//

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

#include <glm/glm.hpp>

using namespace std;


static int winWidth = 800, winHeight = 800;
static int VERSION = 4;

static float cameraX = 0.0f, cameraY = 3.0f, cameraZ = 10.0f;
static float rotationAngle = 0.0f;


static float fogColor[3] = {0.7f, 0.7f, 0.8f}; 
static float fogDensity = 0.15f;
static int fogType = 0;  


static double centerX = -0.75, centerY = 0.0;
static double scale = 1.5;
static double dragStartX = 0, dragStartY = 0;
static double dragCenterX = 0, dragCenterY = 0;
static bool dragging = false;
static double aspect = 1.0;


struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec4 color;
    float size;
    float life;      
    float maxLife;   
};

const int MAX_PARTICLES = 1000;
static vector<Particle> particles(MAX_PARTICLES);
static GLuint particleTexture;


inline void initParticles() {
    std::random_device rd;
    std::mt19937 gen(rd());

    
    std::uniform_real_distribution<float> xDist(-3.0f, 3.0f);
    std::uniform_real_distribution<float> yDist(2.0f, 4.0f);
    std::uniform_real_distribution<float> zDist(-3.0f, 3.0f);

    std::uniform_real_distribution<float> sizeDist(0.2f, 0.5f);
    std::uniform_real_distribution<float> lifeDist(10.0f, 20.0f);

    for (int i = 0; i < MAX_PARTICLES; i++) {
        float life = lifeDist(gen);
        particles[i].position = glm::vec3(xDist(gen), yDist(gen), zDist(gen));
        particles[i].velocity = glm::vec3(0.01f * xDist(gen) / 3.0f, 0.0f, 0.01f * zDist(gen) / 3.0f); 
        particles[i].color = glm::vec4(0.9f, 0.9f, 0.9f, 0.7f); 
        particles[i].size = sizeDist(gen);
        particles[i].life = life;
        particles[i].maxLife = life;
    }
}


inline void updateParticles(float deltaTime) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> xDist(-3.0f, 3.0f);
    std::uniform_real_distribution<float> yDist(2.0f, 4.0f);
    std::uniform_real_distribution<float> zDist(-3.0f, 3.0f);
    std::uniform_real_distribution<float> lifeDist(10.0f, 20.0f);

    for (int i = 0; i < MAX_PARTICLES; i++) {
        particles[i].life -= deltaTime;

        
        if (particles[i].life <= 0.0f) {
            float life = lifeDist(gen);
            particles[i].position = glm::vec3(xDist(gen), yDist(gen), zDist(gen));
            particles[i].velocity = glm::vec3(0.01f * xDist(gen) / 3.0f, 0.0f, 0.01f * zDist(gen) / 3.0f);
            particles[i].life = life;
            particles[i].maxLife = life;
        }

        
        particles[i].position += particles[i].velocity * deltaTime;

        
        if (particles[i].position.x < -5.0f || particles[i].position.x > 5.0f ||
            particles[i].position.z < -5.0f || particles[i].position.z > 5.0f ||
            particles[i].position.y < 1.0f || particles[i].position.y > 5.0f) {

            particles[i].velocity = -particles[i].velocity;
        }
    }
}


inline GLuint loadTexture(const char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    
    const int width = 64, height = 64;
    unsigned char* data = new unsigned char[width * height * 4];

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float dx = x / (float)width - 0.5f;
            float dy = y / (float)height - 0.5f;
            float distance = sqrt(dx*dx + dy*dy);

            
            float alpha = std::max(0.0f, 1.0f - distance * 2.0f);
            alpha *= (0.7f + 0.3f * dist(gen)); 

            data[(y * width + x) * 4 + 0] = 255; 
            data[(y * width + x) * 4 + 1] = 255; 
            data[(y * width + x) * 4 + 2] = 255; 
            data[(y * width + x) * 4 + 3] = (unsigned char)(alpha * 255); 
        }
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    delete[] data;

    return textureID;
}


inline void displayFogControls() {
    
    

    
    cout << "Fog type: " << (fogType == 0 ? "Linear" : (fogType == 1 ? "Exponential" : "Exponential Squared")) << endl;
    cout << "Fog density: " << fogDensity << endl;
    cout << "Fog color: (" << fogColor[0] << ", " << fogColor[1] << ", " << fogColor[2] << ")" << endl;
}

inline GLFWwindow *window_preset() {
    
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, VERSION);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, VERSION);

    
    
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    
    auto *window = glfwCreateWindow(800, 800, "OpenGL Fog Demo", nullptr, nullptr);

    
    if (window == nullptr) {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        exit(1);
    }

    
    glfwMakeContextCurrent(window);

    
    glfwSwapInterval(1);

    
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


#endif 
