
#include <RE.RenderEngine.hpp>
#include <RAL.API.hpp>

namespace RE {

	RenderEngine::RenderEngine(const RenderEngine::CreateInfo& createInfo) {

		api_ = RAL::CreateAPI();
		auto flatShadedModelPipeline = std::make_shared<RAL::Driver::Pipeline>(
			"Flat shading models",
			createInfo.vertexShader_,
			createInfo.fragmentShader_,
			true
		);

		auto texturedModelPipeline = std::make_shared<RAL::Driver::Pipeline>(
			"Textured models",
			createInfo.textureVertexShader_,
			createInfo.textureFragmentShader_,
			true
		);

		RAL::Driver::CreateInfo driverCreateInfo;
		{
			driverCreateInfo.flatShadedPipeline_ = flatShadedModelPipeline;
			driverCreateInfo.texturedPipeline_ = texturedModelPipeline;
			driverCreateInfo.surface_ = *createInfo.renderSurface_;
		}

		driver_ = api_->CreateDriver(driverCreateInfo);
		driver_->SetCamera(createInfo.camera_);
		driver_->AddLight(createInfo.light_);
	}

	[[nodiscard]]
	RenderEngine::Model RenderEngine::RenderModel(const RAL::Vector3f& position, const Geometry::Model<RAL::Vertex3fnt, RAL::Index16>& model) {

		Model drawnModel;

		Common::Index shapeIndex = 0;
		for (const auto& shape : model) {
			Geometry::VertexCloud<Geometry::Vertex3fnt> verticesColored;
			const auto& vertices = shape.GetVertices();
			for (const auto& vertex : vertices) {
				verticesColored.Add(vertex);
			}
			Common::Index shapeId = driver_->DrawIndexed(
				Math::Matrix4x4f::GetTranslate(position),
				(const RAL::Vertex3fnt*)verticesColored.GetData(),
				verticesColored.GetVerticesNumber(),
				shape.GetIndices().GetData(),
				shape.GetIndicesNumber(),
				shape.GetTexture()
			);
			drawnModel.shapes_.push_back({ shapeId });
		}

		return drawnModel;
	}

	[[nodiscard]]
	RenderEngine::Model RenderEngine::RenderModel(const RAL::Vector3f& position, const Geometry::Model<RAL::Vertex3fnc, RAL::Index16>& model) {
		Model drawnModel;
		for (const auto& shape : model) {
			Geometry::VertexCloud<Geometry::Vertex3fnc> verticesColored;
			//if (shapeIndex == 0) {
			//	shapeIndex++;
			//	continue;
			//}
			const auto& vertices = shape.GetVertices();
			for (const auto& vertex : vertices) {
				verticesColored.Add(vertex);
			}
			Common::Index shapeId = driver_->DrawIndexed(
				Math::Matrix4x4f::GetTranslate(position),
				(RAL::Vertex3fnc*)verticesColored.GetData(),
				verticesColored.GetVerticesNumber(),
				shape.GetIndices().GetData(),
				shape.GetIndicesNumber()/*, RAL::Color{ 1.f, 1.f, 1.f }*/);
			drawnModel.shapes_.push_back({ shapeId });
		}
		return drawnModel;
	}

	void RenderEngine::RotateModel(const RenderEngine::Model& model, const RAL::Vector3f& aroundVector, Math::Angle angle) {
		for (auto shape : model.shapes_) {
			driver_->Rotate(shape.id_, aroundVector, angle);
		}
	}


	void RenderEngine::SetModelMatrix(const RenderEngine::Model& model, const Math::Matrix4x4f& modelMatrix) {
		for (auto shape : model.shapes_) {
			driver_->SetModelMatrix(shape.id_, modelMatrix);
		}
	}

	void RenderEngine::Render() {
		//Geometry::Box box{ 1 };

		//Geometry::VertexCloud<RAL::Vertex3fc> coloredBox;
		//for (Common::Index i = 0; i < box.GetVertices().GetVerticesNumber(); i++) {
		//	Math::Vector3f color{ (float)((i * 100) % 255) / 255, (float)((i * 150) % 255) / 255, (float)((i * 199) % 255) / 255 };
		//	RAL::Vertex3fc coloredVertex = { box.GetVertices()[i], color };
		//	coloredBox.Add(coloredVertex);
		//}

		//DS::Vector<Geometry::VertexCloud<RAL::Vertex3fc>> plane;

		//for (int i = 0; i < 25; i++) {
		//	plane.PushBack(coloredBox);
		//}


		//Common::Index i = 0;
		//for (int x = -2; x < 2; x++) {
		//	for (int y = -2; y < 2; y++) {
		//		Geometry::VertexCloud<RAL::Vertex3fc>& box = plane[i];
		//		Math::Vector3f offsetVector{ (float)x, (float)y , 0 };
		//		const Math::Matrix4x4f offset = Math::Matrix4x4f::GetTranslate(offsetVector);
		//		for (RAL::Vertex3fc& vertex : box) {
		//			vertex.position_ = Math::TransformPoint(vertex.position_, offset);
		//		}
		//		++i;
		//	}
		//}

		//for (int i = 0; i < 25; i++) {
		//	Geometry::VertexCloud<RAL::Vertex3fc>& coloredBox = plane[i];
		//	driver_->DrawIndexed(
		//		(RAL::Vertex3fc*)coloredBox.GetData(),
		//		coloredBox.GetVerticesNumber(),
		//		box.GetIndices().GetData(),
		//		box.GetIndicesNumber()/*, RAL::Color{ 1.f, 1.f, 1.f }*/);
		//}

		driver_->StartRender();
		driver_->Render();
		driver_->EndRender();


		using namespace std::chrono_literals;
		std::chrono::high_resolution_clock::time_point now;
		static std::chrono::high_resolution_clock::time_point point = std::chrono::high_resolution_clock::now();;

		now = std::chrono::high_resolution_clock::now();
		auto delta = now - point;
		point = now;

		auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(delta).count();
		Common::Size framesPerSecond = Common::Limits<Common::Size>::Max();
		if (milliseconds != 0) {
			framesPerSecond = 1000 / milliseconds;
		}
		//OS::LogInfo("renderEngine", { "Frames per second {}", framesPerSecond });
	}

}