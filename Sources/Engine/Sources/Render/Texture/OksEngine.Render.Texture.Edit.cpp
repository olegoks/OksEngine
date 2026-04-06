#include <Render/Texture/OksEngine.Render.Texture.Utils.hpp>

namespace OksEngine::Render::Texture {

	void EditBindingArrayIndex(std::shared_ptr<ECS2::World> ecsWorld, ECS2::Entity::Id entityId, BindingArrayIndex* bindingArrayIndex) {
		ImGui::PushID(BindingArrayIndex::GetTypeId());
		ImGui::Image(bindingArrayIndex->index_, { 500, 500 });
		ImGui::PopID();
	}

}







