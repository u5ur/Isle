// //input.cpp
// #include <fstream>
// #include <json.hpp>
//
// #include "Input.h"
// #include "../Render/Render.h"
// #include "../Render/Window/Window.h"
//
// namespace Isle
// {
// 	static void CharCallback(GLFWwindow* window, unsigned int codepoint)
// 	{
// 		ImGui_ImplGlfw_CharCallback(window, codepoint);
// 	}
//
// 	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
// 	{
// 		ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
//
// 		ImGuiIO& io = ImGui::GetIO();
// 		if (!io.WantCaptureKeyboard) {
// 			Input::Instance()->OnMouseButtonCallback(button, action, mods);
// 		}
// 	}
//
// 	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
// 	{
// 		ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
//
// 		ImGuiIO& io = ImGui::GetIO();
// 		if (!io.WantCaptureKeyboard) {
// 			Input::Instance()->OnKeyCallback(key, scancode, action, mods);
// 		}
// 	}
//
// 	static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
// 	{
// 		ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
//
// 		ImGuiIO& io = ImGui::GetIO();
// 		if (!io.WantCaptureMouse) {
// 			Input::Instance()->OnScrollCallback(xoffset, yoffset);
// 		}
// 	}
//
// 	static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
// 	{
// 		ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
// 		Input::Instance()->OnCursorPosCallback(xpos, ypos);
// 	}
//
// 	static void WindowFocusCallback(GLFWwindow* window, int focused)
// 	{
// 		Input::Instance()->OnWindowFocusCallback(focused);
// 	}
//
// 	Input::Input()
// 		: m_MousePosition(0.0f)
// 		, m_LastMousePosition(0.0f)
// 		, m_MouseDelta(0.0f)
// 		, m_ScrollDelta(0.0f)
// 		, m_WindowFocused(true)
// 		, m_FirstMouse(true)
// 		, m_Window(nullptr)
// 	{
// 	}
//
// 	Input::~Input()
// 	{
// 	}
//
// 	void Input::Start()
// 	{
// 		m_Window = Render::Instance()->m_Window->m_Handle;
// 		if (!m_Window)
// 		{
// 			LOG("Input: Failed to get window handle!\n");
// 			return;
// 		}
//
// 		if (IsWindowFocused())
// 			SetCursorMode(true);
//
// 		LoadBinds("settings\\binds.json");
// 		glfwSetKeyCallback(m_Window, KeyCallback);
// 		glfwSetMouseButtonCallback(m_Window, MouseButtonCallback);
// 		glfwSetScrollCallback(m_Window, ScrollCallback);
// 		glfwSetCharCallback(m_Window, CharCallback);
// 		glfwSetWindowFocusCallback(m_Window, WindowFocusCallback);
// 	}
//
// 	void Input::Update()
// 	{
// 		ImGuiIO& io = ImGui::GetIO();
//
// 		double xpos, ypos;
// 		glfwGetCursorPos(m_Window, &xpos, &ypos);
// 		glm::vec2 newPosition = glm::vec2(static_cast<float>(xpos), static_cast<float>(ypos));
// 		io.AddMousePosEvent((float)xpos, (float)ypos);
//
// 		if (m_FirstMouse)
// 		{
// 			m_LastMousePosition = newPosition;
// 			m_MouseDelta = glm::vec2(0.0f);
// 			m_FirstMouse = false;
// 		}
// 		else
// 		{
// 			m_MouseDelta = newPosition - m_LastMousePosition;
// 			m_LastMousePosition = newPosition;
// 		}
//
// 		m_MousePosition = newPosition;
//
// 		UpdateKeyStates();
// 		UpdateMouseStates();
// 	}
//
// 	void Input::Reset()
// 	{
// 		m_ScrollDelta = glm::vec2(0.0f);
// 	}
//
// 	void Input::Destroy()
// 	{
// 		m_ActionBindings.clear();
// 		m_KeyStates.clear();
// 		m_PreviousKeyStates.clear();
// 		m_MouseButtonStates.clear();
// 		m_PreviousMouseButtonStates.clear();
// 	}
//
// 	void Input::BindAction(const std::string& actionName, int key)
// 	{
// 		InputChord chord;
// 		chord.keys.push_back(key);
// 		m_ActionBindings[actionName].chords.clear();
// 		m_ActionBindings[actionName].chords.push_back(chord);
// 	}
//
// 	void Input::BindAction(const std::string& actionName, MouseButton button)
// 	{
// 		InputChord chord;
// 		chord.mouseButtons.push_back(button);
// 		m_ActionBindings[actionName].chords.clear();
// 		m_ActionBindings[actionName].chords.push_back(chord);
// 	}
//
// 	void Input::BindAction(const std::string& actionName, const InputChord& chord)
// 	{
// 		m_ActionBindings[actionName].chords.clear();
// 		m_ActionBindings[actionName].chords.push_back(chord);
// 	}
//
// 	void Input::AddKeyToAction(const std::string& actionName, int key)
// 	{
// 		InputChord chord;
// 		chord.keys.push_back(key);
// 		m_ActionBindings[actionName].chords.push_back(chord);
// 	}
//
// 	void Input::AddMouseButtonToAction(const std::string& actionName, MouseButton button)
// 	{
// 		InputChord chord;
// 		chord.mouseButtons.push_back(button);
// 		m_ActionBindings[actionName].chords.push_back(chord);
// 	}
//
// 	void Input::AddChordToAction(const std::string& actionName, const InputChord& chord)
// 	{
// 		m_ActionBindings[actionName].chords.push_back(chord);
// 	}
//
// 	void Input::UnbindAction(const std::string& actionName)
// 	{
// 		m_ActionBindings.erase(actionName);
// 	}
//
// 	bool Input::IsActionPressed(const std::string& actionName) const
// 	{
// 		auto it = m_ActionBindings.find(actionName);
// 		if (it == m_ActionBindings.end())
// 			return false;
//
// 		const ActionBinding& binding = it->second;
// 		for (const InputChord& chord : binding.chords)
// 		{
// 			if (IsChordPressed(chord))
// 				return true;
// 		}
//
// 		return false;
// 	}
//
// 	bool Input::IsActionHeld(const std::string& actionName) const
// 	{
// 		auto it = m_ActionBindings.find(actionName);
// 		if (it == m_ActionBindings.end())
// 			return false;
//
// 		const ActionBinding& binding = it->second;
// 		for (const InputChord& chord : binding.chords)
// 		{
// 			if (IsChordHeld(chord))
// 				return true;
// 		}
//
// 		return false;
// 	}
//
// 	bool Input::IsActionReleased(const std::string& actionName) const
// 	{
// 		auto it = m_ActionBindings.find(actionName);
// 		if (it == m_ActionBindings.end())
// 			return false;
//
// 		const ActionBinding& binding = it->second;
// 		for (const InputChord& chord : binding.chords)
// 		{
// 			if (IsChordReleased(chord))
// 				return true;
// 		}
//
// 		return false;
// 	}
//
// 	bool Input::IsChordPressed(const InputChord& chord) const
// 	{
// 		if (chord.IsEmpty())
// 			return false;
//
// 		// Check if at least one key/button is pressed
// 		bool hasPressed = false;
//
// 		for (int key : chord.keys)
// 		{
// 			KeyState state = GetKeyState(key);
// 			if (state == KeyState::Pressed)
// 				hasPressed = true;
// 			else if (state != KeyState::Held && state != KeyState::Pressed)
// 				return false; // Required key not held
// 		}
//
// 		for (MouseButton button : chord.mouseButtons)
// 		{
// 			KeyState state = GetMouseButtonState(button);
// 			if (state == KeyState::Pressed)
// 				hasPressed = true;
// 			else if (state != KeyState::Held && state != KeyState::Pressed)
// 				return false; // Required button not held
// 		}
//
// 		return hasPressed;
// 	}
//
// 	bool Input::IsChordHeld(const InputChord& chord) const
// 	{
// 		if (chord.IsEmpty())
// 			return false;
//
// 		// All keys/buttons must be held or pressed
// 		for (int key : chord.keys)
// 		{
// 			KeyState state = GetKeyState(key);
// 			if (state != KeyState::Held && state != KeyState::Pressed)
// 				return false;
// 		}
//
// 		for (MouseButton button : chord.mouseButtons)
// 		{
// 			KeyState state = GetMouseButtonState(button);
// 			if (state != KeyState::Held && state != KeyState::Pressed)
// 				return false;
// 		}
//
// 		return true;
// 	}
//
// 	bool Input::IsChordReleased(const InputChord& chord) const
// 	{
// 		if (chord.IsEmpty())
// 			return false;
//
// 		// Check if at least one key/button is released while others were held
// 		bool hasReleased = false;
//
// 		for (int key : chord.keys)
// 		{
// 			KeyState state = GetKeyState(key);
// 			if (state == KeyState::Released)
// 				hasReleased = true;
// 		}
//
// 		for (MouseButton button : chord.mouseButtons)
// 		{
// 			KeyState state = GetMouseButtonState(button);
// 			if (state == KeyState::Released)
// 				hasReleased = true;
// 		}
//
// 		return hasReleased;
// 	}
//
// 	bool Input::IsKeyPressed(int key) const
// 	{
// 		return GetKeyState(key) == KeyState::Pressed;
// 	}
//
// 	bool Input::IsKeyHeld(int key) const
// 	{
// 		KeyState state = GetKeyState(key);
// 		return state == KeyState::Held || state == KeyState::Pressed;
// 	}
//
// 	bool Input::IsKeyReleased(int key) const
// 	{
// 		return GetKeyState(key) == KeyState::Released;
// 	}
//
// 	Input::KeyState Input::GetKeyState(int key) const
// 	{
// 		auto it = m_KeyStates.find(key);
// 		if (it != m_KeyStates.end())
// 			return it->second;
// 		return KeyState::None;
// 	}
//
// 	bool Input::IsMouseButtonPressed(MouseButton button) const
// 	{
// 		return GetMouseButtonState(button) == KeyState::Pressed;
// 	}
//
// 	bool Input::IsMouseButtonHeld(MouseButton button) const
// 	{
// 		KeyState state = GetMouseButtonState(button);
// 		return state == KeyState::Held || state == KeyState::Pressed;
// 	}
//
// 	bool Input::IsMouseButtonReleased(MouseButton button) const
// 	{
// 		return GetMouseButtonState(button) == KeyState::Released;
// 	}
//
// 	Input::KeyState Input::GetMouseButtonState(MouseButton button) const
// 	{
// 		int buttonIndex = static_cast<int>(button);
// 		auto it = m_MouseButtonStates.find(buttonIndex);
// 		if (it != m_MouseButtonStates.end())
// 			return it->second;
// 		return KeyState::None;
// 	}
//
// 	void Input::SetCursorMode(bool locked)
// 	{
// 		if (!m_Window) return;
//
// 		m_CursorLocked = locked;
//
// 		if (locked)
// 		{
// 			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
// 			m_FirstMouse = true;
// 		}
// 		else
// 		{
// 			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
// 			double xpos, ypos;
// 			glfwGetCursorPos(m_Window, &xpos, &ypos);
// 			ImGuiIO& io = ImGui::GetIO();
// 			io.AddMousePosEvent((float)xpos, (float)ypos);
// 		}
// 	}
//
// 	void Input::OnKeyCallback(int key, int scancode, int action, int mods)
// 	{
// 		if (action == GLFW_PRESS)
// 		{
// 			m_KeyStates[key] = KeyState::Pressed;
// 		}
// 		else if (action == GLFW_RELEASE)
// 		{
// 			m_KeyStates[key] = KeyState::Released;
// 		}
// 	}
//
// 	void Input::OnMouseButtonCallback(int button, int action, int mods)
// 	{
// 		if (action == GLFW_PRESS)
// 		{
// 			m_MouseButtonStates[button] = KeyState::Pressed;
// 		}
// 		else if (action == GLFW_RELEASE)
// 		{
// 			m_MouseButtonStates[button] = KeyState::Released;
// 		}
// 	}
//
// 	void Input::OnCursorPosCallback(double xpos, double ypos)
// 	{
// 		glm::vec2 newPosition = glm::vec2(static_cast<float>(xpos), static_cast<float>(ypos));
//
// 		if (m_FirstMouse)
// 		{
// 			m_MousePosition = newPosition;
// 			m_LastMousePosition = newPosition;
// 			m_MouseDelta = glm::vec2(0.0f);
// 			m_FirstMouse = false;
// 			return;
// 		}
//
// 		m_MousePosition = newPosition;
// 	}
//
// 	void Input::OnScrollCallback(double xoffset, double yoffset)
// 	{
// 		m_ScrollDelta = glm::vec2(static_cast<float>(xoffset), static_cast<float>(yoffset));
// 	}
//
// 	void Input::OnWindowFocusCallback(int focused)
// 	{
// 		m_WindowFocused = (focused == GLFW_TRUE);
// 		if (m_WindowFocused)
// 		{
// 			m_FirstMouse = true;
// 		}
// 	}
//
// 	void Input::UpdateKeyStates()
// 	{
// 		for (auto& pair : m_KeyStates)
// 		{
// 			KeyState& currentState = pair.second;
// 			KeyState previousState = KeyState::None;
//
// 			auto prevIt = m_PreviousKeyStates.find(pair.first);
// 			if (prevIt != m_PreviousKeyStates.end())
// 				previousState = prevIt->second;
//
// 			if (currentState == KeyState::Pressed && previousState == KeyState::None)
// 			{
// 				// Keep as Pressed
// 			}
// 			else if (currentState == KeyState::Pressed)
// 			{
// 				currentState = KeyState::Held;
// 			}
// 			else if (currentState == KeyState::Released && previousState == KeyState::Released)
// 			{
// 				currentState = KeyState::None;
// 			}
//
// 			m_PreviousKeyStates[pair.first] = pair.second;
// 		}
// 	}
//
// 	void Input::UpdateMouseStates()
// 	{
// 		for (auto& pair : m_MouseButtonStates)
// 		{
// 			KeyState& currentState = pair.second;
// 			KeyState previousState = KeyState::None;
//
// 			auto prevIt = m_PreviousMouseButtonStates.find(pair.first);
// 			if (prevIt != m_PreviousMouseButtonStates.end())
// 				previousState = prevIt->second;
//
// 			if (currentState == KeyState::Pressed)
// 			{
// 				currentState = KeyState::Held;
// 			}
// 			else if (currentState == KeyState::Released && previousState == KeyState::Released)
// 			{
// 				currentState = KeyState::None;
// 			}
//
// 			m_PreviousMouseButtonStates[pair.first] = pair.second;
// 		}
// 	}
//
// 	bool Input::SaveBinds(const std::string& filePath) const
// 	{
// 		nlohmann::json j;
//
// 		for (const auto& [actionName, binding] : m_ActionBindings)
// 		{
// 			nlohmann::json chords = nlohmann::json::array();
//
// 			for (const InputChord& chord : binding.chords)
// 			{
// 				nlohmann::json chordJson;
// 				chordJson["keys"] = chord.keys;
//
// 				std::vector<int> mouseInts;
// 				for (auto button : chord.mouseButtons)
// 					mouseInts.push_back(static_cast<int>(button));
//
// 				chordJson["mouseButtons"] = mouseInts;
// 				chords.push_back(chordJson);
// 			}
//
// 			j[actionName] = chords;
// 		}
//
// 		std::ofstream file(filePath);
// 		if (!file.is_open())
// 			return false;
//
// 		file << j.dump(4);
// 		return true;
// 	}
//
// 	bool Input::LoadBinds(const std::string& filePath)
// 	{
// 		std::ifstream file(filePath);
// 		if (!file.is_open())
// 			return false;
//
// 		nlohmann::json j;
// 		file >> j;
//
// 		m_ActionBindings.clear();
//
// 		for (auto it = j.begin(); it != j.end(); ++it)
// 		{
// 			ActionBinding binding;
//
// 			if (it.value().is_array())
// 			{
// 				for (const auto& chordJson : it.value())
// 				{
// 					InputChord chord;
//
// 					if (chordJson.contains("keys"))
// 						chord.keys = chordJson["keys"].get<std::vector<int>>();
//
// 					if (chordJson.contains("mouseButtons"))
// 					{
// 						for (int buttonInt : chordJson["mouseButtons"])
// 							chord.mouseButtons.push_back(static_cast<MouseButton>(buttonInt));
// 					}
//
// 					binding.chords.push_back(chord);
// 				}
// 			}
//
// 			m_ActionBindings[it.key()] = binding;
// 		}
//
// 		return true;
// 	}
// }