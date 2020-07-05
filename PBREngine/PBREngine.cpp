#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vec4.hpp>

#include <iostream>

const uint32_t kWindowWidth = 800;
const uint32_t kWindowHeight = 600;

class HelloTriangleApplication
{
public:
    void run()
    {
        glfwInit();

        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

    void initWindow()
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_window = glfwCreateWindow(kWindowWidth, kWindowHeight, "Vulkan", nullptr, nullptr);
    }

    void initVulkan()
    {
        
    }

    void mainLoop()
    {
        while(!glfwWindowShouldClose(m_window))
        {
            glfwPollEvents();
        }
    }

    void cleanup()
    {
        glfwDestroyWindow(m_window);

        glfwTerminate();
    }

private:
    GLFWwindow* m_window;
};

int main(void)
{
    HelloTriangleApplication app;

    try
    {
        app.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
