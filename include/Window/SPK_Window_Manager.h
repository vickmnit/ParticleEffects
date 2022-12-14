//#define USE_WINDOWS
#ifdef USE_WINDOWS
#ifndef H_SPK_WNDMAN
#define H_SPK_WNDMAN

#if defined(WIN32) || defined(_WIN32)
#include <windows.h>
#endif

#define GLFW_INCLUDE_ES3
#include "glfw3.h"

class WindowManager
{
    public:

    GLFWwindow* window = nullptr;
    float width, height;

    WindowManager();

    GLFWwindow* GetWindow();

    void SetWindowSize(float width, float height);

    void MakeCurrentContext();

    void DestroyWindow();

    bool ShouldWindowClose();

    void RegisterCallBacks();
};
#endif
#endif