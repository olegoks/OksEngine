#pragma once

#include <OS.FileSystem.BinaryFile.hpp>

namespace Render::Vulkan {

	class TextShader /*: private OS::BinaryFile */{
	public:
		TextShader() /*: OS::BinaryFile{ path }*/ {

		}


		void Compile();
	};

}