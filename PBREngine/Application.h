#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Application
{
public:
    Application();
    ~Application();

    void run();
private:

    void initWindow();
    void initVulkan();
    void createInstance();

    void mainLoop();

    void cleanup();

    GLFWwindow* m_window;
    VkInstance instance;
};

