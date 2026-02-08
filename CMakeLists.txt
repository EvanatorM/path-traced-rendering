cmake_minimum_required(VERSION 3.24)
project(PathTracer LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

macro(_add_engine_git_module NAME REPO COMMIT)
  message(STATUS "Module: ${NAME} @ ${COMMIT}")
  FetchContent_Declare(${NAME}
    GIT_REPOSITORY ${REPO}
    GIT_TAG        ${COMMIT}      # exact commit from lockfile
    GIT_SHALLOW    TRUE
    UPDATE_DISCONNECTED FALSE
  )
  FetchContent_MakeAvailable(${NAME})
endmacro()

# Add glad
_add_engine_git_module(glad https://github.com/Dav1dde/glad.git 5bf3eda)

add_executable(PathTracer
    src/main.cpp
    src/PathTracer.cpp
    src/Renderer.cpp
    src/Window.cpp

    ${glad_SOURCE_DIR}/src/glad.c
)

target_include_directories(PathTracer PRIVATE include)

_add_engine_git_module(glm https://github.com/g-truc/glm.git 1.0.1)
target_include_directories(PathTracer PRIVATE ${glm_SOURCE_DIR})
target_link_libraries(PathTracer PRIVATE glm::glm)

# Finish adding glad
target_include_directories(PathTracer PUBLIC ${glad_SOURCE_DIR}/include)

# Add GLFW
_add_engine_git_module(GLFW https://github.com/glfw/glfw.git 3.4)
target_link_libraries(PathTracer PUBLIC glfw)
target_include_directories(PathTracer PUBLIC ${glfw_SOURCE_DIR}/include)