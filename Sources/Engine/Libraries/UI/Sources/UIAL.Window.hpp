#pragma once 

#include <any>
#include <queue>
#include <optional>
#include <Math.hpp>
#include <OS.Assert.hpp>
#include <OS.Logger.hpp>


namespace UIAL {


	enum class Subsystem {
		GLFW,
		WinAPI,
		Undefined
	};

	enum class Render {
		Vulkan,
		OpenGL
	};

	class Window {
	public:



		using Size = Math::Vector2i;

		struct CreateInfo {
			Size windowSize_;
			std::string title_;
		};
#undef DELETE


		enum class Event : int {
			FRAMEBUFFER_RESIZED,
			Undefined
		};

		struct FrameBufferResizeEvent {
			int newWidth_ = 0;
			int newHeight_ = 0;
		};


        enum class KeyboardKey : int {
            // Functional keys
            ESCAPE = 256,          // GLFW_KEY_ESCAPE
            ENTER = 257,           // GLFW_KEY_ENTER
            TAB = 258,             // GLFW_KEY_TAB
            BACKSPACE = 259,       // GLFW_KEY_BACKSPACE
            INSERT = 260,          // GLFW_KEY_INSERT
            DELETE = 261,          // GLFW_KEY_DELETE
            RIGHT = 262,           // GLFW_KEY_RIGHT
            LEFT = 263,            // GLFW_KEY_LEFT
            DOWN = 264,            // GLFW_KEY_DOWN
            UP = 265,              // GLFW_KEY_UP
            PAGE_UP = 266,         // GLFW_KEY_PAGE_UP
            PAGE_DOWN = 267,       // GLFW_KEY_PAGE_DOWN
            HOME = 268,            // GLFW_KEY_HOME
            END = 269,             // GLFW_KEY_END
            CAPS_LOCK = 280,       // GLFW_KEY_CAPS_LOCK
            SCROLL_LOCK = 281,     // GLFW_KEY_SCROLL_LOCK
            NUM_LOCK = 282,        // GLFW_KEY_NUM_LOCK
            PRINT_SCREEN = 283,    // GLFW_KEY_PRINT_SCREEN
            PAUSE = 284,           // GLFW_KEY_PAUSE
            F1 = 290,              // GLFW_KEY_F1
            F2 = 291,              // GLFW_KEY_F2
            F3 = 292,              // GLFW_KEY_F3
            F4 = 293,              // GLFW_KEY_F4
            F5 = 294,              // GLFW_KEY_F5
            F6 = 295,              // GLFW_KEY_F6
            F7 = 296,              // GLFW_KEY_F7
            F8 = 297,              // GLFW_KEY_F8
            F9 = 298,              // GLFW_KEY_F9
            F10 = 299,             // GLFW_KEY_F10
            F11 = 300,             // GLFW_KEY_F11
            F12 = 301,             // GLFW_KEY_F12
            F13 = 302,             // GLFW_KEY_F13
            F14 = 303,             // GLFW_KEY_F14
            F15 = 304,             // GLFW_KEY_F15
            F16 = 305,             // GLFW_KEY_F16
            F17 = 306,             // GLFW_KEY_F17
            F18 = 307,             // GLFW_KEY_F18
            F19 = 308,             // GLFW_KEY_F19
            F20 = 309,             // GLFW_KEY_F20
            F21 = 310,             // GLFW_KEY_F21
            F22 = 311,             // GLFW_KEY_F22
            F23 = 312,             // GLFW_KEY_F23
            F24 = 313,             // GLFW_KEY_F24
            F25 = 314,             // GLFW_KEY_F25
            KP_0 = 320,            // GLFW_KEY_KP_0
            KP_1 = 321,            // GLFW_KEY_KP_1
            KP_2 = 322,            // GLFW_KEY_KP_2
            KP_3 = 323,            // GLFW_KEY_KP_3
            KP_4 = 324,            // GLFW_KEY_KP_4
            KP_5 = 325,            // GLFW_KEY_KP_5
            KP_6 = 326,            // GLFW_KEY_KP_6
            KP_7 = 327,            // GLFW_KEY_KP_7
            KP_8 = 328,            // GLFW_KEY_KP_8
            KP_9 = 329,            // GLFW_KEY_KP_9
            KP_DECIMAL = 330,      // GLFW_KEY_KP_DECIMAL
            KP_DIVIDE = 331,       // GLFW_KEY_KP_DIVIDE
            KP_MULTIPLY = 332,     // GLFW_KEY_KP_MULTIPLY
            KP_SUBTRACT = 333,     // GLFW_KEY_KP_SUBTRACT
            KP_ADD = 334,          // GLFW_KEY_KP_ADD
            KP_ENTER = 335,        // GLFW_KEY_KP_ENTER
            KP_EQUAL = 336,        // GLFW_KEY_KP_EQUAL
            LEFT_SHIFT = 340,      // GLFW_KEY_LEFT_SHIFT
            LEFT_CONTROL = 341,    // GLFW_KEY_LEFT_CONTROL
            LEFT_ALT = 342,        // GLFW_KEY_LEFT_ALT
            LEFT_SUPER = 343,      // GLFW_KEY_LEFT_SUPER
            RIGHT_SHIFT = 344,     // GLFW_KEY_RIGHT_SHIFT
            RIGHT_CONTROL = 345,   // GLFW_KEY_RIGHT_CONTROL
            RIGHT_ALT = 346,       // GLFW_KEY_RIGHT_ALT
            RIGHT_SUPER = 347,     // GLFW_KEY_RIGHT_SUPER
            MENU = 348,            // GLFW_KEY_MENU
            SPACE = 32,            // GLFW_KEY_SPACE
            APOSTROPHE = 39,       // GLFW_KEY_APOSTROPHE ('')
            COMMA = 44,            // GLFW_KEY_COMMA (,)
            MINUS = 45,            // GLFW_KEY_MINUS (-)
            PERIOD = 46,           // GLFW_KEY_PERIOD (.)
            SLASH = 47,            // GLFW_KEY_SLASH (/)
            _0 = 48,               // GLFW_KEY_0
            _1 = 49,               // GLFW_KEY_1
            _2 = 50,               // GLFW_KEY_2
            _3 = 51,               // GLFW_KEY_3
            _4 = 52,               // GLFW_KEY_4
            _5 = 53,               // GLFW_KEY_5
            _6 = 54,               // GLFW_KEY_6
            _7 = 55,               // GLFW_KEY_7
            _8 = 56,               // GLFW_KEY_8
            _9 = 57,               // GLFW_KEY_9
            SEMICOLON = 59,        // GLFW_KEY_SEMICOLON (;)
            EQUAL = 61,            // GLFW_KEY_EQUAL (=)
            A = 65,                // GLFW_KEY_A
            B = 66,                // GLFW_KEY_B
            C = 67,                // GLFW_KEY_C
            D = 68,                // GLFW_KEY_D
            E = 69,                // GLFW_KEY_E
            F = 70,                // GLFW_KEY_F
            G = 71,                // GLFW_KEY_G
            H = 72,                // GLFW_KEY_H
            I = 73,                // GLFW_KEY_I
            J = 74,                // GLFW_KEY_J
            K = 75,                // GLFW_KEY_K
            L = 76,                // GLFW_KEY_L
            M = 77,                // GLFW_KEY_M
            N = 78,                // GLFW_KEY_N
            O = 79,                // GLFW_KEY_O
            P = 80,                // GLFW_KEY_P
            Q = 81,                // GLFW_KEY_Q
            R = 82,                // GLFW_KEY_R
            S = 83,                // GLFW_KEY_S
            T = 84,                // GLFW_KEY_T
            U = 85,                // GLFW_KEY_U
            V = 86,                // GLFW_KEY_V
            W = 87,                // GLFW_KEY_W
            X = 88,                // GLFW_KEY_X
            Y = 89,                // GLFW_KEY_Y
            Z = 90,                // GLFW_KEY_Z
            LEFT_BRACKET = 91,     // GLFW_KEY_LEFT_BRACKET ([)
            BACKSLASH = 92,        // GLFW_KEY_BACKSLASH (\)
            RIGHT_BRACKET = 93,    // GLFW_KEY_RIGHT_BRACKET (])
            GRAVE_ACCENT = 96,     // GLFW_KEY_GRAVE_ACCENT (`)
            WORLD_1 = 161,         // GLFW_KEY_WORLD_1 (non-US #1)
            WORLD_2 = 162,         // GLFW_KEY_WORLD_2 (non-US #2)
            Undefined = -1         // Специальное значение (не из GLFW)
        };

        enum class MouseKey : int {
            Left = 0,  // GLFW_MOUSE_BUTTON_1 (левая кнопка)
            Right = 1,  // GLFW_MOUSE_BUTTON_2 (правая кнопка)
            Middle = 2,  // GLFW_MOUSE_BUTTON_3 (средняя кнопка/колесо)
            _3 = 3,  // GLFW_MOUSE_BUTTON_4 (доп. кнопка 1)
            _4 = 4,  // GLFW_MOUSE_BUTTON_5 (доп. кнопка 2)
            _5 = 5,  // GLFW_MOUSE_BUTTON_6 (доп. кнопка 3)
            _6 = 6,  // GLFW_MOUSE_BUTTON_7 (доп. кнопка 4)
            _7 = 7,  // GLFW_MOUSE_BUTTON_8 (доп. кнопка 5)
            Undefined = -1  // Неопределенная кнопка
        };

        enum class KeyboardAction : Common::UInt64 {
            Pressed = 1,  // GLFW_PRESS (клавиша нажата)
            Released = 0,  // GLFW_RELEASE (клавиша отпущена)
            Undefined = 2   // Неопределенное действие (не используется в GLFW)
        };

		struct KeyboardEvent {
			KeyboardKey key_ = KeyboardKey::Undefined;
			KeyboardAction event_ = KeyboardAction::Undefined;
		};

		struct MouseEvent {
			glm::ivec2 position_;
			glm::ivec2 offset_;
			MouseKey key_ = MouseKey::Undefined;
			KeyboardAction event_ = KeyboardAction::Undefined;
		};


		struct Info {
			std::any param1_;
			std::any param2_;
			std::any param3_;
			std::any param4_;
			glm::u32vec2 size_{ 0, 0 };
			Subsystem subsystem_ = Subsystem::Undefined;
		};


		Window(const CreateInfo& createInfo) noexcept : createInfo_{ createInfo } {  }

		[[nodiscard]]
		std::optional<KeyboardEvent> GetKeyboardEvent() noexcept {
			if (keyboardEvents_.empty()) return {};
			KeyboardEvent event = keyboardEvents_.front();
			keyboardEvents_.erase(keyboardEvents_.begin());
			return event;
		}

		[[nodiscard]]
		std::optional<MouseEvent> GetMouseEvent() noexcept {
			if (mouseEvents_.empty()) return {};
			MouseEvent event = mouseEvents_.front();
			mouseEvents_.erase(mouseEvents_.begin());
			return event;
		}

		[[nodiscard]]
		std::optional<FrameBufferResizeEvent> GetFrameBufferResizeEvent() noexcept {
			if (frameBufferResizeEvents_.empty()) return {};
			FrameBufferResizeEvent event = frameBufferResizeEvents_.front();
			frameBufferResizeEvents_.erase(frameBufferResizeEvents_.begin());
			return event;
		}


		[[nodiscard]]
		virtual glm::u32vec2 GetSize() const noexcept = 0;
		virtual void SetTitle(const std::string& title) noexcept = 0;
		virtual void Show() = 0;
		virtual void ProcessInput() = 0;
		virtual void EnableCursor() = 0;
		virtual void DisableCursor() = 0;

	protected:
		void PushEvent(const KeyboardEvent& event) noexcept {
			keyboardEvents_.push_back(event);
		}
		void PushEvent(const MouseEvent& event) noexcept {
			mouseEvents_.push_back(event);
		}
		void PushEvent(const FrameBufferResizeEvent& event) noexcept {
			frameBufferResizeEvents_.push_back(event);
		}

	private:
		std::vector<FrameBufferResizeEvent> frameBufferResizeEvents_;
		std::vector<KeyboardEvent> keyboardEvents_;
		std::vector<MouseEvent> mouseEvents_;

		CreateInfo createInfo_;

	};

}