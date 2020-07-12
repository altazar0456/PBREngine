#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <optional>

class Application
{
public:
    Application();
    ~Application();

    void run();
private:
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;

        bool isComplete();
    };

    void initWindow();
    void initVulkan();
    void createInstance();
    void setupDebugMessenger();
    void pickPhysicalDevice();
    void createLogicalDevice();
    bool getRequiredExtensions(std::vector<const char*>& extensions) const;

    bool checkRequiredValidationLayers() const;

    void mainLoop();

    void cleanup();

    void initDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) const;
    int rateDeviceSuitability(VkPhysicalDevice device) const;
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) const;

    GLFWwindow* m_window;
    VkInstance m_instance;
    VkDebugUtilsMessengerEXT m_debugMessenger;
    VkPhysicalDevice m_physicalDevice;
    VkDevice m_device;
    VkQueue m_graphicsQueue;
};

