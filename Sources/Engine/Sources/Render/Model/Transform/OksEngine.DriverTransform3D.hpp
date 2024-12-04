#pragma once

#include <OksEngine.ECS.Component.hpp>

#include <Render/Driver/OksEngine.UniformBuffer.hpp>

namespace OksEngine {

	struct DriverTransform3D : public ECSComponent<DriverTransform3D> {

		struct Transform {
			glm::mat4 model_;
		};

		DriverTransform3D(RAL::Driver::UniformBuffer::Id UBId) : 
			ECSComponent<DriverTransform3D>{ nullptr },
			UBId_{ UBId } { }

		RAL::Driver::UniformBuffer::Id UBId_ = RAL::Driver::UniformBuffer::Id::Invalid();
	};


	template<>
	inline void Edit<DriverTransform3D>(DriverTransform3D* driverTransform3D) {
		ImGui::TextDisabled("Uniform buffer Id:%d", driverTransform3D->UBId_);
	}


	template<>
	inline void Add<DriverTransform3D>(ECS::World* world, ECS::Entity::Id id) {
		
	}

}
