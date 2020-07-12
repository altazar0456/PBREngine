#include "Application.h"

#include <iostream>
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
    std::vector<const char*> extensions;
    if(!getRequaredExtensions(extensions))
        throw std::runtime_error("This devise doesn't have required Vulkan extensions");

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

    createInfo.enabledExtensionCount = extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();

    createInfo.enabledLayerCount = 0;

    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

    if(result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create instance!");
    }
}

bool Application::getRequaredExtensions(std::vector<const char*>& extensions)
{
    // Get all supported extensions
    uint32_t vkExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &vkExtensionCount, nullptr);
    std::vector<VkExtensionProperties> vkExtensions(vkExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &vkExtensionCount, vkExtensions.data());

    // Get required extensions
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    extensions.clear();

    std::cout << "Available extensions:" << std::endl;
    bool allSupported = true;
    for(int i = 0; i < glfwExtensionCount; ++i)
    {
        const char* glfwCurrExtension = glfwExtensions[i];
        bool supported = false;
        for(const auto& extension : vkExtensions)
        {
            if(strcmp(glfwCurrExtension, extension.extensionName) == 0)
            {
                supported = true;
                break;
            }
        }

        if(supported)
            extensions.push_back(glfwCurrExtension);

        allSupported = allSupported && supported;
        std::cout << '\t' << glfwCurrExtension << '\t' <<(supported ? "Supported" : "Not Supported")<< std::endl;
    }

    return allSupported;
}
