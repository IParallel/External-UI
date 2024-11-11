#pragma once
#include <GLFW/glfw3.h>
#include "libs/imgui/include/imgui.h"

namespace gui {
	inline GLFWwindow* window{};

	inline bool show_window = true;

	// Function pointers for the callbacks
	inline void (*beforeCreatingWindowCallback)(void) = nullptr;
	inline void (*beforeCreatingContextCallback)(void) = nullptr;
	inline void (*afterCreatingContextCallback)(void) = nullptr;
	inline void (*drawCallback)(void) = nullptr; 

	bool Init();
	void BeforeCreatingWindow(void(*callback)(void));
	void BeforeCreatingContext(void(*callback)(void));
	void AfterCreatingContext(void(*callback)(void));
	void DrawCallback(void (*callback)(void));
	void Loop();
}