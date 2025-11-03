//input.h
#pragma once
#include <Core/Common/Common.h>

namespace Isle
{
	class Input : public Singleton<Input>, public Component
	{
	public:
		enum class KeyState
		{
			None,
			Pressed,
			Held,
			Released
		};

		enum class MouseButton
		{
			Left = 0,
			Right = 1,
			Middle = 2
		};

		struct InputChord
		{
			std::vector<int> keys;
			std::vector<MouseButton> mouseButtons;

			bool IsEmpty() const { return keys.empty() && mouseButtons.empty(); }
		};

		struct ActionBinding
		{
			std::vector<InputChord> chords;
		};

		Input();
		~Input();

		virtual void Start() override;
		virtual void Update() override;
		virtual void Destroy() override;
		void Reset();

		void BindAction(const std::string& actionName, int key);
		void BindAction(const std::string& actionName, MouseButton button);
		void BindAction(const std::string& actionName, const InputChord& chord);

		void AddKeyToAction(const std::string& actionName, int key);
		void AddMouseButtonToAction(const std::string& actionName, MouseButton button);
		void AddChordToAction(const std::string& actionName, const InputChord& chord);

		void UnbindAction(const std::string& actionName);

		bool IsActionPressed(const std::string& actionName) const;
		bool IsActionHeld(const std::string& actionName) const;
		bool IsActionReleased(const std::string& actionName) const;

		bool IsKeyPressed(int key) const;
		bool IsKeyHeld(int key) const;
		bool IsKeyReleased(int key) const;
		KeyState GetKeyState(int key) const;

		bool IsMouseButtonPressed(MouseButton button) const;
		bool IsMouseButtonHeld(MouseButton button) const;
		bool IsMouseButtonReleased(MouseButton button) const;
		KeyState GetMouseButtonState(MouseButton button) const;

		glm::vec2 GetMousePosition() const { return m_MousePosition; }
		glm::vec2 GetMouseDelta() const { return m_MouseDelta; }
		glm::vec2 GetScrollDelta() const { return m_ScrollDelta; }
		bool IsWindowFocused() const { return m_WindowFocused; }
		bool IsCursorLocked() const { return m_CursorLocked; }
		void SetCursorMode(bool locked);

		void OnKeyCallback(int key, int scancode, int action, int mods);
		void OnMouseButtonCallback(int button, int action, int mods);
		void OnCursorPosCallback(double xpos, double ypos);
		void OnScrollCallback(double xoffset, double yoffset);
		void OnWindowFocusCallback(int focused);

	private:
		void UpdateKeyStates();
		void UpdateMouseStates();
		bool SaveBinds(const std::string& filePath) const;
		bool LoadBinds(const std::string& filePath);

		bool IsChordPressed(const InputChord& chord) const;
		bool IsChordHeld(const InputChord& chord) const;
		bool IsChordReleased(const InputChord& chord) const;

		std::unordered_map<int, KeyState> m_KeyStates;
		std::unordered_map<int, KeyState> m_PreviousKeyStates;
		std::unordered_map<int, KeyState> m_MouseButtonStates;
		std::unordered_map<int, KeyState> m_PreviousMouseButtonStates;
		std::unordered_map<std::string, ActionBinding> m_ActionBindings;

		glm::vec2 m_MousePosition;
		glm::vec2 m_LastMousePosition;
		glm::vec2 m_MouseDelta;
		glm::vec2 m_ScrollDelta;
		bool m_WindowFocused;
		bool m_CursorLocked;
		bool m_FirstMouse;
		GLFWwindow* m_Window;
	};
}