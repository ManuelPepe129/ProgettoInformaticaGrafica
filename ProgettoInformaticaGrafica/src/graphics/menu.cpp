#include "menu.h"

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

Menu::Menu(int glfwVersionMajor, int glfwVersionMinor, const char* title, unsigned int scrWidth, unsigned int scrHeight)
	:BaseScene(glfwVersionMajor, glfwVersionMinor, title, scrWidth, scrHeight)
{

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

	ImGui::Begin(title);                          // Create a window called "Hello, world!" and append into it

	//ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
	//ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
	//ImGui::Checkbox("Another Window", &show_another_window);

	//ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
	//ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

	if (ImGui::Button("New Game"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		std::cout << "start new game" << std::endl;
	if (ImGui::Button("Rules"))
		std::cout << "print rules" << std::endl;
	if (ImGui::Button("Credits"))
		std::cout << "print credis" << std::endl;
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

	glfwDestroyWindow(window);
	glfwTerminate();
}