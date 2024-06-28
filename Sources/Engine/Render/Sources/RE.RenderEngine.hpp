#pragma once
#include <chrono>
#include <Math.hpp>
#include <Geometry.Shapes.hpp>
#include <Geometry.Model.hpp>
#include <RAL.hpp>

namespace RE {

	class [[nodiscard]] RenderEngine {
	public:

		struct CreateInfo {
			std::shared_ptr<RAL::Camera> camera_ = nullptr;
			std::shared_ptr<RAL::Light> light_ = nullptr;
			std::shared_ptr<RAL::Shader> vertexShader_ = nullptr;
			std::shared_ptr<RAL::Shader> fragmentShader_ = nullptr;
			std::shared_ptr<RAL::Shader> textureVertexShader_ = nullptr;
			std::shared_ptr<RAL::Shader> textureFragmentShader_ = nullptr;
			std::shared_ptr<RAL::RenderSurface> renderSurface_ = nullptr;
		};

		RenderEngine(const CreateInfo& createInfo);

		class Model {
		public:

			struct Shape {
				Common::Index id_ = 0;
			};

			std::vector<Shape> shapes_;
		};

		[[nodiscard]]
		Model RenderModel(const RAL::Vector3f& position, const Geometry::Model<RAL::Vertex3fnt, RAL::Index16>& model);

		[[nodiscard]]
		Model RenderModel(const RAL::Vector3f& position, const Geometry::Model<RAL::Vertex3fnc, RAL::Index16>& model);

		void RotateModel(const Model& model, const RAL::Vector3f& aroundVector, Math::Angle angle);

		void SetModelMatrix(const Model& model, const Math::Matrix4x4f& modelMatrix);

		void Render();

	private:

		std::shared_ptr<class RAL::API> api_ = nullptr;
		std::shared_ptr<class RAL::Driver> driver_ = nullptr;
	};

}