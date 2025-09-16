cmake_minimum_required(VERSION 3.31)

project(
  linux-resource-monitor
  VERSION 1.0
  DESCRIPTION "Linux Resource Monitor"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_library(
  imgui STATIC
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/imgui.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/imgui_draw.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/imgui_tables.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/imgui_widgets.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/imgui_demo.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/imgui_impl_glfw.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/imgui_impl_opengl3.cpp)
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui)

target_link_libraries(imgui PUBLIC OpenGL::GL glfw)

add_library(
  implot STATIC
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/implot/implot.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/implot/implot_items.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/implot/implot_demo.cpp)

target_include_directories(implot PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui
                                         ${CMAKE_CURRENT_SOURCE_DIR}/lib/implot)

target_link_libraries(implot PUBLIC imgui)

# add_executable( ${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
# ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/CpuManager.cpp
# ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/CpuParser.cpp
# ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/MemoryManager.cpp
# ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/MemoryParser.cpp
# ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/ProcessManager.cpp
# ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/ProcessParser.cpp
# ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/NetworkManager.cpp
# ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/NetParser.cpp
# ${CMAKE_CURRENT_SOURCE_DIR}/src/LRM/Application.cpp
# ${CMAKE_CURRENT_SOURCE_DIR}/src/LRM/ProcessList.cpp)
#
# target_include_directories( ${PROJECT_NAME} PRIVATE
# ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/src/LRM
# ${CMAKE_CURRENT_SOURCE_DIR}/src/Core ${CMAKE_CURRENT_SOURCE_DIR}/src/Models)
#
# target_link_libraries(${PROJECT_NAME} PUBLIC imgui implot) add_executable(test
# ${CMAKE_CURRENT_SOURCE_DIR}/src/test.cpp
# ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/NetParser.cpp)
# target_include_directories( test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/Core
# ${CMAKE_CURRENT_SOURCE_DIR}/src/Models ${CMAKE_CURRENT_SOURCE_DIR}/src/LRM)
