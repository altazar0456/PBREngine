#include "Application.h"

#include <stdexcept>

const uint32_t kWindowWidth = 800;
const uint32_t kWindowHeight = 600;

Application::Application() : m_window(nullptr)
{    
}

Application::~Application()
{
}

void Application::run()
{
    glfwInit();

    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}

void Application::initWindow()
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_window = glfwCreateWindow(kWindowWidth, kWindowHeight, "Vulkan", nullptr, nullptr);
}

void Application::initVulkan()
{
    createInstance();
}

void Application::mainLoop()
{
    while(!glfwWindowShouldClose(m_window))
    {
        glfwPollEvents();
    }
}

void Application::cleanup()
{
    vkDestroyInstance(instance, nullptr);

    glfwDestroyWindow(m_window);

    glfwTerminate();
}

void Application::createInstance()
{
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "PBR Engine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "PBR Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

    createInfo.enabledLayerCount = 0;

    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

    if(result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create instance!");
    }
}
