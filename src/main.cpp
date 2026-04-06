#include <rendering/render-objects/Image.h>
#include <rendering/render-objects/Window.h>
#include <rendering/render-objects/ComputeShader.h>
#include <rendering/render-objects/Shader.h>
#include <rendering/render-objects/Mesh.h>
#include <rendering/render-objects/Texture.h>
#include <rendering/render-objects/GPUBuffer.h>
#include <rendering/render-objects/Framebuffer.h>
#include <rendering/PathTracer.h>
#include <rendering/Renderer.h>
#include <rendering/Rasterizer.h>
#include <scenes/Scene.h>
#include <scenes/PointLight.h>
#include <scenes/Sphere.h>
#include <scenes/Plane.h>
#include <UIManager.h>
#include <Raycast.h>
#include <imgui.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <chrono>

void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

Camera* camera;
Scene scene;

PathTracer* pathTracer;

bool paused = false;
bool pathTraced = true;

int placementShape = 0; // 0 for sphere, 1 for cube
float placementRadius = 0.5f;
float placementCubeSize[3] = { 1.0f, 1.0f, 1.0f };
int placementMaterial = 0;

int renderSamples = 1000;

int main()
{
    // Initialize scene (Cornell Box)
    scene.AddMaterial(Material(glm::vec3(1.0f), glm::vec3(0.0f), 1.0f, 0.0f));
    scene.AddMaterial(Material(glm::vec3(1.0f), glm::vec3(1.0f), 1.0f, 0.0f));
    scene.AddMaterial(Material(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f), 1.0f, 0.0f));
    scene.AddMaterial(Material(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f), 1.0f, 0.0f));
    scene.AddMaterial(Material(glm::vec3(1.0f), glm::vec3(0.0f), 0.0f, 1.0f));
    scene.AddMaterial(Material(glm::vec3(0.6f, 0.8f, 1.0f), glm::vec3(0.0f), 0.3f, 1.0f));

    scene.AddPlane(Plane(glm::vec3(0.0f, -2.5f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), 0));
    scene.AddCube(Cube(glm::vec3(0.0f, 0.0f, -2.75f), glm::vec3(5.0f, 5.0f, 0.5f), 0));
    scene.AddCube(Cube(glm::vec3(0.0f, 2.75f, 0.0f), glm::vec3(6.0f, 0.5f, 6.0f), 0));
    scene.AddCube(Cube(glm::vec3(-2.75f, 0.0f, 0.0f), glm::vec3(0.5f, 5.0f, 5.0f), 2));
    scene.AddCube(Cube(glm::vec3(2.75f, 0.0f, 0.0f), glm::vec3(0.5f, 5.0f, 5.0f), 3));
    scene.AddCube(Cube(glm::vec3(1.0f, -1.0f, -1.5f), glm::vec3(1.5f, 3.0f, 1.5f), 5));
    scene.AddSphere(Sphere(glm::vec3(-1.25f, -1.75f, -0.5f), 4, 0.75f));
    scene.AddCube(Cube(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(5.0f, 5.0f, 0.25f), 4));

    scene.AddQuadLight(QuadLight(glm::vec3(-0.5f, 2.499f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), 1, glm::vec3(1.0f), 15.0f, 1.0f));

    const unsigned int TEXTURE_WIDTH = 1920, TEXTURE_HEIGHT = 1080;

    // Create window
    Renderer::Init();
    Window::InitWindow(TEXTURE_WIDTH, TEXTURE_HEIGHT, "Path Traced Renderer");
    auto& window = Window::GetInstance();
    glfwSetCursorPosCallback(window.GetWindow(), mouseCallback);
    glfwSetMouseButtonCallback(window.GetWindow(), mouseButtonCallback);
    glfwSetKeyCallback(window.GetWindow(), keyCallback);

    camera = new Camera(glm::vec3(0.0f, 0.0f, 4.5f), glm::vec3(0.0f, -90.0f, 0.0f), 70.0f, glm::vec3(0.1f));

    // Create test texture
    Texture pathTracedTexture(TEXTURE_WIDTH, TEXTURE_HEIGHT);

    // Create quad for displaying the texture
    Vertex vertices[] = {
        // positions   // normals    // texCoords
        glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec2(0.0f, 1.0f),
        glm::vec3( 1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec2(1.0f, 1.0f),
        glm::vec3(-1.0f,  1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec2(0.0f, 0.0f),
        glm::vec3( 1.0f,  1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec2(1.0f, 0.0f)
    };
    uint32_t indices[] = {
        0, 2, 1,
        1, 2, 3
    };
    Mesh screenMesh(vertices, 4, indices, 6);

    // Load shaders
    Shader screenShader("assets/shaders/screen.vert", "assets/shaders/screen.frag");
    screenShader.SetInt("tex", 0);

    Shader shapeShader("assets/shaders/shape.vert", "assets/shaders/shape.frag");

    ComputeShader computeShader("assets/shaders/compute_shader.glsl");

    // Create Renderers
    pathTracer = new PathTracer(scene, computeShader);
    Rasterizer rasterizer(scene, shapeShader);

    // Initialize ImGUI
    UIManager::InitImGUI();

    // Lock mouse
    glfwSetInputMode(window.GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    float deltaTime = 0;
    float lastFrame = 0;

    float previousTime = 0;
    int frameCount = 0;
    int fps = 0;

    float avgRenderTime = 0.0f;
    float renderTime = 0.0f;

    bool vsync = false;
    Renderer::SetVsync(vsync);

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
            // Set FPS
            fps = frameCount;

            // Set render time
            avgRenderTime = renderTime / frameCount;
            renderTime = 0;

            // Reset counters
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

        // Render
        auto start = std::chrono::steady_clock::now();

        if (pathTraced)
        {
            pathTracer->PathTrace(*camera, TEXTURE_WIDTH, TEXTURE_HEIGHT);

            // Render quad
            screenShader.Bind();
            pathTracedTexture.Bind(0);
            screenMesh.Draw();
        }
        else
        {
            window.SetBackgroundColor(camera->backgroundColor.r, camera->backgroundColor.g, camera->backgroundColor.b, 1.0f);

            rasterizer.Render(*camera);
        }

        auto end = std::chrono::steady_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        renderTime += duration.count();

        // Render UI
        UIManager::BeginFrame();

        ImGui::Text("FPS: %d", fps);
        ImGui::Text("Average Render Time: %f µs", avgRenderTime);
        ImGui::Text("Pos: %f, %f, %f", camera->position.x, camera->position.y, camera->position.z);
        ImGui::Text("Dir: %f, %f, %f", camera->direction.x, camera->direction.y, camera->direction.z);
        ImGui::Checkbox("Path Traced", &pathTraced);
        if (ImGui::Checkbox("Vsync", &vsync))
        {
            Renderer::SetVsync(vsync);
        }
        ImGui::Text("Placement");
        ImGui::SliderInt("Shape", &placementShape, 0, 1);
        if (placementShape == 0)
            ImGui::SliderFloat("Sphere Radius", &placementRadius, 0.1f, 2.0f);
        else
            ImGui::SliderFloat3("Cube Size", placementCubeSize, 0.1f, 4.0f);
        ImGui::SliderInt("Material", &placementMaterial, 0, 5);
        ImGui::InputInt("Render Samples", &renderSamples);
        if (ImGui::Button("Render Image"))
        {
            Framebuffer framebuffer;
            framebuffer.StartRender(pathTracedTexture);
            pathTracer->ResetImage();
            for (int i = 0; i < renderSamples; i++)
            {
                pathTracer->PathTrace(*camera, TEXTURE_WIDTH, TEXTURE_HEIGHT);
            }

            framebuffer.SaveToFile("output.png", TEXTURE_WIDTH, TEXTURE_HEIGHT);
        }

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

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (paused) return;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        Ray ray(camera->position, camera->Front());
        Raycast::RaycastResult result;
        if (Raycast::Cast(scene, ray, 1000.0f, result))
        {
            if (scene.Destroy(result.objectHit))
                pathTracer->ResetImage();
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        Ray ray(camera->position, camera->Front());
        Raycast::RaycastResult result;
        if (Raycast::Cast(scene, ray, 1000.0f, result))
        {
            if (placementShape == 0)
            {
                glm::vec3 spawnPos = ray.origin + ray.direction * (result.dist - placementRadius);
                scene.AddSphere(Sphere(spawnPos, placementMaterial, placementRadius));
            }
            else
            {
                glm::vec3 spawnPos = ray.origin + ray.direction * (result.dist - (placementCubeSize[0] * 0.5f));
                scene.AddCube(Cube(spawnPos, glm::vec3(placementCubeSize[0], placementCubeSize[1], placementCubeSize[2]), placementMaterial)); 
            }
            pathTracer->ResetImage();
        }
    }
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