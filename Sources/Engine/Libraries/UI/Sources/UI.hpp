#pragma once 

namespace UI {

	enum class Subsystem {
		GLFW,
		WinAPI,
		Undefined
	};

	enum class Render {
		Vulkan,
		OpenGL
	};

}