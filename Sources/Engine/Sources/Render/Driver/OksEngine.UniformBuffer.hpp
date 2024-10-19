#pragma once 

#include <OksEngine.ECS.Component.hpp>

#include <RAL.Driver.hpp>

namespace OksEngine {

	struct UniformBuffer : public ECSComponent<UniformBuffer> {
	public:

		UniformBuffer() : ECSComponent{ nullptr } { }
		UniformBuffer(RAL::Driver::UniformBuffer::Id id) :
			ECSComponent{ nullptr },
			id_{ id } {}

		RAL::Driver::UniformBuffer::Id id_;
	};

	template<>
	inline void Edit<UniformBuffer>(UniformBuffer* uniformBuffer) {
		ImGui::TextDisabled("id: %d", uniformBuffer->id_);

	}


}