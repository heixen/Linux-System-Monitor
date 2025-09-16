#include "Application.h"

#include <array>
#include <csignal>
#include <vector>

#include "CpuManager.h"
#include "MemoryManager.h"
#include "NetworkManager.h"
#include "Process.h"
#include "ProcessList.h"
#include "imgui.h"
#include "implot.h"

namespace {
void CoreUI(CPU &cpu) {
    std::vector<Core> cores = cpu.GetCpuUsage();
    if (!cores.empty())
        cores.erase(cores.begin());

    static std::vector<Core> prevCores;

    ImGui::BeginChild(
        "Cores", ImVec2(0, 0),
        ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AutoResizeY);

    if (!prevCores.empty()) {
        for (int i = 0; i < cores.size(); i++) {
            const Core &currentCore = cores[i];
            const Core &previousCore = prevCores[i];
            int currentCoreIdle = currentCore.stat[3] + currentCore.stat[4];
            int currentCoreTotal = 0;
            for (int i : currentCore.stat)
                currentCoreTotal += i;
            int previousCoreIdle = previousCore.stat[3] + previousCore.stat[4];
            int previousCoreTotal = 0;
            for (int i : previousCore.stat)
                previousCoreTotal += i;
            int TotalDiff = currentCoreTotal - previousCoreTotal;
            int IdleDiff = currentCoreIdle - previousCoreIdle;
            float coreUsage = 0.0f;
            if (TotalDiff > 0)
                coreUsage =
                    (float)(TotalDiff - IdleDiff) / (float)TotalDiff * 100.0f;
            else
                coreUsage = 0;
            ImGui::Text("Core %d: %.1f%% race condition error me thinks", i,
                        coreUsage);
        }
    } else {
        for (int i = 0; i < cores.size(); i++) {
            ImGui::Text("Core %d: 0.0%%", i);
        }
    }
    ImGui::EndChild();
    prevCores = cores;
}

}  // namespace
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
            filter.Draw("Filter");
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
    static ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels |
                                   ImPlotAxisFlags_NoGridLines |
                                   ImPlotAxisFlags_NoHighlight;

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
                                ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100);

        if (sample_timer >= 1.0f) {
            float cpuUsage = cpu.GetCpuPercentage();
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
        if (ImGui::BeginItemTooltip()) {
            CoreUI(cpu);
            ImGui::EndTooltip();
        }
        ImPlot::EndPlot();
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
        if (memT.count("MemTotal"))
            memTotal = memT["MemTotal"];
        else {
            memTotal = 1;
            memAvail = 0;
        }
        firstTime = false;
    }

    if (ImGui::BeginChild("Memory", ImVec2(0, 0), ImGuiChildFlags_Border)) {
        ImPlotAxisFlags axisflags =
            ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_NoTickMarks;

        if (ImPlot::BeginPlot("Memory", ImVec2(-1, -1),
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
            static float ymax =
                (memTotal > 0) ? (float)memTotal / 1048576.0f : 1;
            ImPlot::SetupAxisLimits(ImAxis_Y1, 0, ymax);

            ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
            ImPlot::PlotShaded("mem", time.data(), usage.data(), count, 0, 0);
            ImPlot::PopStyleVar();
            ImPlot::PlotLine("mem", time.data(), usage.data(), count, 0, 0);
            ImPlot::EndPlot();
        }
    }
    ImGui::EndChild();
}

};  // namespace Application
