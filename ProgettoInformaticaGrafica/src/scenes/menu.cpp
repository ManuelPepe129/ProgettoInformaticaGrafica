#include "menu.h"

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

Menu::Menu(int glfwVersionMajor, int glfwVersionMinor, const char* title, unsigned int scrWidth, unsigned int scrHeight)
	:BaseScene(glfwVersionMajor, glfwVersionMinor, title, scrWidth, scrHeight)
{
	sceneType = SceneType::MENU;
	currentMenuState = MenuState::MAIN_MENU;
}

void Menu::init() {
	/*
	// Setup window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return;

	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	// Create window with graphics context
	window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
	if (window == NULL)
		return;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

		// Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
	bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
	bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
	bool err = gladLoadGL() == 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
	bool err = gladLoadGL(glfwGetProcAddress) == 0; // glad2 recommend using the windowing library loader instead of the (optionally) bundled one.
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
	bool err = false;
	glbinding::Binding::initialize();
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
	bool err = false;
	glbinding::initialize([](const char* name) { return (glbinding::ProcAddress)glfwGetProcAddress(name); });
#else
	bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
	if (err)
	{
		std::cout << "Failed to initialize OpenGL loader!" << std::endl;
		return;
	}
	*/

	// init window
	BaseScene::init();
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");
}

void Menu::render()
{
	// Poll and handle events (inputs, window resize, etc.)
	// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
	// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
	// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
	// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
	glfwPollEvents();

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	static float f = 0.0f;
	static int counter = 0;

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoTitleBar;

	// We demonstrate using the full viewport area or the work area (without menu-bars, task-bars etc.)
	// Based on your use case you may want one of the other.
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);

	ImGui::Begin(title, NULL, window_flags);                          // Create a window  and append into it

	//ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
	//ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
	//ImGui::Checkbox("Another Window", &show_another_window);

	//ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
	//ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

	switch (currentMenuState)
	{
	case MenuState::MAIN_MENU:
		if (buttonCentered("New Game"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		{
			currentMenuState = MenuState::NEW_GAME;
		}
		if (buttonCentered(" Rules  "))
		{
			currentMenuState = MenuState::RULES;
		}
		if (buttonCentered(" Credits"))
		{
			currentMenuState = MenuState::CREDITS;
		}
		if (buttonCentered("  Close "))
		{
			setShouldClose(true);
		}
		break;
	case MenuState::CREDITS:
		textCentered("02BBHIOV - Informatica Grafica - Anno Accademico 2020/21");
		ImGui::Text("");
		textCentered("Professore titolare: Fabrizio Lamberti");
		textCentered("Esercitatore: Alberto Cannavo'");
		ImGui::Text("");
		textCentered("Progetto Gruppo 20 - The Shining");
		textCentered("Manuel Pepe - s281221");
		textCentered("Riccardo Malvicino - s290338");
		textCentered("Emanuele Zacheo - s290260");
		ImGui::Text("");
		ImGui::Text("Press ESC to return to main menu...");
		break;
	case MenuState::RULES:
		textCentered("Esci dal labirinto.");
		ImGui::Text("");
		ImGui::Text("Press ESC to return to main menu...");
		break;
	default:
		break;
	}
	//ImGui::SameLine();
	//ImGui::Text("counter = %d", counter);

	//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();

	// Rendering
	ImGui::Render();
	int display_w, display_h;
	glfwGetFramebufferSize(window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
}

void Menu::update()
{
	glClearColor(background.r, background.g, background.b, background.a);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Menu::cleanup()
{
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	if (BaseScene::instances == 1) {
		glfwDestroyWindow(window);
		glfwTerminate();
	}
	
}

bool Menu::buttonCentered(const char* label, float alignment)
{
	//ImGuiStyle& style = ImGui::GetStyle();

	//float size = ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f;
	//float avail = ImGui::GetContentRegionAvail().x;

	float off = (BaseScene::scrWidth - BaseScene::scrWidth / 5.0f) * alignment;
	if (off > 0.0f)
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 25);

	return ImGui::Button(label, ImVec2(BaseScene::scrWidth / 5, BaseScene::scrHeight / 10));
}

void Menu::textCentered(std::string text) {
	float font_size = ImGui::GetFontSize() * text.size() / 2;
	ImGui::SameLine(
		ImGui::GetWindowSize().x / 2 -
		font_size + (font_size / 2)
	);

	ImGui::Text(text.c_str());
	ImGui::Text("");
}

void Menu::processInput(float dt)
{
	if (Keyboard::keyWentDown(GLFW_KEY_ESCAPE)) {
		switch (currentMenuState)
		{
		case MenuState::MAIN_MENU:
			setShouldClose(true);
			break;
		case MenuState::NEW_GAME:
			break;
		case MenuState::CREDITS:
			currentMenuState = MenuState::MAIN_MENU;
			break;
		case MenuState::RULES:
			currentMenuState = MenuState::MAIN_MENU;
			break;
		default:
			setShouldClose(true);
			break;
		}
	}

}