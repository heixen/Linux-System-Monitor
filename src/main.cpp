#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>
#include <stdio.h>

#include <array>
#include <iostream>
#include <vector>

#include "Application.h"
#include "Cpu.h"
#include "CpuManager.h"
#include "MemoryManager.h"
#include "NetworkManager.h"
#include "ProcessManager.h"

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main(int, char**) {
    // Set GLFW error callback
    glfwSetErrorCallback(glfw_error_callback);

    // Initialize GLFW
    if (!glfwInit()) {
        return 1;
    }

    // Set GL version (OpenGL 3.0 + GLSL 130 for compatibility)
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create window
    GLFWwindow* window =
        glfwCreateWindow(1280, 720, "Alexandre", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // Enable vsync

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // Enable Docking
    // io.ConfigFlags |=
    //     ImGuiConfigFlags_ViewportsEnable;  // Enable Multi-Viewport

    // Set ImGui style
    ImGui::StyleColorsDark();

    // Tweak style for viewports
    ImGuiStyle& style = ImGui::GetStyle();
    // if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    //     style.WindowRounding = 0.0f;
    //     style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    // }

    // Initialize ImGui backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Initialize ImPlot
    ImPlot::CreateContext();

    // Initialize ProcessManager and CpuManager
    static ProcessManager s_processManager;
    s_processManager.start_();

    static CPU s_CpuManager;
    s_CpuManager.start_();

    static Memory s_MemoryManager;
    s_MemoryManager.start_();

    static NetworkManager s_NetworkManager;
    s_NetworkManager.start_();

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Skip rendering if window is minimized
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Docking setup
        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        ImGuiWindowFlags window_flags =
            ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen) {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(viewport->Size);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar |
                            ImGuiWindowFlags_NoCollapse |
                            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus |
                            ImGuiWindowFlags_NoNavFocus;
        }

        if (!opt_padding) {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,
                                ImVec2(0.0f, 0.0f));
        }

        ImGui::PushStyleColor(
            ImGuiCol_WindowBg,
            ImVec4(1.0f, 1.0f, 1.0f, 1.0f));  // Solid white BG

        ImGui::Begin("MainDockSpace", nullptr, window_flags);

        // Menu bar
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("I'm")) {
                if (ImGui::MenuItem("asdsd")) {
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("CPU")) {
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Memory")) {
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Disks")) {
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        if (!opt_padding) {
            ImGui::PopStyleVar();
        }
        if (opt_fullscreen) {
            ImGui::PopStyleVar(2);
        }
        ImGui::PopStyleColor();

        // Submit DockSpace
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }
        ImGui::End();

        ImGui::ShowDemoWindow();
        ImPlot::ShowDemoWindow();

        // Process UI
        ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
        std::vector<Process> process = s_processManager.get_process();
        Application::ProcessUI(process);

        // CPU usage plot
        if (ImGui::Begin("CPU", nullptr)) {
            Application::CpuUI(s_CpuManager);
        }
        ImGui::End();

        // MemoryUI
        if (ImGui::Begin("Memory")) {
            Application::MemoryUI(s_MemoryManager);
        }
        ImGui::End();

        // Network UI
        // if (ImGui::Begin("Network")) {
        //     Application::NetworkUI(s_NetworkManager);
        // }
        ImGui::End();
        // Render
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        glClearColor(clear_color.x * clear_color.w,
                     clear_color.y * clear_color.w,
                     clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and render additional platform windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    ImPlot::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
