#pragma once 

#include <OksEngine.ECS.Component.hpp>
#include <OksEngine.ECS.System.hpp>
#include <Resources/OksEngine.LoadResourceRequest.hpp>
#include <Resources/OksEngine.Resource.hpp>

namespace OksEngine {

	struct AddImmutableRenderGeometryFromObjRequest : public ECSComponent<AddImmutableRenderGeometryFromObjRequest> {
	public:
		AddImmutableRenderGeometryFromObjRequest() : ECSComponent{ nullptr } {

		}
		std::string obj_ = "";
		std::string mtl_ = "";
		std::vector<std::string> textures_;
		bool bakeGeometry_ = false;
	};


	class AddImmutableRenderGeometryFromObjSystem : public ECSSystem {
	public:

		AddImmutableRenderGeometryFromObjSystem(Context& context) noexcept : ECSSystem { context }{

		}

	public:

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId) override {
			const auto* request = world->GetComponent<AddImmutableRenderGeometryFromObjRequest>(entityId);
			if (request == nullptr) { return; }
			OS::AssertMessage(request->obj_ != "", "Obj name must be set.");
			world->CreateComponent<LoadResourceRequest>();
		}

	private:

		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<AddImmutableRenderGeometryFromObjSystem>().GetId();
		}
	};


}