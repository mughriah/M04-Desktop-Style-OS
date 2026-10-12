// CSOPESY - M04
// DEVELOPERS: Abenojar, Caya, Diamante, Guiller
// VERSION DATE: October 12, 2026

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

#include <ctime>
#include <string>
#include <cstdio>
#include <algorithm>
#include <cmath>

static bool showFiles = false;       // Files window state
static bool showGallery = false;     // Gallery window state
static bool showTaskManager = false; // Task Manager window state
static bool running = true;          // Controls the main program loop
static double bootStart = 0.0;       // Stores the startup timestamp


// Get the current system date and time
static std::string getClock()
{
	std::time_t now = std::time(nullptr);   // get raw time
	std::tm localTime{};                    // create a tm structure to hold local time

// if statement to check for Windows platform and
// use the appropriate function to convert raw time to local time
#ifdef _WIN32
    localtime_s(&localTime, &now);
#else
    localtime_r(&now, &localTime);
#endif

	char buffer[64];    // buffer to hold the formatted date and time string

	// format the local time into a human-readable string
    std::strftime(
        buffer,
        sizeof(buffer),
        "%A, %b %d, %Y | %I:%M:%S %p",
        &localTime
    );

    return buffer;
}

// Fill a curved hill down to the bottom of the desktop.
static void drawHill(ImDrawList* draw, ImVec2 size,
    float start, float control1, float control2, float end, ImU32 color)
{
    draw->PathLineTo(ImVec2(0, size.y * start));
    draw->PathBezierCubicCurveTo(
        ImVec2(size.x / 3.0f, size.y * control1),
        ImVec2(size.x * 2.0f / 3.0f, size.y * control2),
        ImVec2(size.x, size.y * end), 64);
    draw->PathLineTo(size);
    draw->PathLineTo(ImVec2(0, size.y));
    draw->PathFillConcave(color);
}

// Small flowers use only stems, leaves, and circular petals.
static void drawFlower(ImDrawList* draw, ImVec2 center, float scale, ImU32 petals)
{
    draw->AddLine(center, ImVec2(center.x, center.y + 25 * scale),
        IM_COL32(35, 100, 62, 255), 2.0f * scale);
    draw->AddTriangleFilled(ImVec2(center.x, center.y + 17 * scale),
        ImVec2(center.x - 10 * scale, center.y + 8 * scale),
        ImVec2(center.x - 8 * scale, center.y + 17 * scale),
        IM_COL32(65, 139, 67, 255));
    for (int petal = 0; petal < 5; ++petal)
    {
        float angle = petal * 6.2831853f / 5.0f - 1.5707963f;
        draw->AddCircleFilled(ImVec2(center.x + std::cos(angle) * 6 * scale,
            center.y + std::sin(angle) * 6 * scale), 4.5f * scale, petals);
    }
    draw->AddCircleFilled(center, 3.0f * scale, IM_COL32(245, 184, 62, 255));
}

// Draw the landscape first so every application remains above it.
static void drawDesktopBackground()
{
    ImDrawList* bg = ImGui::GetBackgroundDrawList();
    ImVec2 size = ImGui::GetIO().DisplaySize;
    float scale = std::min(size.x / 1280.0f, size.y / 800.0f);

    // Corner colors run clockwise: upper-left, upper-right, lower-right, lower-left.
    bg->AddRectFilledMultiColor(ImVec2(0, 0), size,
        IM_COL32(55, 133, 197, 255), IM_COL32(96, 171, 218, 255),
        IM_COL32(214, 236, 220, 255), IM_COL32(166, 214, 223, 255));

    // The sun stays below the clock; translucent circles soften its glow.
    ImVec2 sun(size.x * 0.79f, size.y * 0.23f);
    for (int ring = 6; ring > 0; --ring)
        bg->AddCircleFilled(sun, (40.0f + ring * 7.0f) * scale,
            IM_COL32(255, 224, 139, 10), 64);
    bg->AddCircleFilled(sun, 40.0f * scale, IM_COL32(255, 222, 126, 255), 64);
    bg->AddCircleFilled(ImVec2(sun.x - 4 * scale, sun.y - 5 * scale),
        32.0f * scale, IM_COL32(255, 232, 155, 255), 64);

    drawHill(bg, size, 0.66f, 0.30f, 0.83f, 0.56f, IM_COL32(137, 184, 111, 255));
    drawHill(bg, size, 0.76f, 0.92f, 0.43f, 0.71f, IM_COL32(79, 147, 91, 255));

    // Sparse flowers leave the center of the desktop uncluttered.
    const float flowerX[] = { 0.045f, 0.082f, 0.12f, 0.88f, 0.923f, 0.96f };
    const float flowerOffset[] = { 0, 18, -4, 9, -5, 20 };
    const ImU32 flowerColors[] = {
        IM_COL32(255, 188, 205, 255), IM_COL32(255, 243, 205, 255),
        IM_COL32(255, 221, 104, 255)
    };
    for (int i = 0; i < 6; ++i)
        drawFlower(bg, ImVec2(size.x * flowerX[i],
            size.y - 96.0f - flowerOffset[i] * scale), scale, flowerColors[i % 3]);

    bg->AddText(ImVec2(18, 18), IM_COL32(240, 250, 255, 255), "CSOPESY OS v1.0");
}

// Apply a staggered starting layout once per launch; users can still move/resize.
static void setAppWindowLayout(ImVec2 position, ImVec2 desiredSize)
{
    ImVec2 screen = ImGui::GetIO().DisplaySize;
    ImVec2 size(std::min(desiredSize.x, std::max(240.0f, screen.x - 32.0f)),
        std::min(desiredSize.y, std::max(180.0f, screen.y - 140.0f)));
    position.x = std::max(16.0f, std::min(position.x, screen.x - size.x - 16.0f));
    position.y = std::max(64.0f, std::min(position.y, screen.y - size.y - 72.0f));
    ImGui::SetNextWindowPos(position, ImGuiCond_Once);
    ImGui::SetNextWindowSize(size, ImGuiCond_Once);
}

enum class TaskbarIcon { Apps, Files, TaskManager };

// Keep normal ImGui button behavior, with a small drawn icon beside the label.
static bool drawTaskbarButton(const char* label, TaskbarIcon icon, bool isOpen)
{
    const float iconSize = 16.0f;
    const float padding = 10.0f;
    const float gap = 8.0f;
    const float height = 36.0f;
    const float width = padding * 2.0f + iconSize + gap + ImGui::CalcTextSize(label).x;

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
        ImVec2(padding, ImGui::GetStyle().FramePadding.y));
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(1.0f, 0.5f));
    bool clicked = ImGui::Button(label, ImVec2(width, height));
    ImGui::PopStyleVar(2);

    ImDrawList* draw = ImGui::GetWindowDrawList();
    ImVec2 origin = ImGui::GetItemRectMin();
    origin.x += padding;
    origin.y += (height - iconSize) * 0.5f;
    const ImU32 color = ImGui::GetColorU32(ImGuiCol_Text);

    switch (icon)
    {
    case TaskbarIcon::Apps:
        // Four tiles form the applications grid.
        for (int row = 0; row < 2; ++row)
            for (int column = 0; column < 2; ++column)
            {
                ImVec2 tile(origin.x + column * 10.0f, origin.y + row * 10.0f);
                draw->AddRectFilled(tile, ImVec2(tile.x + 6, tile.y + 6), color, 1.0f);
            }
        break;
    case TaskbarIcon::Files:
        // A raised tab and rectangular body form the folder.
        draw->AddRectFilled(ImVec2(origin.x, origin.y + 2),
            ImVec2(origin.x + 7, origin.y + 7), color, 1.0f);
        draw->AddRectFilled(ImVec2(origin.x, origin.y + 5),
            ImVec2(origin.x + 16, origin.y + 14), color, 1.0f);
        break;
    case TaskbarIcon::TaskManager:
        // Three bars inside a frame represent process statistics.
        draw->AddRect(origin, ImVec2(origin.x + 16, origin.y + 16), color, 1.0f);
        draw->AddRectFilled(ImVec2(origin.x + 3, origin.y + 8),
            ImVec2(origin.x + 5, origin.y + 13), color);
        draw->AddRectFilled(ImVec2(origin.x + 7, origin.y + 4),
            ImVec2(origin.x + 9, origin.y + 13), color);
        draw->AddRectFilled(ImVec2(origin.x + 11, origin.y + 6),
            ImVec2(origin.x + 13, origin.y + 13), color);
        break;
    }

    // A short underline marks an open window, including when it is behind another.
    if (isOpen)
    {
        ImVec2 buttonMin = ImGui::GetItemRectMin();
        ImVec2 buttonMax = ImGui::GetItemRectMax();
        float centerX = (buttonMin.x + buttonMax.x) * 0.5f;
        draw->AddRectFilled(ImVec2(centerX - 12.0f, buttonMax.y + 2.0f),
            ImVec2(centerX + 12.0f, buttonMax.y + 5.0f),
            IM_COL32(80, 210, 255, 255), 1.5f);
    }

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
    {
        const char* description = icon == TaskbarIcon::Apps ? "Applications and shortcuts" :
            icon == TaskbarIcon::Files ? "Sample folders in File Explorer" :
            "Sample processes, CPU and memory usage";
        ImGui::SetTooltip("%s\nClick to %s.", description, isOpen ? "close" : "open");
    }
    return clicked;
}

// Draw desktop taskbar
static void drawTaskbar()
{
    ImGuiIO& io = ImGui::GetIO();
    const float barHeight = 56.0f;
    const float sidePadding = 10.0f;

	// lock the taskbar to the bottom of the window and set its size
    ImGui::SetNextWindowPos(
        ImVec2(0, io.DisplaySize.y - barHeight),
        ImGuiCond_Always
    );

	// set the taskbar size to match the width of the window and a fixed height
    ImGui::SetNextWindowSize(
        ImVec2(io.DisplaySize.x, barHeight),
        ImGuiCond_Always
    );

	// remove window rounding and set padding for the taskbar
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(sidePadding, 8.0f));

	// set the background color of the taskbar to a semi-transparent dark color: dark blue
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(20, 20, 32, 235));

	// combine flags to create a taskbar window without title bar, resizing, moving, or scrollbars
    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar;
    
	// handle the taskbar window and its buttons
    if (ImGui::Begin("Taskbar", nullptr, flags))
    {
        // Button 1: Opens Applications/Gallery Window
        if (drawTaskbarButton("Apps", TaskbarIcon::Apps, showGallery))
        {
            showGallery = !showGallery;
        }

        ImGui::SameLine();
        
        // Button 2: Opens File Explorer
        if (drawTaskbarButton("Files", TaskbarIcon::Files, showFiles))
        {
            showFiles = !showFiles;
        }

        ImGui::SameLine();

        // Button 3: Opens Task Manager
        if (drawTaskbarButton("Task Manager", TaskbarIcon::TaskManager, showTaskManager))
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
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
            ImGui::SetTooltip("Shut down the desktop emulator.");
    }

    ImGui::End();

	// pop the style color and variables to restore previous settings
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
}

// File Explorer window
static void drawFileWindow()
{
    if (!showFiles)
        return;

    setAppWindowLayout(ImVec2(380, 150), ImVec2(480, 320));

    if (ImGui::Begin("CSOPESY File Explorer", &showFiles))
    {
		// draw header and separator for the file explorer window
        ImGui::Text("Quick Access");
        ImGui::Separator();

		// draw bullet points for the quick access items in the file explorer
        ImGui::BulletText("Desktop");
        ImGui::BulletText("Documents");
        ImGui::BulletText("Downloads");
        ImGui::BulletText("Pictures");
        ImGui::BulletText("This PC");

		ImGui::Spacing();   // add vertical spacing between the quick access items and the placeholder text

		// dimmed text to indicate that this is a placeholder for the file explorer interface
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

    setAppWindowLayout(ImVec2(40, 80), ImVec2(480, 300));

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

    setAppWindowLayout(ImVec2(690, 260), ImVec2(560, 340));

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
            "Version Date: October 12, 2026";

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

    // Use the built-in font at a readable size without an external font file.
    ImFontConfig fontConfig;
    fontConfig.SizePixels = 18.0f;
    io.Fonts->AddFontDefault(&fontConfig);

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
