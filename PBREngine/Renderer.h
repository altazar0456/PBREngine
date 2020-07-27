#pragma once

#include "QueueFamilyIndices.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <vector>

class Renderer
{
public:
    Renderer(VkDevice device, VkSwapchainKHR swapChain, const std::vector<VkImage>& swapChainImages, const std::vector<VkImageView>& swapChainImageViews, VkFormat swapChainImageFormat,
        VkExtent2D swapChainExtent, const QueueFamilyIndices& queueFamilyIndices, VkQueue graphicsQueue, VkQueue presentQueue);
    ~Renderer();

    void init(const std::string& vertShaderFilename, const std::string& fragShaderFilename);
    void shutdown();

    void drawFrame();

private:
    VkShaderModule createShaderModule(const std::string& filename);

    void createRenderPass();
    void createGraphicsPipeline(const std::string& vertShaderFilename, const std::string& fragShaderFilename);
    void createFrameBuffers();
    void createCommandPool();
    void createCommandBuffers();
    void createSyncObjects();

    // Variables set outside
    VkDevice m_device;
    VkSwapchainKHR m_swapChain;
    std::vector<VkImage> m_swapChainImages;
    std::vector<VkImageView> m_swapChainImageViews;
    VkFormat m_swapChainImageFormat;
    VkExtent2D m_swapChainExtent;
    QueueFamilyIndices m_queueFamilyIndices;
    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;

    // Variables created here
    VkRenderPass m_renderPass;
    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_graphicsPipeline;
    std::vector<VkFramebuffer> m_swapChainFrameBuffers;
    VkCommandPool m_commandPool;
    std::vector<VkCommandBuffer> m_commandBuffers;
    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;
    std::vector<VkFence> m_imagesInFlight;

    size_t m_currentFrame;
};

