#pragma once

#include <vector>

#include "CpuManager.h"
#include "MemoryManager.h"
#include "NetworkManager.h"
#include "Process.h"

namespace Application {
void ProcessUI(std::vector<Process>& processes);
void CpuUI(CPU& cpu);
void MemoryUI(Memory& memory);
void NetworkUI(NetworkManager& network);
};  // namespace Application
