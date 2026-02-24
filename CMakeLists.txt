cmake_minimum_required(VERSION 3.24)
project(PathTracer LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

macro(_fetch_git_project NAME REPO COMMIT)
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
_fetch_git_project(glad https://github.com/Dav1dde/glad.git 5bf3eda)

add_executable(PathTracer
    src/Camera.cpp
    src/ComputeShader.cpp
    src/main.cpp
    src/Mesh.cpp
    src/PathTracer.cpp
    src/Plane.cpp
    src/Renderer.cpp
    src/Shader.cpp
    src/UIManager.cpp
    src/Window.cpp

    ${glad_SOURCE_DIR}/src/glad.c
)

target_include_directories(PathTracer PRIVATE include)

_fetch_git_project(glm https://github.com/g-truc/glm.git 1.0.1)
target_include_directories(PathTracer PRIVATE ${glm_SOURCE_DIR})
target_link_libraries(PathTracer PRIVATE glm::glm)

# Finish adding glad
target_include_directories(PathTracer PUBLIC ${glad_SOURCE_DIR}/include)

# Add GLFW
_fetch_git_project(GLFW https://github.com/glfw/glfw.git 3.4)
target_link_libraries(PathTracer PUBLIC glfw)
target_include_directories(PathTracer PUBLIC ${glfw_SOURCE_DIR}/include)

# Add ImGUI
_fetch_git_project(imgui https://github.com/ocornut/imgui.git v1.92.4)

set(IMGUI_SOURCES
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC
  ${imgui_SOURCE_DIR}
  ${imgui_SOURCE_DIR}/misc/cpp
  ${imgui_SOURCE_DIR}/backends
)

add_library(imgui::imgui ALIAS imgui)
set_target_properties(imgui PROPERTIES POSITION_INDEPENDENT_CODE ON)

set(IMGUI_BACKEND_DIR ${imgui_SOURCE_DIR}/backends)

add_library(imgui_backend
  ${IMGUI_BACKEND_DIR}/imgui_impl_glfw.cpp
  ${IMGUI_BACKEND_DIR}/imgui_impl_opengl3.cpp
)

target_include_directories(imgui_backend PUBLIC
  ${IMGUI_BACKEND_DIR}
  ${imgui_SOURCE_DIR}
  ${imgui_SOURCE_DIR}/misc/cpp
)

find_package(OpenGL REQUIRED)
target_link_libraries(imgui_backend PUBLIC
  imgui::imgui
  glfw
  OpenGL::GL
)

target_link_libraries(${PROJECT_NAME} PRIVATE imgui_backend)
target_include_directories(${PROJECT_NAME} PRIVATE ${imgui_backend_SOURCE_DIR})

# Copy assets folder to build folder
add_custom_command(
  TARGET PathTracer POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory 
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:PathTracer>/assets
  COMMENT "Copying assets"
)