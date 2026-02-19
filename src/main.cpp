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

    Camera camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.1f));

    Image image(1280, 720);
    PathTracer::PathTrace(scene, camera, image);
    image.SaveToPPM("output.ppm");

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

    // TEMP - Set compute shader variables
    glm::vec3 target = camera.position + camera.direction;
    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 viewMatrix = glm::lookAt(camera.position, target, worldUp);
    glm::mat4 cameraToWorld = glm::inverse(viewMatrix);

    glm::vec3 rayOriginWorld = glm::vec3(cameraToWorld * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    computeShader.Bind();
    computeShader.SetMat4("cameraToWorld", cameraToWorld);
    computeShader.SetVec3("rayOriginWorld", rayOriginWorld);
    computeShader.SetFloat("fov", camera.fov);
    computeShader.SetVec3("backgroundColor", camera.backgroundColor);

    // Pass spheres
    #pragma pack(1)

    struct GPUSphere
    {
        glm::vec3 center;
        float radius;
        glm::vec4 color;
    };

    struct GPUPlane
    {
        glm::vec3 offset;
        float padding1;
        glm::vec3 orientation;
        float padding2;
        glm::vec4 color;
    };

    #pragma pack()

    std::vector<GPUSphere> gpuSpheres = {
        { { 0.0f, 0.0f, -5.0f }, 1.0f, { 1.0f, 1.0f, 1.0f, 1.0f } },
        { { 1.0f, 0.5f, -4.0f }, 0.5f, { 0.4f, 0.5f, 1.0f, 1.0f } }
    };
    std::vector<GPUPlane> gpuPlanes = {
        { { 0.0f, -2.0f, 0.0f }, 0.0f, { 0.0f, -1.0f, 0.0f }, 0.0f, { 0.5f, 0.5f, 0.5f, 1.0f } }
    };

    GLuint spheresBuffer;
    glCreateBuffers(1, &spheresBuffer);
    glNamedBufferStorage(spheresBuffer, sizeof(GPUSphere) * gpuSpheres.size(), (const void*)gpuSpheres.data(), GL_DYNAMIC_STORAGE_BIT);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, spheresBuffer);
    computeShader.SetInt("numSpheres", gpuSpheres.size());

    GLuint planesBuffer;
    glCreateBuffers(1, &planesBuffer);
    glNamedBufferStorage(planesBuffer, sizeof(GPUPlane) * gpuPlanes.size(), (const void*)gpuPlanes.data(), GL_DYNAMIC_STORAGE_BIT);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, planesBuffer);
    computeShader.SetInt("numPlanes", gpuPlanes.size());

    float deltaTime = 0;
    float lastFrame = 0;

    while (!window.ShouldClose())
    {
        // Set frame time
        float currentFrame = Renderer::GetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Clear window
        window.Clear();

        // Run compute shader
        computeShader.Bind();
        glDispatchCompute((unsigned int)TEXTURE_WIDTH/16, (unsigned int)TEXTURE_HEIGHT/16, 1);
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