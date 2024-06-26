#pragma once

//#include <Vulkan/Render.Vulkan.hpp>

#include <memory>
#include <filesystem>

//#include <Engine.ECS;
//#include <RAL;
//#include <UIAL;
//#include <Resource;
//#include <OksEngine.Render.Subsystem.hpp>
#include <ECS.hpp>

#include <Geometry.Model.hpp>

#include "Geometry.Shapes.hpp"


namespace OksEngine {

	struct ImmutableRenderGeometry : public ECSComponent<ImmutableRenderGeometry> {
		Common::Index modelId_ = 0;
		Math::Matrix4x4f modelMatrix_ = Math::Matrix4x4f::GetIdentity();
		std::string modelObjFileName_ = "";
		std::string modelMtlFileName_ = "";
		std::string modelTextureFileName_ = "";

		ImmutableRenderGeometry(
			Context& context,
			const Math::Matrix4x4f& modelMatrix,
			std::string objFileName,
			std::string mtlFileName,
			std::string textureFileName) :
			ECSComponent{ context },
			modelMatrix_{ modelMatrix },
			modelObjFileName_{ objFileName },
			modelMtlFileName_{ modelMtlFileName_ },
			modelTextureFileName_{ textureFileName } {}
	};

	class RenderSystem : public ECS::System {
	public:

		RenderSystem(Context& context) noexcept : 
			context_{ context } { }

	public:
		
		virtual void Update(ECS::World* world, ECS::Entity::Id entityId) const override {
			auto* immutableRenderGeometry = world->GetComponent<ImmutableRenderGeometry>(entityId);
			if (immutableRenderGeometry == nullptr) return;
			//context_.GetRenderSubsystem()->

		}

	private:

		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<RenderSystem>().GetId();
		}

		Context& context_;
	};



	struct DebugRenderableGeometry : public ECS::IComponent<DebugRenderableGeometry> {
		Geometry::Model<Geometry::Vertex3fc> model_;

		DebugRenderableGeometry() {
			Geometry::Box box{ 1 };

			Geometry::VertexCloud<Geometry::Vertex3fc> coloredBox;
			for (Common::Index i = 0; i < box.GetVertices().GetVerticesNumber(); i++) {
				Math::Vector3f color{ (float)((i * 100) % 255) / 255, (float)((i * 150) % 255) / 255, (float)((i * 199) % 255) / 255 };
				Geometry::Vertex3fc coloredVertex = { box.GetVertices()[i], color };
				coloredBox.Add(coloredVertex);
			}

			Geometry::Shape shape{ coloredBox, box. GetIndices() };
			model_.AddShape(shape);
		}
	};

}
