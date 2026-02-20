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

int main()
{
    Scene scene;
    scene.AddPlane(Plane(glm::vec3(0.0f, -3.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.5f)));
    scene.AddSphere(Sphere(glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0.25f));
    scene.AddSphere(Sphere(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(1.0f, 0.0f, 0.0f), 1.0f));
    scene.AddSphere(Sphere(glm::vec3(1.0f, 0.0f, -8.0f), glm::vec3(0.0f, 0.0f, 1.0f), 1.0f));
    scene.AddSphere(Sphere(glm::vec3(-1.0f, -1.0f, -8.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.5f));

    Camera camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.1f));

    const unsigned int TEXTURE_WIDTH = 1024, TEXTURE_HEIGHT = 1024;

    // Create window
    Renderer::Init();
    Window::InitWindow(TEXTURE_WIDTH, TEXTURE_HEIGHT, "Path Traced Renderer");
    auto& window = Window::GetInstance();

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
    float vertices[] = {
        // positions   // texCoords
        -1.0f, -1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  0.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 0.0f
    };
    unsigned int indices[] = {
        0, 2, 1,
        1, 2, 3
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Load shader
    Shader shader("assets/shaders/quad.vert", "assets/shaders/quad.frag");
    shader.SetInt("tex", 0);

    // Create Path Tracer
    PathTracer pathTracer(scene, computeShader);

    // Initialize ImGUI
    UIManager::InitImGUI();

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

        // Clear window
        window.Clear();

        // Run path tracer
        pathTracer.PathTrace(camera, TEXTURE_WIDTH, TEXTURE_HEIGHT);

        // Render quad
        shader.Bind();
        glBindVertexArray(VAO);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Render UI
        UIManager::BeginFrame();

        ImGui::Text("FPS: %d", fps);

        UIManager::EndFrame();

        // End-of-frame logic
        window.SwapBuffers();
        window.PollEvents();
    }

    Renderer::Shutdown();
}