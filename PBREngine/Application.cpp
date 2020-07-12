#include "Application.h"

#include <iostream>
#include <map>
#include <stdexcept>

const uint32_t kWindowWidth = 800;
const uint32_t kWindowHeight = 600;

const std::vector<const char*> kValidationLayers =
{
    "VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
const bool kEnableValidationLayers = false;
#else
const bool kEnableValidationLayers = true;
#endif

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

    if(func == nullptr)
        return VK_ERROR_EXTENSION_NOT_PRESENT;

    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

    if(func != nullptr)
        return func(instance, debugMessenger, pAllocator);
}

Application::Application() : m_window(nullptr), m_instance(VK_NULL_HANDLE), m_debugMessenger(VK_NULL_HANDLE), m_physicalDevice(VK_NULL_HANDLE)
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
    setupDebugMessenger();
    pickPhysicalDevice();
    createLogicalDevice();
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
    vkDestroyDevice(m_device, nullptr);

    if(kEnableValidationLayers)
        DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);

    vkDestroyInstance(m_instance, nullptr);

    glfwDestroyWindow(m_window);

    glfwTerminate();
}

void Application::createInstance()
{
    if(kEnableValidationLayers && !checkRequiredValidationLayers())
        throw std::runtime_error("Validation layer requested, but not available");

    std::vector<const char*> extensions;
    if(!getRequiredExtensions(extensions))
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

    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if(kEnableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(kValidationLayers.size());
        createInfo.ppEnabledLayerNames = kValidationLayers.data();

        initDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = &debugCreateInfo;
    }
    else
    {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    VkResult result = vkCreateInstance(&createInfo, nullptr, &m_instance);

    if(result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create instance!");
    }
}

void Application::setupDebugMessenger()
{
    if(!kEnableValidationLayers)
        return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    initDebugMessengerCreateInfo(createInfo);

    if(CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS)
        throw std::runtime_error("Failed to set up debug messenger");
}

void Application::pickPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

    if(deviceCount == 0)
        throw std::runtime_error("Failed to find GPUs with Vulkan support!");

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

    // Use ordered map to find best-match GPU
    std::multimap<int, VkPhysicalDevice> deviceRating;

    for(const auto& device : devices)
    {
        int score = rateDeviceSuitability(device);
        deviceRating.insert(std::make_pair(score, device));
    }

    // Check if the best candidate is suitable
    if(deviceRating.rbegin()->first > 0)
        m_physicalDevice = deviceRating.rbegin()->second;
    else
        throw std::runtime_error("Failed to find a suitable GPU!");
}

void Application::createLogicalDevice()
{
    QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);

    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
    queueCreateInfo.queueCount = 1;
    float queuePriority = 1.f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = 0;

    if(kEnableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(kValidationLayers.size());
        createInfo.ppEnabledLayerNames = kValidationLayers.data();
    }
    else
        createInfo.enabledLayerCount = 0;

    if(vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS)
        throw std::runtime_error("Failed to create logical device!");

    vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
}

bool Application::getRequiredExtensions(std::vector<const char*>& extensions) const
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

    std::vector<const char*> requiredExtensions = std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);
    if(kEnableValidationLayers)
        requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    extensions.clear();

    for(const auto& requiredExtension : requiredExtensions)
    {
        bool supported = false;
        for(const auto& extension : vkExtensions)
        {
            if(strcmp(requiredExtension, extension.extensionName) == 0)
            {
                supported = true;
                break;
            }
        }

        if(!supported)
        {
            std::cerr << requiredExtension << " is not supported on this device" << std::endl;
            return false;
        }

        extensions.push_back(requiredExtension);
    }

    return true;
}

bool Application::checkRequiredValidationLayers() const
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for(const char* layerName : kValidationLayers)
    {
        bool layerFound = false;

        for(const auto& layerProperties : availableLayers)
        {
            if(strcmp(layerProperties.layerName, layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if(!layerFound)
            return false;
    }

    return true;
}

void Application::initDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) const
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr;
}

int Application::rateDeviceSuitability(VkPhysicalDevice device) const
{
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;

    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    // Application needs geometry shader support
    if(!deviceFeatures.geometryShader)
        return 0;

    // Check support for graphics Queue family
    QueueFamilyIndices indices = findQueueFamilies(device);
    if(!indices.isComplete())
        return  0;

    // Calculate score for other parameters 
    int score = 0;

    // Discrete GPUs has performance advantage
    if(deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        score += 1000;

    // Maximum possible size of textures affects graphics quality
    score += deviceProperties.limits.maxImageDimension2D;

    return score;
}

Application::QueueFamilyIndices Application::findQueueFamilies(VkPhysicalDevice device) const
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for(const auto& queueFamily : queueFamilies)
    {
        if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            indices.graphicsFamily = i;

        if(indices.isComplete())
            break;

        ++i;
    }

    return indices;
}


bool Application::QueueFamilyIndices::isComplete()
{
    return graphicsFamily.has_value();
}