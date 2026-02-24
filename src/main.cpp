#include <Scene.h>
#include <Sphere.h>
#include <Plane.h>
#include <Image.h>
#include <PathTracer.h>
#include <Renderer.h>
#include <Window.h>
#include <imgui.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ComputeShader.h>
#include <Shader.h>
#include <UIManager.h>
#include <Mesh.h>

void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

Camera* camera;

bool paused = false;
bool pathTraced = true;

int main()
{
    Scene scene;
    scene.AddPlane(Plane(glm::vec3(0.0f, -3.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.5f)));
    scene.AddSphere(Sphere(glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0.25f));
    scene.AddSphere(Sphere(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(1.0f, 0.2f, 0.2f), 1.0f));
    scene.AddSphere(Sphere(glm::vec3(1.0f, 0.0f, -8.0f), glm::vec3(0.2f, 0.2f, 1.0f), 1.0f));
    scene.AddSphere(Sphere(glm::vec3(-1.0f, -1.0f, -8.0f), glm::vec3(0.2f, 1.0f, 0.2f), 0.5f));


    const unsigned int TEXTURE_WIDTH = 1920, TEXTURE_HEIGHT = 1080;

    // Create window
    Renderer::Init();
    Window::InitWindow(TEXTURE_WIDTH, TEXTURE_HEIGHT, "Path Traced Renderer");
    auto& window = Window::GetInstance();
    glfwSetCursorPosCallback(window.GetWindow(), mouseCallback);
    glfwSetKeyCallback(window.GetWindow(), keyCallback);

    camera = new Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -90.0f, 0.0f), 70.0f, glm::vec3(0.1f));

    // Create test texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);

    glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    
    ComputeShader computeShader("assets/shaders/compute_shader.glsl");

    // Create quad for displaying the texture
    Vertex vertices[] = {
        // positions   // texCoords
        glm::vec3(-1.0f, -1.0f, 0.0f),  glm::vec2(0.0f, 1.0f),
        glm::vec3( 1.0f, -1.0f, 0.0f),  glm::vec2(1.0f, 1.0f),
        glm::vec3(-1.0f,  1.0f, 0.0f),  glm::vec2(0.0f, 0.0f),
        glm::vec3( 1.0f,  1.0f, 0.0f),  glm::vec2(1.0f, 0.0f)
    };
    uint32_t indices[] = {
        0, 2, 1,
        1, 2, 3
    };
    Mesh screenMesh(vertices, 4, indices, 6);

    // Load shader
    Shader shader("assets/shaders/quad.vert", "assets/shaders/quad.frag");
    shader.SetInt("tex", 0);

    // Create Path Tracer
    PathTracer pathTracer(scene, computeShader);

    // Initialize ImGUI
    UIManager::InitImGUI();

    // Lock mouse
    glfwSetInputMode(window.GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    float deltaTime = 0;
    float lastFrame = 0;

    float previousTime = 0;
    int frameCount = 0;
    int fps = 0;

    while (!window.ShouldClose())
    {
        // Set frame time
        float currentFrame = Renderer::GetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Get FPS
        frameCount++;
        if (currentFrame - previousTime >= 1.0f)
        {
            fps = frameCount;
            frameCount = 0;
            previousTime = currentFrame;
        }

        // Run update logic
        float movementSpeed = 5.0f;
        if (glfwGetKey(window.GetWindow(), GLFW_KEY_W))
            camera->position += camera->Front() * movementSpeed * deltaTime;
        if (glfwGetKey(window.GetWindow(), GLFW_KEY_S))
            camera->position -= camera->Front() * movementSpeed * deltaTime;
        if (glfwGetKey(window.GetWindow(), GLFW_KEY_A))
            camera->position -= camera->Right() * movementSpeed * deltaTime;
        if (glfwGetKey(window.GetWindow(), GLFW_KEY_D))
            camera->position += camera->Right() * movementSpeed * deltaTime;
        if (glfwGetKey(window.GetWindow(), GLFW_KEY_E))
            camera->position += camera->Up() * movementSpeed * deltaTime;
        if (glfwGetKey(window.GetWindow(), GLFW_KEY_Q))
            camera->position -= camera->Up() * movementSpeed * deltaTime;

        // Clear window
        window.Clear();

        // Run path tracer
        if (pathTraced)
        {
            pathTracer.PathTrace(*camera, TEXTURE_WIDTH, TEXTURE_HEIGHT);

            // Render quad
            shader.Bind();
            glBindTexture(GL_TEXTURE_2D, texture);
            screenMesh.Draw();
        }
        else
        {
            window.SetBackgroundColor(camera->backgroundColor.r, camera->backgroundColor.g, camera->backgroundColor.b, 1.0f);

            auto view = camera->GetViewMatrix();
            auto proj = camera->GetProjectionMatrix();
            for (auto& plane : scene.GetPlanes())
            {
                plane.RenderRaster(view, proj);
            }
            for (auto& sphere : scene.GetSpheres())
            {
                sphere.RenderRaster(view, proj);
            }
        }

        // Render UI
        UIManager::BeginFrame();

        ImGui::Text("FPS: %d", fps);
        ImGui::Text("Pos: %f, %f, %f", camera->position.x, camera->position.y, camera->position.z);
        ImGui::Text("Dir: %f, %f, %f", camera->direction.x, camera->direction.y, camera->direction.z);
        ImGui::Checkbox("Path Traced", &pathTraced);

        UIManager::EndFrame();

        // End-of-frame logic
        window.SwapBuffers();
        window.PollEvents();
    }

    Renderer::Shutdown();
}

float lastX = 0, lastY = 0;
bool firstFrame = true;

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    if (firstFrame)
    {
        firstFrame = false;
        return;
    }

    if (paused) return;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    camera->direction.y += xoffset;
    camera->direction.x += yoffset;

    if(camera->direction.x > 89.0f)
        camera->direction.x = 89.0f;
    if(camera->direction.x < -89.0f)
        camera->direction.x = -89.0f;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        paused = !paused;
        if (paused)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}