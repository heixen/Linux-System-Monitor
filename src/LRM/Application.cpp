#include "Application.h"

#include <algorithm>
#include <array>
#include <csignal>
#include <cstddef>
#include <iostream>
#include <vector>

#include "CpuManager.h"
#include "MemoryManager.h"
#include "NetworkManager.h"
#include "Process.h"
#include "ProcessList.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "implot.h"

namespace Application {
void ProcessUI(std::vector<Process> &processes) {
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysVerticalScrollbar;

    static ImGuiTextFilter filter;
    static int selectedIndex = -1;
    std::vector<Process *> filteredProcesses;

    filteredProcesses.clear();
    for (const auto &proc : processes) {
        if (filter.PassFilter(proc.program.c_str())) {
            filteredProcesses.push_back(const_cast<Process *>(&proc));
        }
    }

    if (ImGui::Begin("Process", nullptr, window_flags)) {
        if (ImGui::BeginMenuBar()) {
            filter.Draw("Search");
            ImGui::Separator();
            if (ImGui::Button("Terminate")) {
                if (!(selectedIndex < 0)) {
                    kill(filteredProcesses[selectedIndex]->pid, SIGINT);
                    selectedIndex = -1;
                }
            }
            ImGui::EndMenuBar();
        }

        RenderProcessTable(filteredProcesses, selectedIndex);
    }
    ImGui::End();
}

void CpuUI(CPU &cpu) {
    static float t = 0;
    static float sample_timer = 0.0f;
    static constexpr int BUFFER_SIZE = 3600;
    static std::array<float, BUFFER_SIZE> usage;
    static std::array<float, BUFFER_SIZE> time;
    static int index = 0;
    static int count = 0;
    static float cpuUsage;
    float dt = ImGui::GetIO().DeltaTime;
    t += dt;
    sample_timer += dt;

    ImPlotAxisFlags axisflags =
        ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_NoTickMarks;

    ImGui::BeginChild("Cpu", ImVec2(0, 0), ImGuiChildFlags_Border);

    // ImPlot::SetNextAxesToFit();
    if (ImPlot::BeginPlot("CPU", ImVec2(-1, -1),
                          ImPlotFlags_NoLegend | ImPlotFlags_NoFrame)) {
        ImPlot::SetupAxis(ImAxis_X1, nullptr, axisflags);
        ImPlot::SetupAxis(ImAxis_Y1, nullptr,
                          axisflags | ImPlotAxisFlags_Opposite);
        ImPlot::SetupAxisLimits(ImAxis_X1, t - 60.0f, t - 0.9,
                                ImPlotCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100, ImPlotCond_Always);

        if (sample_timer >= 1.0f) {
            cpuUsage = cpu.GetCpuPercentage();
            sample_timer = 0.0f;
            time[index] = t;
            usage[index] = cpuUsage;
            index = (index + 1) % BUFFER_SIZE;
            count = std::min(count + 1, BUFFER_SIZE);
        }
        ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
        ImPlot::PlotShaded("cpu", time.data(), usage.data(), count, 0, 0);
        ImPlot::PopStyleVar();
        ImPlot::PlotLine("cpu", time.data(), usage.data(), count, 0, 0);
        ImPlot::EndPlot();
    }
    if (ImGui::BeginItemTooltip()) {
        ImGui::Text("CPU Usage: %.2f%%", cpuUsage);
        ImGui::EndTooltip();
    }
    ImGui::EndChild();
}

void MemoryUI(Memory &memory) {
    static float t = 0;
    static float sample_timer = 0.0f;
    static constexpr int BUFFERSIZE = 3600;
    static std::array<float, BUFFERSIZE> usage;
    static std::array<float, BUFFERSIZE> time;
    static int index = 0;
    static int count = 0;

    float dt = ImGui::GetIO().DeltaTime;
    t += dt;
    sample_timer += dt;

    static float memTotal = 0.0f;
    static float memAvail = 0.0f;

    static bool firstTime = true;
    if (firstTime) {
        auto memT = memory.GetMemoryUsage();
        if (memT.count("MemTotal") && memT.count("MemAvailable")) {
            memTotal = static_cast<float>(memT["MemTotal"]) / 1048576.0f;
            memAvail = static_cast<float>(memT["MemAvailable"]) / 1048576.0f;
        } else {
            memTotal = 1.0f;
            memAvail = 0.0f;
        }
        firstTime = false;
    }

    if (ImGui::BeginChild("Memory", ImVec2(0, 0), ImGuiChildFlags_Border)) {
        ImPlotAxisFlags axisflags =
            ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_NoTickMarks;

        if (ImPlot::BeginPlot("Memory (GiB)", ImVec2(-1, -1),
                              ImPlotFlags_NoLegend | ImPlotFlags_NoFrame)) {
            ImPlot::SetupAxis(ImAxis_X1, nullptr, axisflags);
            ImPlot::SetupAxis(ImAxis_Y1, nullptr,
                              axisflags | ImPlotAxisFlags_Opposite);

            if (sample_timer >= 1.0f) {
                auto mem = memory.GetMemoryUsage();
                sample_timer = 0.0f;
                if (mem.count("MemTotal") && mem.count("MemAvailable")) {
                    time[index] = t;
                    memTotal = static_cast<float>(mem["MemTotal"]) / 1048576.0f;
                    memAvail =
                        static_cast<float>(mem["MemAvailable"]) / 1048576.0f;
                    usage[index] = memTotal - memAvail;
                    index = (index + 1) % BUFFERSIZE;
                    count = std::min(count + 1, BUFFERSIZE);
                } else {
                    memTotal = memTotal ? memTotal : 1.0f;
                    memAvail = 0.0f;
                }
            }

            ImPlot::SetupAxisLimits(ImAxis_X1, t - 60.0f, t - 0.9,
                                    ImPlotCond_Always);
            static float ymax = memTotal > 0 ? memTotal : 1.0f;
            ImPlot::SetupAxisLimits(ImAxis_Y1, 0, ymax, ImPlotCond_Always);

            ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
            ImPlot::PlotShaded("mem", time.data(), usage.data(), count, 0, 0);
            ImPlot::PopStyleVar();
            ImPlot::PlotLine("mem", time.data(), usage.data(), count, 0, 0);

            ImPlot::EndPlot();
            if (ImGui::BeginItemTooltip()) {
                ImGui::Text("Total: %.2fGiB", memTotal);
                ImGui::Text("Free: %.2fGiB", memAvail);
                ImGui::Text("Used: %.2fGiB", memTotal - memAvail);
                ImGui::EndTooltip();
            }
        }
    }
    ImGui::EndChild();
}
void NetworkUI(Network &network) {
    static float t = 0;
    static float sample_timer = 0.0f;
    static constexpr int BUFFERSIZE = 3600;
    static std::array<float, BUFFERSIZE> rxBuffer;
    static std::array<float, BUFFERSIZE> txBuffer;
    static std::array<float, BUFFERSIZE> time;
    static int index = 0;
    static int count = 0;
    static std::string wifi_name;
    static unsigned long prev_rx = 0;
    static unsigned long prev_tx = 0;
    static bool first_sample = true;
    static float current_rx_rate = 0.0f;
    static float current_tx_rate = 0.0f;
    static float ymax = 10.0f;  // Default to 10 KB/s to avoid zero

    float dt = ImGui::GetIO().DeltaTime;
    t += dt;
    sample_timer += dt;

    auto net = network.GetNetwork();

    if (wifi_name.empty() && !net.empty()) {
        for (const auto &iface : net) {
            if (iface.name != "lo") {
                wifi_name = iface.name;
                break;
            }
        }
        if (wifi_name.empty()) {
            wifi_name = "No Wi-Fi";
        }
    }

    if (ImGui::BeginChild("Network", ImVec2(0, 0), ImGuiChildFlags_Border)) {
        ImPlotAxisFlags axisflags =
            ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_NoTickMarks;

        if (ImPlot::BeginPlot(("Network (" + wifi_name + ")").c_str(),
                              ImVec2(-1, -1), ImPlotFlags_NoFrame)) {
            ImPlot::SetupAxis(ImAxis_X1, nullptr, axisflags);
            ImPlot::SetupAxis(ImAxis_Y1, "KB/s",
                              axisflags | ImPlotAxisFlags_Opposite);

            // Update data every second
            if (sample_timer >= 1.0f) {
                sample_timer = 0.0f;
                time[index] = t;

                // Find Wi-Fi interface data
                unsigned long curr_rx = 0;
                unsigned long curr_tx = 0;
                for (const auto &iface : net) {
                    if (iface.name == wifi_name) {
                        curr_rx = iface.rx_bytes;
                        curr_tx = iface.tx_bytes;
                        break;
                    }
                }

                float delta_rx = 0.0f;
                float delta_tx = 0.0f;
                if (!first_sample) {
                    delta_rx = static_cast<float>(curr_rx - prev_rx) /
                               1024.0f;  // KB/s
                    delta_tx = static_cast<float>(curr_tx - prev_tx) /
                               1024.0f;  // KB/s
                }
                first_sample = false;
                prev_rx = curr_rx;
                prev_tx = curr_tx;

                rxBuffer[index] = delta_rx;
                txBuffer[index] = delta_tx;
                current_rx_rate = delta_rx;
                current_tx_rate = delta_tx;

                // Update ymax based on max rate seen
                ymax = std::max({ymax, delta_rx, delta_tx});

                index = (index + 1) % BUFFERSIZE;
                count = std::min(count + 1, BUFFERSIZE);
            }

            ImPlot::SetupAxisLimits(ImAxis_X1, t - 60.0f, t - 0.9,
                                    ImPlotCond_Always);
            ImPlot::SetupAxisLimits(ImAxis_Y1, 0, ymax * 1.2f,
                                    ImPlotCond_Always);  // 20% padding

            ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
            ImPlot::PlotShaded("down", time.data(), rxBuffer.data(), count, 0,
                               0);
            ImPlot::PopStyleVar();
            ImPlot::PlotLine("down", time.data(), rxBuffer.data(), count, 0, 0);

            ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
            ImPlot::PlotShaded("up", time.data(), txBuffer.data(), count, 0, 0);
            ImPlot::PopStyleVar();
            ImPlot::PlotLine("up", time.data(), txBuffer.data(), count, 0, 0);
            // Tooltip similar to CoreUI
            if (ImGui::BeginItemTooltip()) {
                ImGui::Text("Interface: %s", wifi_name.c_str());
                ImGui::Text("RX: %.2f KB/s", current_rx_rate);
                ImGui::Text("TX: %.2f KB/s", current_tx_rate);
                ImGui::EndTooltip();
            }

            ImPlot::EndPlot();
        }
    }
    ImGui::EndChild();
}

void SetupUILayout() {
    static bool layoutInitialized = false;

    // Create a dockspace over the entire viewport
    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar();
    // Add global menu bar
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("I'm")) {
            ImGui::MenuItem("Process", nullptr,
                            nullptr);  // Process window is always visible
            ImGui::MenuItem("CPU", nullptr);
            ImGui::MenuItem("Memory", nullptr);
            ImGui::MenuItem("Network", nullptr);
            if (ImGui::MenuItem("Reset Layout")) {
                layoutInitialized =
                    false;  // Reset docking layout on next frame
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
            ImGui::MenuItem("ImGui Demo", nullptr);
            ImGui::MenuItem("ImPlot Demo", nullptr);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f),
                     ImGuiDockNodeFlags_PassthruCentralNode);

    if (!layoutInitialized) {
        // Clear any existing docking layout
        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->WorkSize);

        // Split the dockspace into left and right
        ImGuiID dock_left_id, dock_right_id;
        ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.5f,
                                    &dock_left_id, &dock_right_id);

        // Split the right side into three vertical sections for CPU, Memory,
        // and Network
        ImGuiID dock_right_top_id, dock_right_mid_id, dock_right_bottom_id;
        ImGui::DockBuilderSplitNode(dock_right_id, ImGuiDir_Up, 0.33f,
                                    &dock_right_top_id, &dock_right_mid_id);
        ImGui::DockBuilderSplitNode(dock_right_mid_id, ImGuiDir_Up, 0.5f,
                                    &dock_right_mid_id, &dock_right_bottom_id);

        // Dock windows into their respective nodes
        ImGui::DockBuilderDockWindow("Process", dock_left_id);
        ImGui::DockBuilderDockWindow("CPU", dock_right_top_id);
        ImGui::DockBuilderDockWindow("Memory", dock_right_mid_id);
        ImGui::DockBuilderDockWindow("Network", dock_right_bottom_id);

        ImGui::DockBuilderFinish(dockspace_id);
        layoutInitialized = true;
    }
    ImGui::End();
}

};  // namespace Application
