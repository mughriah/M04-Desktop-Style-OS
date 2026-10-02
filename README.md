## M04 - Desktop-Style OS

**Developers:** 
- Abenojar, Fred
- Caya, Mary Faye
- Diamante, Deo
- Guiller, Gerylyn
  
**Programming Language:** C++
**Entry File:** `main.cpp`

### Overview

A desktop-style operating system mockup developed using C++, GLFW, OpenGL, and Dear ImGui.

### Features

* Desktop interface with a blue gradient wallpaper
* Real-time clock and date
* Taskbar with clickable application buttons
* File Explorer placeholder screen
* Settings placeholder screen
* Task Manager with sample CPU and memory usage
* Power button to close the application

### Requirements

* C++ compiler (GCC)
* GLFW
* OpenGL
* Dear ImGui

### How to Run

1. Open a terminal in the project directory.

2. Make sure the Dear ImGui source files are inside the `imgui` folder.

3. Compile the program using:

   ```bash
   g++ main.cpp imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp imgui/backends/imgui_impl_glfw.cpp imgui/backends/imgui_impl_opengl3.cpp -Iimgui -Iimgui/backends -o main.exe -lglfw3 -lopengl32 -lgdi32
   ```

4. Run the program:

   ```powershell
   .\main.exe
   ```

### Entry Point

The `main()` function is located in `main.cpp`.

### Notes

* CPU and memory values in Task Manager are sample data.
* The Power button closes the mockup application. It does not shut down Windows.
