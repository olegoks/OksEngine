#include <Render/Texture/OksEngine.Render.Texture.Utils.hpp>

namespace OksEngine::Render::Texture {

	void EditBindingArrayIndex(std::shared_ptr<ECS2::World> ecsWorld, ECS2::Entity::Id entityId, BindingArrayIndex* bindingArrayIndex) {
		ImGui::PushID(BindingArrayIndex::GetTypeId());

		ImGui::Begin("Texture Viewer");
		ImGui::Image(bindingArrayIndex->index_, { 600, 600 });
		ImGui::End();
		ImGui::PopID();
	}

}







