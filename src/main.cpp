#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>
#include <implot.h>
#include <stdio.h>

#include <vector>

#include "Application.h"
#include "CpuManager.h"
#include "MemoryManager.h"
#include "NetworkManager.h"
#include "ProcessManager.h"

int main(int, char**) {
    // Initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) !=
        0) {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // Enable native IME
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    // Set OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    // Create window
    float main_scale = 1.0f;  // Adjust for high-DPI if needed
    SDL_Window* window = SDL_CreateWindow(
        "Linux Resource Monitor (SDL2+OpenGL3)", SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, (int)(1280 * main_scale),
        (int)(720 * main_scale),
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!window) {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (!gl_context) {
        printf("Error: SDL_GL_CreateContext(): %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1);  // Enable vsync

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // Enable Docking
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Disabled for
    // simplicity

    // Set ImGui style
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;

    // Initialize ImGui backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Initialize ImPlot
    ImPlot::CreateContext();

    // Initialize managers
    static ProcessManager s_processManager;
    s_processManager.start_();

    static CPU s_CpuManager;
    s_CpuManager.start_();

    static Memory s_MemoryManager;
    s_MemoryManager.start_();

    static Network s_NetworkManager;
    s_NetworkManager.start_();

    // Main loop
    bool done = false;
    while (!done) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT &&
                event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) {
            SDL_Delay(10);
            continue;
        }

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        // Set up the docked layout
        Application::SetupUILayout();

        ImGui::ShowDemoWindow();

        // Process UI
        std::vector<Process> process = s_processManager.get_process();
        Application::ProcessUI(process);

        // CPU usage plot
        if (ImGui::Begin("CPU", nullptr)) {
            Application::CpuUI(s_CpuManager);
        }
        ImGui::End();

        // Memory UI
        if (ImGui::Begin("Memory", nullptr)) {
            Application::MemoryUI(s_MemoryManager);
        }
        ImGui::End();

        // Network UI
        if (ImGui::Begin("Network", nullptr)) {
            Application::NetworkUI(s_NetworkManager);
        }
        ImGui::End();

        // Render
        ImGui::Render();
        int display_w, display_h;
        SDL_GetWindowSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        glClearColor(clear_color.x * clear_color.w,
                     clear_color.y * clear_color.w,
                     clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
