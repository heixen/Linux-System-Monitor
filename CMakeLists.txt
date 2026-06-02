cmake_minimum_required(VERSION 3.25)

project(
  linux-resource-monitor
  VERSION 1.0
  DESCRIPTION "Linux Resource Monitor"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
find_package(OpenGL REQUIRED)

message(STATUS "SDL2 Include Dirs: ${SDL2_INCLUDE_DIRS}")
message(STATUS "SDL2 Libraries: ${SDL2_LIBRARIES}")
message(STATUS "OpenGL Include Dirs: ${OPENGL_INCLUDE_DIR}")
message(STATUS "OpenGL Libraries: ${OPENGL_LIBRARIES}")
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

# ImGui library
add_library(
  imgui STATIC
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/imgui.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/imgui_demo.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/imgui_draw.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/imgui_tables.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/imgui_widgets.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/imgui_impl_sdl2.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/imgui_impl_opengl3.cpp)

target_include_directories(
  imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui ${SDL2_INCLUDE_DIRS}
               ${OPENGL_INCLUDE_DIR})

target_link_libraries(imgui PUBLIC ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES})

# Define for OpenGL3 backend
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GL3W)

# ImPlot library
add_library(
  implot STATIC
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/implot/implot.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/implot/implot_items.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/implot/implot_demo.cpp)

target_include_directories(
  implot PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/implot
                ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui)

target_link_libraries(implot PUBLIC imgui)

# Main executable
add_executable(
  ${PROJECT_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/CpuManager.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/CpuParser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/MemoryManager.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/MemoryParser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/ProcessManager.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/ProcessParser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/NetworkManager.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/NetParser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Manager.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/LRM/Application.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/LRM/ProcessList.cpp)
target_include_directories(
  ${PROJECT_NAME}
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
          ${CMAKE_CURRENT_SOURCE_DIR}/src/LRM
          ${CMAKE_CURRENT_SOURCE_DIR}/src/Core
          ${CMAKE_CURRENT_SOURCE_DIR}/src/Models
          ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui
          ${CMAKE_CURRENT_SOURCE_DIR}/lib/implot
          ${SDL2_INCLUDE_DIRS}
          ${OPENGL_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE imgui implot ${SDL2_LIBRARIES}
                                              ${OPENGL_LIBRARIES})

# Compiler flags
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -g)

# Test executable (commented out) add_executable( test
# ${CMAKE_CURRENT_SOURCE_DIR}/src/test.cpp
# ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/NetParser.cpp )
# target_include_directories( test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/Core
# ${CMAKE_CURRENT_SOURCE_DIR}/src/Models ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui
# ${CMAKE_CURRENT_SOURCE_DIR}/lib/implot ${SDL2_INCLUDE_DIRS}
# ${OPENGL_INCLUDE_DIR} ) target_link_libraries(test PRIVATE imgui implot
# ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES}) target_compile_options(test PRIVATE
# -Wall -Wextra -g)
