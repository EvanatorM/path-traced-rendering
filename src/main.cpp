#include <Scene.h>
#include <Sphere.h>
#include <Plane.h>
#include <Image.h>
#include <PathTracer.h>
#include <Renderer.h>
#include <Window.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ComputeShader.h>
#include <Shader.h>

int main()
{
    Scene scene;
    scene.AddObject(new Plane(glm::vec3(0.0f, -3.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f)));
    scene.AddObject(new Sphere(glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0.25f));
    scene.AddObject(new Sphere(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(1.0f, 0.0f, 0.0f), 1.0f));
    scene.AddObject(new Sphere(glm::vec3(1.0f, 0.0f, -8.0f), glm::vec3(0.0f, 0.0f, 1.0f), 1.0f));
    scene.AddObject(new Sphere(glm::vec3(-1.0f, -1.0f, -8.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.5f));

    Camera camera;

    Image image(1280, 720);
    PathTracer::PathTrace(scene, camera, image);
    image.SaveToPPM("output.ppm");

    // Create window
    Renderer::Init();
    Window::InitWindow(512, 512, "Path Traced Renderer");
    auto& window = Window::GetInstance();

    // Create test texture
    const unsigned int TEXTURE_WIDTH = 512, TEXTURE_HEIGHT = 512;
    
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
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 1.0f
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

    while (!window.ShouldClose())
    {
        // Clear window
        window.Clear();

        // Run compute shader
        computeShader.Bind();
        glDispatchCompute((unsigned int)TEXTURE_WIDTH, (unsigned int)TEXTURE_HEIGHT, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        // Render quad
        shader.Bind();
        glBindVertexArray(VAO);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // End-of-frame logic
        window.SwapBuffers();
        window.PollEvents();
    }

    Renderer::Shutdown();
}