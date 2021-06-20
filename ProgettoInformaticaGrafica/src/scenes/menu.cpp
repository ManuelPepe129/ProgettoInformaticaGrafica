#include "menu.h"

#include <string>
#include <fstream>
#include <vector>
#include <utility> // std::pair
#include <stdexcept> // std::runtime_error
#include <sstream> // std::stringstream
#include <algorithm>

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

bool Menu::init() {
	// init window
	if (BaseScene::init()) {
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			ImGui::StyleColorsDark();
			if (ImGui_ImplGlfw_InitForOpenGL(window, true)) {
				return ImGui_ImplOpenGL3_Init("#version 130");
			}
	}
	return false;
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
	char buf1[64] = "";
	switch (currentMenuState)
	{
	case MenuState::MAIN_MENU:
		if (buttonCentered("New Game"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		{
			currentMenuState = MenuState::GET_NAME;
		}
		if (buttonCentered(" Rules  "))
		{
			currentMenuState = MenuState::RULES;
		}
		if (buttonCentered(" Leaderboard  "))
		{
			if (scores.empty())
			{
				// File pointer
				std::fstream fin;

				// Open an existing file
				fin.open("leaderboard.txt", std::ios::in);

				// Read the Data from the file
				// as String Vector
				std::vector<std::string> row;
				std::string line, word, temp;

				while (getline(fin, line)) {

					row.clear();

					//std::cout << line << std::endl;

					// used for breaking words
					std::stringstream s(line);

					// read every column data of a row and
					// store it in a string variable, 'word'
					while (std::getline(s, word, ',')) {

						// add all the column data
						// of a row to a vector
						row.push_back(word);
						//std::cout << word << std::endl;
					}
					scores.push_back(Score{ row[0], row[1], row[2] });

				}
				fin.close();
				sortScores();
			}
			

			currentMenuState = MenuState::LEADERBOARD;
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
	case MenuState::GET_NAME:
		textCentered("Insert Player Name");
		if (textInputCentered("", buf1, 64, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue))
		{
			BaseScene::playerName = std::string(buf1);
			currentMenuState = MenuState::NEW_GAME;
		}
		break;
	case MenuState::LEADERBOARD:
		textCentered("Hall of Fame");
		for (int i = 0; i < scores.size(); ++i)
		{
			textCentered(std::to_string(i) + ". " + scores[i].name + " " + scores[i].time + "s " + scores[i].points + " points");
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
		textCentered("Esci dal labirinto evitando o sconfiggendo i mostri.");
		ImGui::Text("");
		textCentered("Attenzione pero'! Il pericolo e' dietro l'angolo");
		textCentered("e le risorse a disposizione sono limitate.");
		ImGui::Text("");
		textCentered("Sconfiggi i mostri per ricaricare munizioni");
		textCentered("Raccogli le caramelle per ottenere vite bonus e munizioni");
		ImGui::Text("");
		textCentered("Esci dal labirinto sconfiggendo piu' nemici possibile per entrare nella leaderboard!");
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

void Menu::sortScores()
{
	std::sort(scores.begin(), scores.end(), [](const Score& lhs, const Score& rhs) {
		return lhs.points > rhs.points;
	});
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

bool Menu::textInputCentered(const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags)
{
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + BaseScene::scrWidth/5);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 25);
	return ImGui::InputText(label, buf, buf_size, flags);
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
		case MenuState::GET_NAME:
			currentMenuState = MenuState::MAIN_MENU;
			break;
		case MenuState::CREDITS:
			currentMenuState = MenuState::MAIN_MENU;
			break;
		case MenuState::RULES:
			currentMenuState = MenuState::MAIN_MENU;
			break;
		case MenuState::LEADERBOARD:
			currentMenuState = MenuState::MAIN_MENU;
			break;
		default:
			setShouldClose(true);
			break;
		}
	}

}