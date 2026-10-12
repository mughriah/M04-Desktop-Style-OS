CSOPESY M04 - Desktop-Style OS Mockup
Version date: October 12, 2026

DEVELOPERS
Abenojar, Fred
Caya, Mary Faye
Diamante, Deo
Guiller, Gerylyn

ENTRY FILE
CSOPESY-M04/CSOPESY-M04/CSOPESY-M04.cpp contains main().
Solution: CSOPESY-M04/CSOPESY-M04.slnx

REQUIREMENTS
Windows, Visual Studio 2026 with Desktop development with C++, Dear ImGui,
and a GLFW library built for x64 and the compatible Visual C++ toolset.
The project uses the v145 toolset and C++20.
The configuration verified on the development computer is Debug | x64.

SETUP
1. Open CSOPESY-M04/CSOPESY-M04.slnx in Visual Studio.
2. Select Debug and x64 in the toolbar.
3. The project currently references developers' local library folders.
   If those folders do not exist on your computer, configure your own copies:
   - Project Properties > C/C++ > General > Additional Include Directories:
     add the Dear ImGui root, its backends folder, and GLFW's include folder.
   - Linker > General > Additional Library Directories:
     add the GLFW folder containing the compatible x64 glfw3.lib file.
   - Linker > Input > Additional Dependencies:
     include glfw3.lib and opengl32.lib, keeping inherited dependencies.
   - Ensure the project compiles these six files from your Dear ImGui copy:
     imgui.cpp
     imgui_draw.cpp
     imgui_tables.cpp
     imgui_widgets.cpp
     backends/imgui_impl_glfw.cpp
     backends/imgui_impl_opengl3.cpp
     If existing references point to missing files, remove only those project
     references and add the corresponding existing files from your copy.
     Do not delete the library files or add duplicate source references.
4. Select Build > Build Solution.
5. Press F5 or click Local Windows Debugger to run.

HOW TO USE
- Wait for the approximately three-second boot screen to finish.
- The landscape wallpaper has a gradient sky, sun, filled hills, and flowers.
- Application windows start in staggered positions each launch; you can still
  move and resize them. The interface uses a larger built-in font.
- Hover over taskbar buttons to see a short explanation.
- Apps opens an applications placeholder with shortcuts to the other windows.
- Files opens a file explorer placeholder showing sample folder names.
- Task Manager opens a table of dummy processes, CPU usage, and memory usage.
- An open application's taskbar button has a blue underline, even when its
  window is behind another window.
- Click its taskbar button again or its window's X to close that window.
- The desktop clock shows the current local system date and time.
- Use PWR to shut down the emulator.

This is a GUI mockup. It does not implement real file operations, process
management, or live CPU/memory monitoring.

IMPLEMENTATION OVERVIEW
GLFW creates the window and handles events. Each frame, Dear ImGui builds the
interface and its OpenGL backend renders it. After the boot screen, the desktop
background is drawn on the background layer, followed by the application
windows, taskbar, and clock. Boolean flags track which application windows
are open. PWR ends the main loop, followed by ImGui and GLFW cleanup.
