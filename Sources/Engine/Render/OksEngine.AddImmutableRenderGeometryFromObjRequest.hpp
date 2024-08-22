#pragma once 

#include <OksEngine.ECS.Component.hpp>
#include <OksEngine.ECS.System.hpp>
#include <Resources/OksEngine.LoadResourceRequest.hpp>
#include <Resources/OksEngine.Resource.hpp>
#include <Render/OksEngine.Render.Subsystem.hpp>

namespace OksEngine {

	struct ImmutableRenderGeometryObj : public ECSComponent<ImmutableRenderGeometryObj> {
	public:

		ImmutableRenderGeometryObj(
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

		Common::Index modelIndex_ = Common::Limits<Common::Index>::Max();
	};

	template<>
	inline void Edit<ImmutableRenderGeometryObj>(ImmutableRenderGeometryObj* request) {
		ImGui::TextDisabled("Obj: %s", request->obj_.c_str());
		ImGui::TextDisabled("Mtl: %s", request->mtl_.c_str());
		ImGui::TextDisabled("Textures:");
		for (auto& texture : request->textures_) {
			ImGui::TextDisabled("	%s", texture.c_str());
		}
	}


	template<>
	inline void Add<ImmutableRenderGeometryObj>(ECS::World* world, ECS::Entity::Id id) {
		static char obj[100] = { "dragon_lore.obj" }; // Test mesh 
		static char mtl[100] = { "dragon_lore.mtl" };
		static Common::Size texturesNumber = 1;
		static std::vector<std::unique_ptr<char[]>> textures;
		if (texturesNumber <= textures.size()) {
			textures.resize(texturesNumber);
		}
		else {
			for (Common::Size i = 0; i < texturesNumber - textures.size(); i++) {
				auto newTexture = std::make_unique<char[]>(100);
				std::memset(newTexture.get(), 0, 100);
				const char* testTexture = "dragon_lore.bmp";
				std::memcpy(newTexture.get(), testTexture, std::strlen(testTexture));
				textures.push_back(std::move(newTexture));
			}
		}


		if (ImGui::CollapsingHeader("Create info")) {
			ImGui::InputText("Obj", obj, sizeof(obj));
			ImGui::InputText("Mtl", mtl, sizeof(mtl));
			{
				ImGui::TextDisabled("Textures:");
				for (Common::Size i = 0; i < texturesNumber; i++) {
					ImGui::PushID(std::to_string(i).c_str());
					ImGui::InputText("", (char*)textures[i].get(), 100); ImGui::SameLine();
					if (ImGui::SmallButton("-")) { textures.erase(textures.begin() + i); --texturesNumber; }
					ImGui::PopID();
				}
				if (ImGui::SmallButton("+")) { ++texturesNumber; }
			}
			ImGui::Spacing();
		}
		if (ImGui::Button("Add component")) {
			if (!world->IsComponentExist<ImmutableRenderGeometryObj>(id)) {
				std::vector<std::string> texturesStrs;
				for (auto& textureInput : textures) {
					texturesStrs.push_back(textureInput.get());
				}
				world->CreateComponent<ImmutableRenderGeometryObj>(id, obj, mtl, texturesStrs);
			}
		}
	}

	class AddImmutableRenderGeometryFromObjSystem : public ECSSystem {
	public:

		AddImmutableRenderGeometryFromObjSystem(Context& context) noexcept : ECSSystem { context }{

		}

	public:

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override {
			auto* request = world->GetComponent<ImmutableRenderGeometryObj>(entityId);
			if (request->modelIndex_ == Common::Limits<Common::Index>::Max()) {
				request->modelIndex_ = GetContext().GetRenderSubsystem()->RenderModel(request->obj_, request->mtl_, request->textures_);
			}
		}

		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override {
			return { ECS::Entity::Filter{}.Include<ImmutableRenderGeometryObj>(), ECS::Entity::Filter{}.ExcludeAll() };
		}
	private:
		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<AddImmutableRenderGeometryFromObjSystem>().GetId();
		}
	};





}