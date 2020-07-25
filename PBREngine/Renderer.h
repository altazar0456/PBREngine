#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

class Renderer
{
public:
    Renderer(VkDevice device, VkFormat swapChainImageFormat, VkExtent2D swapChainExtent);
    ~Renderer();

    void init(const std::string& vertShaderFilename, const std::string& fragShaderFilename);
    void shutdown();

    VkPipeline getPipeline();
private:
    VkShaderModule createShaderModule(const std::string& filename);

    void createRenderPass();
    void createGraphicsPipeline(const std::string& vertShaderFilename, const std::string& fragShaderFilename);
    
    VkDevice m_device;
    VkFormat m_swapChainImageFormat;
    VkExtent2D m_swapChainExtent;

    VkRenderPass m_renderPass;
    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_graphicsPipeline;
};

