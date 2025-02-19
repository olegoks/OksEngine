#pragma once 

#include <any>
#include <queue>
#include <optional>
#include <Math.hpp>
#include <OS.Assert.hpp>
#include <OS.Logger.hpp>

#include <glfw/glfw3.h>

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

			//Functional keys
			ESCAPE = GLFW_KEY_ESCAPE,
			ENTER = GLFW_KEY_ENTER,
			TAB = GLFW_KEY_TAB,
			BACKSPACE = GLFW_KEY_BACKSPACE,
			INSERT = GLFW_KEY_INSERT,

			DELETE = GLFW_KEY_DELETE,
			RIGHT = GLFW_KEY_RIGHT,
			LEFT = GLFW_KEY_LEFT,
			DOWN = GLFW_KEY_DOWN,
			UP = GLFW_KEY_UP,
			PAGE_UP = GLFW_KEY_PAGE_UP,
			PAGE_DOWN = GLFW_KEY_PAGE_DOWN,
			HOME = GLFW_KEY_HOME,
			END = GLFW_KEY_END,
			CAPS_LOCK = GLFW_KEY_CAPS_LOCK,
			SCROLL_LOCK = GLFW_KEY_SCROLL_LOCK,
			NUM_LOCK = GLFW_KEY_NUM_LOCK,
			PRINT_SCREEN = GLFW_KEY_PRINT_SCREEN,
			PAUSE = GLFW_KEY_PAUSE,
			F1 = GLFW_KEY_F1,
			F2 = GLFW_KEY_F2,
			F3 = GLFW_KEY_F3,
			F4 = GLFW_KEY_F4,
			F5 = GLFW_KEY_F5,
			F6 = GLFW_KEY_F6,
			F7 = GLFW_KEY_F7,
			F8 = GLFW_KEY_F8,
			F9 = GLFW_KEY_F9,
			F10 = GLFW_KEY_F10,
			F11 = GLFW_KEY_F11,
			F12 = GLFW_KEY_F12,
			F13 = GLFW_KEY_F13,
			F14 = GLFW_KEY_F14,
			F15 = GLFW_KEY_F15,
			F16 = GLFW_KEY_F16,
			F17 = GLFW_KEY_F17,
			F18 = GLFW_KEY_F18,
			F19 = GLFW_KEY_F19,
			F20 = GLFW_KEY_F20,
			F21 = GLFW_KEY_F21,
			F22 = GLFW_KEY_F22,
			F23 = GLFW_KEY_F23,
			F24 = GLFW_KEY_F24,
			F25 = GLFW_KEY_F25,
			KP_0 = GLFW_KEY_KP_0,
			KP_1 = GLFW_KEY_KP_1,
			KP_2 = GLFW_KEY_KP_2,
			KP_3 = GLFW_KEY_KP_3,
			KP_4 = GLFW_KEY_KP_4,
			KP_5 = GLFW_KEY_KP_5,
			KP_6 = GLFW_KEY_KP_6,
			KP_7 = GLFW_KEY_KP_7,
			KP_8 = GLFW_KEY_KP_8,
			KP_9 = GLFW_KEY_KP_9,
			KP_DECIMAL = GLFW_KEY_KP_DECIMAL,
			KP_DIVIDE = GLFW_KEY_KP_DIVIDE,
			KP_MULTIPLY = GLFW_KEY_KP_MULTIPLY,
			KP_SUBTRACT = GLFW_KEY_KP_SUBTRACT,
			KP_ADD = GLFW_KEY_KP_ADD,
			KP_ENTER = GLFW_KEY_KP_ENTER,
			KP_EQUAL = GLFW_KEY_KP_EQUAL,
			LEFT_SHIFT = GLFW_KEY_LEFT_SHIFT,
			LEFT_CONTROL = GLFW_KEY_LEFT_CONTROL,
			LEFT_ALT = GLFW_KEY_LEFT_ALT,
			LEFT_SUPER = GLFW_KEY_LEFT_SUPER,
			RIGHT_SHIFT = GLFW_KEY_RIGHT_SHIFT,
			RIGHT_CONTROL = GLFW_KEY_RIGHT_CONTROL,
			RIGHT_ALT = GLFW_KEY_RIGHT_ALT,
			RIGHT_SUPER = GLFW_KEY_RIGHT_SUPER,
			MENU = GLFW_KEY_MENU,
			SPACE = GLFW_KEY_SPACE,
			APOSTROPHE = GLFW_KEY_APOSTROPHE, /* ' */
			COMMA = GLFW_KEY_COMMA/* , */,
			MINUS = GLFW_KEY_MINUS/* - */,
			PERIOD = GLFW_KEY_PERIOD /* . */,
			SLASH = GLFW_KEY_SLASH /* / */,
			_0 = GLFW_KEY_0,
			_1 = GLFW_KEY_1,
			_2 = GLFW_KEY_2,
			_3 = GLFW_KEY_3,
			_4 = GLFW_KEY_4,
			_5 = GLFW_KEY_5,
			_6 = GLFW_KEY_6,
			_7 = GLFW_KEY_7,
			_8 = GLFW_KEY_8,
			_9 = GLFW_KEY_9,
			SEMICOLON = GLFW_KEY_SEMICOLON, /* ; */
			EQUAL = GLFW_KEY_EQUAL            /* = */,
			A = GLFW_KEY_A,
			B = GLFW_KEY_B,
			C = GLFW_KEY_C,
			D = GLFW_KEY_D,
			E = GLFW_KEY_E,
			F = GLFW_KEY_F,
			G = GLFW_KEY_G,
			H = GLFW_KEY_H,
			I = GLFW_KEY_I,
			J = GLFW_KEY_J,
			K = GLFW_KEY_K,
			L = GLFW_KEY_L,
			M = GLFW_KEY_M,
			N = GLFW_KEY_N,
			O = GLFW_KEY_O,
			P = GLFW_KEY_P,
			Q = GLFW_KEY_Q,
			R = GLFW_KEY_R,
			S = GLFW_KEY_S,
			T = GLFW_KEY_T,
			U = GLFW_KEY_U,
			V = GLFW_KEY_V,
			W = GLFW_KEY_W,
			X = GLFW_KEY_X,
			Y = GLFW_KEY_Y,
			Z = GLFW_KEY_Z,
			LEFT_BRACKET = GLFW_KEY_LEFT_BRACKET,       /* [ */
			BACKSLASH = GLFW_KEY_BACKSLASH,       /* \ */
			RIGHT_BRACKET = GLFW_KEY_RIGHT_BRACKET,       /* ] */
			GRAVE_ACCENT = GLFW_KEY_GRAVE_ACCENT,       /* ` */
			WORLD_1 = GLFW_KEY_WORLD_1,       /* non-US #1 */
			WORLD_2 = GLFW_KEY_WORLD_2,       /* non-US #2 */

			Undefined
		};

		enum class MouseKey : int {
			Left = GLFW_MOUSE_BUTTON_1  ,
			Right = GLFW_MOUSE_BUTTON_2 ,
			Middle = GLFW_MOUSE_BUTTON_3,
			_3 = GLFW_MOUSE_BUTTON_4    ,
			_4 = GLFW_MOUSE_BUTTON_5    ,
			_5 = GLFW_MOUSE_BUTTON_6    ,
			_6 = GLFW_MOUSE_BUTTON_7    ,
			_7 = GLFW_MOUSE_BUTTON_8    ,
			Undefined
		};

		enum class KeyboardAction : Common::UInt64 {
			Pressed = GLFW_PRESS,
			Released = GLFW_RELEASE,
			Undefined
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
			keyboardEvents_.pop();
			return event;
		}

		[[nodiscard]]
		std::optional<MouseEvent> GetMouseEvent() noexcept {
			if (mouseEvents_.empty()) return {};
			MouseEvent event = mouseEvents_.front();
			mouseEvents_.pop();
			return event;
		}

		[[nodiscard]]
		std::optional<FrameBufferResizeEvent> GetFrameBufferResizeEvent() noexcept {
			if (frameBufferResizeEvents_.empty()) return {};
			FrameBufferResizeEvent event = frameBufferResizeEvents_.front();
			frameBufferResizeEvents_.pop();
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
			keyboardEvents_.push(event);
		}
		void PushEvent(const MouseEvent& event) noexcept {
			mouseEvents_.push(event);
		}
		void PushEvent(const FrameBufferResizeEvent& event) noexcept {
			frameBufferResizeEvents_.push(event);
		}

	private:
		std::queue<FrameBufferResizeEvent> frameBufferResizeEvents_;
		std::queue<KeyboardEvent> keyboardEvents_;
		std::queue<MouseEvent> mouseEvents_;

		CreateInfo createInfo_;

	};

}