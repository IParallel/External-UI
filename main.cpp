#include <iostream>
#include "gui.hpp"
#include "libs/imgui/include/imgui.h"

static void DrawMenu() {
    static bool opened = true;

    ImGui::SetNextWindowSize(ImVec2{ 800, 600 }, ImGuiCond_FirstUseEver);
    ImGui::Begin("Test", &opened);
    ImGui::Button("Test Btn");
    ImGui::End();

    if (!opened) {
        exit(0);
    }
}

static void BeforeWindow() {
    glfwWindowHint(GLFW_DECORATED, 0);
}

int main() {
    gui::DrawCallback(DrawMenu);
    gui::BeforeCreatingWindow(BeforeWindow);

    if (!gui::Init())
        return 0;

    gui::Loop();

    return 0;
}