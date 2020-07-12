#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

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
    bool getRequaredExtensions(std::vector<const char*>& extensions);

    void mainLoop();

    void cleanup();

    GLFWwindow* m_window;
    VkInstance instance;
};

