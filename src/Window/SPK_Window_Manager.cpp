//#define USE_WINDOWS
#ifdef USE_WINDOWS
#include <Window/SPK_Window_Manager.h>
#include "glad/glad.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

WindowManager::WindowManager()
{
    width = 800;
    height = 600;
}

GLFWwindow* WindowManager::GetWindow()
{
    if(window == nullptr)
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
        window = glfwCreateWindow(width, height, "SPARK Rain Demo", NULL, NULL);
    }

    return window;
}

void WindowManager::DestroyWindow()
{
    glfwTerminate();
}

void WindowManager::MakeCurrentContext()
{
    glfwMakeContextCurrent(window);
}

void WindowManager::SetWindowSize(float width, float height)
{
    width = width; height = height;
}

bool WindowManager::ShouldWindowClose()
{
    return glfwWindowShouldClose(window);
}

void WindowManager::RegisterCallBacks()
{
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	if (glfwRawMouseMotionSupported())
    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	glfwSetCursorPosCallback(window, cursor_position_callback);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	// angleY += (xpos-prevMouseX) * 0.05f;
	// angleX += (ypos-prevMouseY) * 0.05f;
	// angleX = min(90.0f,max(-90.0f,angleX)); 

	// prevMouseX = xpos;
	// prevMouseY = ypos;
}
#endif