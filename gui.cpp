#include "gui.hpp"
#include <Windows.h>
#include <iostream>
#include "libs/imgui/include/imgui.h"
#include "libs/imgui/include/imgui_impl_glfw.h"
#include "libs/imgui/include/imgui_impl_opengl3.h"
#include "libs/imgui/include/imgui_internal.h"
#include <thread>
#include <chrono>

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

bool gui::Init() {
    glfwSetErrorCallback(glfw_error_callback);
    /* Initialize the library */
    if (!glfwInit())
        return false;


#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+
    // only glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only
#endif

    /* Create a windowed mode window and its OpenGL context */
    if (beforeCreatingWindowCallback != nullptr)
        beforeCreatingWindowCallback();

    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);

    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return false;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    if (beforeCreatingContextCallback != nullptr)
        beforeCreatingContextCallback();

    ImGui::CreateContext();

    if (afterCreatingContextCallback != nullptr)
        afterCreatingContextCallback();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      // Enable docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    io.IniFilename = nullptr;

    if (drawCallback == nullptr)
        return false;

    return true;

}

void gui::BeforeCreatingWindow(void(*callback)(void)) {
    beforeCreatingWindowCallback = callback;
}

void gui::BeforeCreatingContext(void(*callback)(void)) {
    beforeCreatingContextCallback = callback;
}

void gui::AfterCreatingContext(void(*callback)(void)) {
    afterCreatingContextCallback = callback;
}

void gui::DrawCallback(void(*callback)(void)) {
    drawCallback = callback;
}

void gui::Loop() {
    bool passing_through = true;
    while (!glfwWindowShouldClose(window)) {
        //std::this_thread::sleep_for(std::chrono::milliseconds(1));
        if (show_window && passing_through) {
            passing_through = false;
            glfwSetWindowAttrib(window, GLFW_MOUSE_PASSTHROUGH, GLFW_FALSE);
        }

        if (!show_window && !passing_through) {
            passing_through = true;
            glfwSetWindowAttrib(window, GLFW_MOUSE_PASSTHROUGH, GLFW_TRUE);
        }

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // ImGui Draw
        if (show_window) {
            drawCallback();
        }

        ImGuiContext* context = ImGui::GetCurrentContext();
        if (!context) return;

        ImGuiContext& g = *context;
        for (ImGuiWindow* window : g.Windows) {
            if (window->Active) { // Check if the window is currently active
                // Check window position and correct if out of screen bounds
                ImVec2 pos = window->Pos;
                ImVec2 size = window->Size;
                ImVec2 display_size = ImGui::GetIO().DisplaySize;

                float new_x = pos.x;
                float new_y = pos.y;

                if (pos.x < 0) new_x = 0;
                if (pos.y < 0) new_y = 0;
                if (pos.x + size.x > display_size.x) new_x = display_size.x - size.x;
                if (pos.y + size.y > display_size.y) new_y = display_size.y - size.y;

                if (new_x != pos.x || new_y != pos.y) {
                    ImGui::SetWindowPos(window, ImVec2(new_x, new_y));
                }
            }
        }

        // Render ImGui
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        // Update and Render additional Platform Windows
        // glfw needs to swap context or else something buggy happens
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault(NULL, NULL);
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);

        glfwPollEvents();

    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}
