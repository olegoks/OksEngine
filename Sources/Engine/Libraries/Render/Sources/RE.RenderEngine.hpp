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

		class Model {
		public:

			struct Shape {
				Common::Index id_ = 0;
				RAL::Texture::Id texture_ = RAL::Texture::Id::Invalid();
			};

			std::vector<Shape> shapes_;
		};

		//[[nodiscard]]
		//Model RenderModel(const glm::mat4& position, const Geometry::Model<RAL::Vertex3fnt, RAL::Index16>& model);

		//[[nodiscard]]
		//Model RenderModel(const  glm::mat4& position, const Geometry::Model<RAL::Vertex3fnc, RAL::Index16>& model);

		//void SetModelMatrix(const Model& model, const  glm::mat4& modelMatrix);

		void SetCamera(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up);

		void Render();

		std::shared_ptr<class RAL::Driver> GetDriver() noexcept { return driver_; }

	private:

		RAL::Texture::Id imguiTextureId_ = RAL::Texture::Id::Invalid();

		std::shared_ptr<class RAL::API> api_ = nullptr;
		std::shared_ptr<class RAL::Driver> driver_ = nullptr;
	};

}