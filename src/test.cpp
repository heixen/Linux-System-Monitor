#include <GLFW/glfw3.h>  // GLFW for window and input
#include <imgui.h>       // Dear ImGui
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>  // ImPlot for plotting

#include <array>
#include <cmath>  // For sin/cos simulation

// Simulated network data retrieval (replace with real network API)
struct Network {
    float GetDownloadSpeed() {
        return 50 + 20 * std::sin(t_ / 5.0f);
    }  // 30-70 Mbps
    float GetUploadSpeed() {
        return 10 + 5 * std::cos(t_ / 5.0f);
    }  // 5-15 Mbps
    void SetTime(float t) { t_ = t; }

   private:
    float t_ = 0.0f;
};

void NetworkUI(Network& network) {
    static float t = 0;                    // Current time
    static float sample_timer = 0.0f;      // Time since last sample
    static constexpr int BUFFERSIZE = 60;  // 60 seconds of data (1 sample/sec)
    static std::array<float, BUFFERSIZE>
        download_usage;  // Positive: download speeds
    static std::array<float, BUFFERSIZE>
        upload_usage;                           // Negative: upload speeds
    static std::array<float, BUFFERSIZE> time;  // Timestamps
    static int index = 0;                       // Current buffer index
    static int count = 0;                       // Number of valid data points

    // Update time
    float dt = ImGui::GetIO().DeltaTime;
    t += dt;
    sample_timer += dt;

    // Sample network speeds every 1 second
    if (sample_timer >= 1.0f) {
        network.SetTime(t);
        time[index] = t;
        download_usage[index] = network.GetDownloadSpeed();  // Positive
        upload_usage[index] = -network.GetUploadSpeed();     // Negative
        index = (index + 1) % BUFFERSIZE;
        count = std::min(count + 1, BUFFERSIZE);
        sample_timer = 0.0f;
    }

    // Render plot
    if (ImGui::BeginChild("Network", ImVec2(0, 0), ImGuiChildFlags_Border)) {
        ImPlotAxisFlags axisflags =
            ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_NoTickMarks;

        if (ImPlot::BeginPlot("Network Speeds", ImVec2(-1, -1),
                              ImPlotFlags_NoLegend | ImPlotFlags_NoFrame)) {
            // Setup x-axis (time, last 60 seconds)
            ImPlot::SetupAxis(ImAxis_X1, nullptr, axisflags);
            ImPlot::SetupAxisLimits(ImAxis_X1, t - 60.0f, t - 0.9f,
                                    ImPlotCond_Always);

            // Setup y-axis (symmetric for download and upload)
            ImPlot::SetupAxis(ImAxis_Y1, nullptr,
                              axisflags | ImPlotAxisFlags_Opposite);
            ImPlot::SetupAxisLimits(
                ImAxis_Y1, -20.0f,
                100.0f);  // -20 for upload, +100 for download

            // Plot download (positive y, above x-axis) as filled area
            ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
            ImPlot::PushStyleColor(ImPlotCol_Fill,
                                   ImVec4(0.0f, 0.5f, 1.0f, 1.0f));  // Blue
            ImPlot::PlotShaded("download", time.data(), download_usage.data(),
                               count, 0.0f, 0, 0);
            ImPlot::PopStyleColor();
            ImPlot::PlotLine("download", time.data(), download_usage.data(),
                             count, 0, 0);
            ImPlot::PopStyleVar();

            // Plot upload (negative y, below x-axis) as filled area
            ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
            ImPlot::PushStyleColor(ImPlotCol_Fill,
                                   ImVec4(1.0f, 0.5f, 0.0f, 1.0f));  // Orange
            ImPlot::PlotShaded("upload", time.data(), upload_usage.data(),
                               count, 0.0f, 0, 0);
            ImPlot::PopStyleColor();
            ImPlot::PlotLine("upload", time.data(), upload_usage.data(), count,
                             0, 0);
            ImPlot::PopStyleVar();

            ImPlot::EndPlot();
        }
        ImGui::EndChild();
    }
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        return -1;
    }

    // Create GLFW window with OpenGL 3.3 context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window =
        glfwCreateWindow(1280, 720, "Network Speed Plot", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // V-sync

    // Initialize ImGui and ImPlot
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();

    // Setup ImGui backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Network object
    Network network;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Poll events
        glfwPollEvents();

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render network plot
        ImGui::Begin("Network Monitor");
        NetworkUI(network);
        ImGui::End();

        // Render ImGui
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
