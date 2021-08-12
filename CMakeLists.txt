cmake_minimum_required(VERSION 3.0.0)
project(OpenGLProject VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES src/main.cpp src/mesh.cpp include/camera.h include/mesh.h include/shader.h include/model.h src/model.cpp) 
add_executable(OpenGLProject ${SOURCE_FILES} src/glad.c)

target_include_directories(OpenGLProject PRIVATE ${PROJECT_SOURCE_DIR}/include ${ASSIMP_INCLUDE_DIR})
target_link_libraries(OpenGLProject C:/lib/VS2017/glfw/build/src/Release/glfw3.lib C:/lib/VS2017/assimp-5.0.1/build/code/Release/assimp-vc141-mt.lib)