#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	struct Mesh : public ECSComponent<Mesh> {
	public:
		Mesh() : ECSComponent{ nullptr } {

		}
		Mesh(/*const std::string& tag, */const Geom::Model2::Id& modelId) :
			ECSComponent{ nullptr }, modelId_{ modelId } {}
		Geom::Model2::Id modelId_;
		//ModelStorage::Model::Id model_;
		//Common::Id driverModelId_;
	};






	template<>
	inline void Edit<Mesh>(Mesh* mesh) {
		//ImGui::TextDisabled("Tag: \"%s\"", mesh->tag_.c_str());
		//ImGui::TextDisabled("Id: %d", mesh->meshId_);
	}


}