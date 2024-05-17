#pragma once

export module OksEngine.Render.Vulkan.Shader:TextShader;

import OksEngine.OS.FileSystem.BinaryFile;

namespace Render::Vulkan {

	class TextShader /*: private OS::BinaryFile */{
	public:
		TextShader() /*: OS::BinaryFile{ path }*/ {

		}


		void Compile();
	};

}