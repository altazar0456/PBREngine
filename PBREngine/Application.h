#pragma once

class GLFWwindow;

class Application
{
public:
    Application();
    ~Application();

    void run();

    void initWindow();

    void initVulkan();

    void mainLoop();

    void cleanup();

private:
    GLFWwindow* m_window;
};

