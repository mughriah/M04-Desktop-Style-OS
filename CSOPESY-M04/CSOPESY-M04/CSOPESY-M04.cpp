// CSOPESY - M04
// DEVELOPERS: Abenojar, Caya, Diamante, Guiller
// VERSION DATE: October 9, 2026

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

#include <ctime>
#include <string>
#include <cstdio>

static bool showFiles = false;
static bool showGallery = false;
static bool showTaskManager = false;
static bool running = true;
static double bootStart = 0.0;

// Get the current system date and time
static std::string getClock()
{
    std::time_t now = std::time(nullptr);
    std::tm localTime{};

#ifdef _WIN32
    localtime_s(&localTime, &now);
#else
    localtime_r(&now, &localTime);
#endif

    char buffer[64];

    std::strftime(
        buffer,
        sizeof(buffer),
        "%A, %b %d, %Y | %I:%M:%S %p",
        &localTime
    );

    return buffer;
}

// Draw desktop wallpaper
static void drawDesktopBackground()
{
    ImDrawList* bg = ImGui::GetBackgroundDrawList();
    ImVec2 size = ImGui::GetIO().DisplaySize;

    // Sky gradient
    bg->AddRectFilledMultiColor(
        ImVec2(0, 0),
        size,
        IM_COL32(35, 110, 190, 255),
        IM_COL32(80, 170, 235, 255),
        IM_COL32(20, 65, 110, 255),
        IM_COL32(25, 100, 160, 255)
    );

    // Stylized green hills
    bg->AddBezierCubic(
        ImVec2(0, size.y * 0.65f),
        ImVec2(size.x * 0.28f, size.y * 0.48f),
        ImVec2(size.x * 0.62f, size.y * 0.83f),
        ImVec2(size.x, size.y * 0.60f),
        IM_COL32(90, 175, 70, 255),
        8.0f
    );

    bg->AddRectFilled(
        ImVec2(0, size.y * 0.72f),
        ImVec2(size.x, size.y),
        IM_COL32(30, 105, 45, 255)
    );

    bg->AddText(
        ImVec2(18, 18),
        IM_COL32(20, 255, 100, 255),
        "CSOPESY OS v1.0"
    );
}

// Draw desktop taskbar
static void drawTaskbar()
{
    ImGuiIO& io = ImGui::GetIO();
    const float barHeight = 56.0f;
    const float sidePadding = 10.0f;

    ImGui::SetNextWindowPos(
        ImVec2(0, io.DisplaySize.y - barHeight),
        ImGuiCond_Always
    );

    ImGui::SetNextWindowSize(
        ImVec2(io.DisplaySize.x, barHeight),
        ImGuiCond_Always
    );

    ImGui::PushStyleVar(
        ImGuiStyleVar_WindowRounding, 0.0f
    );

    ImGui::PushStyleVar(
        ImGuiStyleVar_WindowPadding,
        ImVec2(sidePadding, 8.0f)
    );

    ImGui::PushStyleColor(
        ImGuiCol_WindowBg,
        IM_COL32(20, 20, 32, 235)
    );

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar;

    if (ImGui::Begin("Taskbar", nullptr, flags))
    {
        // TASK MANAGER button on the bottom-left
        if (ImGui::Button("TASK MANAGER", ImVec2(125, 36)))
        {
            showTaskManager = !showTaskManager;
        }

        // Move the PWR button to the bottom-right
        float powerButtonWidth = 55.0f;

        ImGui::SameLine();

        ImGui::SetCursorPosX(
            ImGui::GetWindowWidth() -
            powerButtonWidth -
            sidePadding * 2.0f
        );

        if (ImGui::Button("PWR", ImVec2(powerButtonWidth, 36)))
        {
            running = false;
        }
    }

    ImGui::End();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
}

// File Explorer window
static void drawFileWindow()
{
    if (!showFiles)
        return;

    ImGui::SetNextWindowSize(
        ImVec2(440, 300),
        ImGuiCond_FirstUseEver
    );

    if (ImGui::Begin("CSOPESY File Explorer", &showFiles))
    {
        ImGui::Text("Quick Access");
        ImGui::Separator();

        ImGui::BulletText("Desktop");
        ImGui::BulletText("Documents");
        ImGui::BulletText("Downloads");
        ImGui::BulletText("Pictures");
        ImGui::BulletText("This PC");

        ImGui::Spacing();

        ImGui::TextDisabled(
            "Placeholder file explorer - sample interface."
        );
    }

    ImGui::End();
}

// Applications window
static void drawGalleryWindow()
{
    if (!showGallery)
        return;

    ImGui::SetNextWindowSize(
        ImVec2(430, 280),
        ImGuiCond_FirstUseEver
    );

    if (ImGui::Begin("CSOPESY Applications", &showGallery))
    {
        ImGui::Text("Welcome to CSOPESY OS");
        ImGui::Separator();

        ImGui::TextWrapped(
            "This is a simulated desktop environment "
            "built with C++, GLFW, OpenGL and Dear ImGui."
        );

        ImGui::Spacing();

        if (ImGui::Button("Open File Explorer"))
            showFiles = true;

        if (ImGui::Button("Open Task Manager"))
            showTaskManager = true;

        ImGui::Spacing();

        ImGui::TextDisabled("System status: Running");
    }

    ImGui::End();
}

// Task Manager window
static void drawTaskManager()
{
    if (!showTaskManager)
        return;

    ImGui::SetNextWindowSize(
        ImVec2(540, 330),
        ImGuiCond_FirstUseEver
    );

    if (ImGui::Begin("Task Manager", &showTaskManager))
    {
        ImGui::Text("Processes");
        ImGui::Separator();

        if (ImGui::BeginTable(
            "Processes",
            3,
            ImGuiTableFlags_Borders |
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_Resizable
        ))
        {
            ImGui::TableSetupColumn("Process");
            ImGui::TableSetupColumn("CPU");
            ImGui::TableSetupColumn("Memory");

            ImGui::TableHeadersRow();

            const char* names[] = {
                "CSOPESY Desktop",
                "Window Manager",
                "System UI",
                "File Explorer",
                "OpenGL Renderer"
            };

            const char* cpu[] = {
                "4.2%", "1.1%", "0.8%", "2.4%", "3.7%"
            };

            const char* memory[] = {
                "120 MB", "45 MB", "32 MB", "65 MB", "90 MB"
            };

            for (int i = 0; i < 5; ++i)
            {
                ImGui::TableNextRow();

                ImGui::TableNextColumn();
                ImGui::TextUnformatted(names[i]);

                ImGui::TableNextColumn();
                ImGui::TextUnformatted(cpu[i]);

                ImGui::TableNextColumn();
                ImGui::TextUnformatted(memory[i]);
            }

            ImGui::EndTable();
        }

        ImGui::Spacing();

        ImGui::TextDisabled(
            "Sample values only; not actual system statistics."
        );
    }

    ImGui::End();
}

// Draw the centered boot screen
static void drawBootScreen()
{
    ImGuiIO& io = ImGui::GetIO();

    ImGui::SetNextWindowPos(
        ImVec2(0, 0),
        ImGuiCond_Always
    );

    ImGui::SetNextWindowSize(
        io.DisplaySize,
        ImGuiCond_Always
    );

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoBackground;

    if (ImGui::Begin("Boot Screen", nullptr, flags))
    {
        float contentWidth = ImGui::GetContentRegionAvail().x;
        float leftPadding = ImGui::GetStyle().WindowPadding.x;

        // Center the CSOPESY title
        ImGui::SetCursorPosY(io.DisplaySize.y * 0.30f);

        const char* logo = "CSOPESY";
        const float logoScale = 2.5f;

        float logoWidth =
            ImGui::CalcTextSize(logo).x * logoScale;

        ImGui::SetCursorPosX(
            leftPadding + (contentWidth - logoWidth) * 0.5f
        );

        ImGui::SetWindowFontScale(logoScale);

        ImGui::PushStyleColor(
            ImGuiCol_Text,
            IM_COL32(0, 220, 255, 255)
        );

        ImGui::TextUnformatted(logo);

        ImGui::PopStyleColor();

        ImGui::SetWindowFontScale(1.0f);

        // Center the subtitle
        const char* subtitle =
            "CSOPESY Operating System Emulator";

        ImGui::SetCursorPosX(
            leftPadding +
            (contentWidth - ImGui::CalcTextSize(subtitle).x) * 0.5f
        );

        ImGui::TextUnformatted(subtitle);

        ImGui::Spacing();

        // Center the developer names
        const char* developers =
            "Developers: Abenojar, Caya, Diamante, Guiller";

        ImGui::SetCursorPosX(
            leftPadding +
            (contentWidth - ImGui::CalcTextSize(developers).x) * 0.5f
        );

        ImGui::TextDisabled("%s", developers);

        ImGui::Spacing();

        // Center the version date
        const char* versionDate =
            "Version Date: October 9, 2026";

        ImGui::SetCursorPosX(
            leftPadding +
            (contentWidth - ImGui::CalcTextSize(versionDate).x) * 0.5f
        );

        ImGui::TextDisabled("%s", versionDate);

        // Center the loading message
        const char* loading = "Loading...";

        ImGui::SetCursorPosY(io.DisplaySize.y * 0.65f);

        ImGui::SetCursorPosX(
            leftPadding +
            (contentWidth - ImGui::CalcTextSize(loading).x) * 0.5f
        );

        ImGui::TextColored(
            ImVec4(0.2f, 1.0f, 0.3f, 1.0f),
            "%s",
            loading
        );
    }

    ImGui::End();
}

// Draw the live date and time at the top-right
static void drawTopRightClock()
{
    ImGuiIO& io = ImGui::GetIO();

    const float rightMargin = 18.0f;
    const float topMargin = 12.0f;

    std::string clockText = getClock();
    ImVec2 clockSize = ImGui::CalcTextSize(clockText.c_str());

    ImGui::SetNextWindowPos(
        ImVec2(
            io.DisplaySize.x - clockSize.x - rightMargin - 10.0f,
            topMargin
        ),
        ImGuiCond_Always
    );

    ImGui::SetNextWindowSize(
        ImVec2(clockSize.x + 20.0f, 34.0f),
        ImGuiCond_Always
    );

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoFocusOnAppearing;

    if (ImGui::Begin("Desktop Clock", nullptr, flags))
    {
        ImGui::SetCursorPosY(7.0f);
        ImGui::TextUnformatted(clockText.c_str());
    }

    ImGui::End();
}

// Main program
int main()
{
    if (!glfwInit())
        return 1;

    // Create OpenGL 3.0 context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(
        1280,
        800,
        "CSOPESY Desktop OS Emulator",
        nullptr,
        nullptr
    );

    if (!window)
    {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Initialize Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 8.0f;
    style.FrameRounding = 6.0f;
    style.WindowBorderSize = 1.0f;

    // Initialize GLFW and OpenGL backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    bootStart = glfwGetTime();

    // Main application loop
    while (!glfwWindowShouldClose(window) && running)
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        double elapsed = glfwGetTime() - bootStart;

        if (elapsed < 3.0)
        {
            // Display the boot screen for 3 seconds
            drawBootScreen();
        }
        else
        {
            // Main desktop
            drawDesktopBackground();
            drawFileWindow();
            drawGalleryWindow();
            drawTaskManager();

            // Task Manager and PWR share the bottom taskbar
            drawTaskbar();

            // Date and time stay at the top-right
            drawTopRightClock();
        }

        // Render frame
        ImGui::Render();

        int displayWidth, displayHeight;

        glfwGetFramebufferSize(
            window,
            &displayWidth,
            &displayHeight
        );

        glViewport(0, 0, displayWidth, displayHeight);

        glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(
            ImGui::GetDrawData()
        );

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