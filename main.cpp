// CSOPESY - GROUP 8
// Version Date: October 2, 2026
// Developers: Abenojar, Caya, Diamante, Guiller


#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <sstream>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>

struct Process {
    std::string name;
    float cpu;
    float memory;
};

int main() {
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW\n";
        return -1;
    }

    // Create the application window
    GLFWwindow* window = glfwCreateWindow(
        1280, 720, "M04 - Desktop OS Mockup", NULL, NULL
    );

    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Initialize Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Application state
    bool showFiles = false;
    bool showSettings = false;
    bool showTaskManager = false;
    bool running = true;

    std::vector<Process> processes = {
        {"System", 2.4f, 120.0f},
        {"File Explorer", 1.8f, 85.0f},
        {"Desktop Window Manager", 3.2f, 140.0f},
        {"Browser", 5.6f, 320.0f},
        {"Background Service", 0.8f, 64.0f}
    };

    while (!glfwWindowShouldClose(window) && running) {
        glfwPollEvents();

        // Start a new ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGuiIO& frameIO = ImGui::GetIO();
        ImVec2 screenSize = frameIO.DisplaySize;

        // Desktop background
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(screenSize);

        ImGui::Begin("Desktop",
            nullptr,
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoBringToFrontOnFocus);

        ImDrawList* draw = ImGui::GetWindowDrawList();

        // Draw a simple blue gradient-style wallpaper
        draw->AddRectFilledMultiColor(
            ImVec2(0, 0),
            screenSize,
            IM_COL32(20, 80, 160, 255),
            IM_COL32(50, 140, 220, 255),
            IM_COL32(10, 35, 100, 255),
            IM_COL32(25, 75, 150, 255)
        );

        // Desktop title
        ImGui::SetCursorPos(ImVec2(35, 30));
        ImGui::SetWindowFontScale(1.6f);
        ImGui::TextColored(
            ImVec4(1, 1, 1, 1),
            "My Desktop"
        );
        ImGui::SetWindowFontScale(1.0f);

        // Real-time clock
        std::time_t now = std::time(nullptr);
        std::tm localTime{};

#ifdef _WIN32
        localtime_s(&localTime, &now);
#else
        localtime_r(&now, &localTime);
#endif

        std::ostringstream timeText;
        timeText << std::put_time(&localTime, "%I:%M:%S %p");

        std::ostringstream dateText;
        dateText << std::put_time(&localTime, "%B %d, %Y");

        ImGui::SetCursorPos(ImVec2(screenSize.x - 190, 25));
        ImGui::Text("%s", timeText.str().c_str());

        ImGui::SetCursorPos(ImVec2(screenSize.x - 190, 48));
        ImGui::Text("%s", dateText.str().c_str());

        // Taskbar
        float taskbarHeight = 65.0f;
        ImGui::SetCursorPos(ImVec2(0, screenSize.y - taskbarHeight));

        ImGui::PushStyleColor(
            ImGuiCol_ChildBg,
            IM_COL32(15, 25, 45, 235)
        );

        ImGui::BeginChild(
            "Taskbar",
            ImVec2(screenSize.x, taskbarHeight),
            false
        );

        ImGui::SetCursorPos(ImVec2(15, 12));

        if (ImGui::Button("Files", ImVec2(110, 40))) {
            showFiles = !showFiles;
            showSettings = false;
            showTaskManager = false;
        }

        ImGui::SameLine();

        if (ImGui::Button("Settings", ImVec2(110, 40))) {
            showSettings = !showSettings;
            showFiles = false;
            showTaskManager = false;
        }

        ImGui::SameLine();

        if (ImGui::Button("Task Manager", ImVec2(140, 40))) {
            showTaskManager = !showTaskManager;
            showFiles = false;
            showSettings = false;
        }

        ImGui::SameLine();
        ImGui::SetCursorPosX(screenSize.x - 95);

        if (ImGui::Button("PWR", ImVec2(75, 40))) {
            running = false;
        }

        ImGui::EndChild();
        ImGui::PopStyleColor();

        ImGui::End();

        // Files screen
        if (showFiles) {
            ImGui::SetNextWindowSize(ImVec2(450, 300), ImGuiCond_FirstUseEver);

            if (ImGui::Begin("File Explorer", &showFiles)) {
                ImGui::Text("File Explorer");
                ImGui::Separator();
                ImGui::BulletText("Documents");
                ImGui::BulletText("Downloads");
                ImGui::BulletText("Pictures");
                ImGui::BulletText("Desktop");
            }
            ImGui::End();
        }

        // Settings screen
        if (showSettings) {
            ImGui::SetNextWindowSize(ImVec2(450, 300), ImGuiCond_FirstUseEver);

            if (ImGui::Begin("Settings", &showSettings)) {
                ImGui::Text("Desktop Settings");
                ImGui::Separator();
                ImGui::Text("Display");
                ImGui::Text("Personalization");
                ImGui::Text("System Information");
                ImGui::Text("This is a placeholder settings screen.");
            }
            ImGui::End();
        }

        // Task Manager screen
        if (showTaskManager) {
            ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);

            if (ImGui::Begin("Task Manager", &showTaskManager)) {
                ImGui::Text("Processes");
                ImGui::Separator();

                if (ImGui::BeginTable(
                    "ProcessTable",
                    3,
                    ImGuiTableFlags_Borders |
                    ImGuiTableFlags_RowBg |
                    ImGuiTableFlags_Resizable
                )) {
                    ImGui::TableSetupColumn("Process Name");
                    ImGui::TableSetupColumn("CPU (%)");
                    ImGui::TableSetupColumn("Memory (MB)");
                    ImGui::TableHeadersRow();

                    for (const Process& process : processes) {
                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("%s", process.name.c_str());

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%.1f", process.cpu);

                        ImGui::TableSetColumnIndex(2);
                        ImGui::Text("%.1f", process.memory);
                    }

                    ImGui::EndTable();
                }

                ImGui::Spacing();
                ImGui::Text("Note: CPU and memory values are sample data.");
            }

            ImGui::End();
        }

        // Render everything
        ImGui::Render();

        int displayWidth, displayHeight;
        glfwGetFramebufferSize(window, &displayWidth, &displayHeight);

        glViewport(0, 0, displayWidth, displayHeight);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Clean up
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}