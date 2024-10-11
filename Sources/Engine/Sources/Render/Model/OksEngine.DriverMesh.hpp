#pragma once 

#include <OksEngine.ECS.Component.hpp>

#include <RAL.Driver.hpp>

namespace OksEngine {





	struct DriverMesh : public ECSComponent<DriverMesh> {
	public:

		DriverMesh() : ECSComponent{ nullptr } { }
		DriverMesh(Common::Id id) :
			ECSComponent{ nullptr },
			id_{ id } {}

		Common::Id id_;
	};

	template<>
	inline void Edit<DriverMesh>(DriverMesh* mesh) {
		ImGui::TextDisabled("DriverMesh id: \"%d\"", mesh->id_);
	}




}