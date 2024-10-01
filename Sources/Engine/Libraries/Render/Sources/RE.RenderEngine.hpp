#pragma once

#include <Math.hpp>
#include <Geometry.Shapes.hpp>
#include <Geometry.Model.hpp>
#include <RAL.hpp>
#include <RAL.Texture.hpp>
namespace RE {

	class [[nodiscard]] RenderEngine {
	public:

		struct CreateInfo {
			//std::shared_ptr<RAL::Camera> camera_ = nullptr;
			//std::shared_ptr<RAL::Light> light_ = nullptr;
			std::string imguiVertexShader_;
			std::string imguiFragmentShader_;
			std::string linesVertexShader_;
			std::string linesFragmentShader_;
			std::string vertexShader_;
			std::string fragmentShader_;
			std::string textureVertexShader_;
			std::string textureFragmentShader_;
			std::shared_ptr<RAL::RenderSurface> renderSurface_ = nullptr;
		};

		RenderEngine(const CreateInfo& createInfo);


		void Render();

		std::shared_ptr<class RAL::Driver> GetDriver() noexcept { return driver_; }

	private:

		//RAL::Texture::Id imguiTextureId_ = RAL::Texture::Id::Invalid();

		std::shared_ptr<class RAL::API> api_ = nullptr;
		std::shared_ptr<class RAL::Driver> driver_ = nullptr;
	};

}