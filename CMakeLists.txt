cmake_minimum_required(VERSION 3.5)
# set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BINARY_DIR build)
project(raytracer C CXX)

### Libraries
find_package(SDL2 REQUIRED)
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
set(LIBS ${OPENGL_LIBRARY} ${SDL2_LIBRARY})

### Add 3rd-party headers
set(include_dir include)
file(GLOB_RECURSE ${include_dir}/*.h)
include_directories(${include_dir} ${include_dir}/stb)

### ImGui
set(IMGUI_DIR vendor/imgui)
file(GLOB imgui_src ${IMGUI_DIR}/*.cpp ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp)
file(GLOB imgui_headers ${IMGUI_DIR}/*.h ${IMGUI_DIR}/backends/imgui_impl_sdl2.h ${IMGUI_DIR}/backends/imgui_impl_opengl3.h)
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends)

### Source files
set(source
  src/main.cpp
  src/Application.cpp
  src/Camera.cpp
  src/Shader.cpp
  src/GUI.cpp)

set(source_headers
  src/EntryPoint.h
  src/Application.h
  src/Camera.h
  src/Shader.h
  src/Shape.h
  src/Clock.h
  src/GUI.h)

### Build exe and link libraries 
set(EXE out)
include_directories(src)
add_executable(${EXE} ${source} ${source_headers} ${imgui_src} ${imgui_headers} ${include_dir})
target_link_libraries(${EXE} PUBLIC ${LIBS} "SDL2::SDL2" PRIVATE "GLEW;GLU")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23")
