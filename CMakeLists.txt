cmake_minimum_required(VERSION 3.16)
project(graphics_seminar_cloud LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include the FetchContent module
include(FetchContent)

# Fetch GLM (OpenGL Mathematics) library
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 0af55ccecd98d4e5a8d1fad7de25ba429d60e863 # refs/tags/1.0.1
)
FetchContent_MakeAvailable(glm)

# GLFW
add_subdirectory(glfw)

# GLAD
add_library(glad STATIC glad/src/glad.c)
target_include_directories(glad PUBLIC glad/include)

# Executable
add_executable(main main.cpp
        util.h)
target_include_directories(main PRIVATE glad/include glfw/include)
target_link_libraries(main PRIVATE glad glfw glm::glm)
