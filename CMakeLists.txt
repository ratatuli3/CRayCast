cmake_minimum_required(VERSION 3.10.0)
project(CRayCast)

add_executable(${PROJECT_NAME} main.c)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Find GLUT (FreeGLUT)
find_package(GLUT REQUIRED)

# Manually set the path to GLU (if not found by CMake)
find_library(GLU_LIBRARY NAMES GLU libGLU)

# Link OpenGL and GLUT to your project
target_link_libraries(${PROJECT_NAME} OpenGL::GL GLUT::GLUT ${GLU_LIBRARY})
