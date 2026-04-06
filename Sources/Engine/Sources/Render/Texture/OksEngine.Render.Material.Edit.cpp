#include <Render/Texture/auto_OksEngine.Render.Material.hpp>

namespace OksEngine::Render::Material {

	void EditInfoResourceSet(std::shared_ptr<ECS2::World> ecsWorld, ECS2::Entity::Id ecsEntityId,
		InfoResourceSet* infoResourceSet) {
		ImGui::PushID(InfoResourceSet::GetTypeId());
		////Info resource set for image with all need ids.
		//ImGui::Image(infoResourceSet->RSId_.GetValue(), {200, 200});
		ImGui::PopID();
	}

}