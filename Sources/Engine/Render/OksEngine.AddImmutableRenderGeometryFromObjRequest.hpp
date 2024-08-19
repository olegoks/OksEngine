#pragma once 

#include <OksEngine.ECS.Component.hpp>
#include <OksEngine.ECS.System.hpp>
#include <Resources/OksEngine.LoadResourceRequest.hpp>
#include <Resources/OksEngine.Resource.hpp>

namespace OksEngine {

	struct AddImmutableRenderGeometryFromObjRequest : public ECSComponent<AddImmutableRenderGeometryFromObjRequest> {
	public:

		AddImmutableRenderGeometryFromObjRequest(
			std::string obj,
			std::string mtl, 
			const std::vector<std::string>& textures, 
			bool bakeGeometry = false) : ECSComponent{ nullptr },
			obj_{ obj },
			mtl_{ mtl },
			textures_{ textures },
			bakeGeometry_{ bakeGeometry } { }

		std::string obj_ = "";
		std::string mtl_ = "";
		std::vector<std::string> textures_;
		bool bakeGeometry_ = false;
	};

	template<>
	inline void Edit<AddImmutableRenderGeometryFromObjRequest>(AddImmutableRenderGeometryFromObjRequest* request) {
		ImGui::TextDisabled("Obj: %s", request->obj_.c_str());
		ImGui::TextDisabled("Mtl: %s", request->mtl_.c_str());
		ImGui::TextDisabled("Textures:");
		for (auto& texture : request->textures_) {
			ImGui::TextDisabled("	%s", texture.c_str());
		}
	}


	class AddImmutableRenderGeometryFromObjSystem : public ECSSystem {
	public:

		AddImmutableRenderGeometryFromObjSystem(Context& context) noexcept : ECSSystem { context }{

		}

	public:

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override {
			const auto* request = world->GetComponent<AddImmutableRenderGeometryFromObjRequest>(entityId);
			if (request == nullptr) { return; }
			OS::AssertMessage(request->obj_ != "", "Obj name must be set.");
			//world->CreateComponent<LoadResourceRequest>();
		}

	private:
		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<AddImmutableRenderGeometryFromObjSystem>().GetId();
		}
	};





}