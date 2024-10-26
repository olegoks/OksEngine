#pragma once 

#include <OksEngine.ECS.Component.hpp>

#include <RAL.Driver.hpp>

namespace OksEngine {

	struct DriverIndexBuffer : public ECSComponent<DriverIndexBuffer> {
	public:

		DriverIndexBuffer() : ECSComponent{ nullptr } { }
		DriverIndexBuffer(RAL::Driver::IndexBuffer::Id id) :
			ECSComponent{ nullptr },
			id_{ id } {}

		RAL::Driver::IndexBuffer::Id id_;
	};

	template<>
	inline void Edit<DriverIndexBuffer>(DriverIndexBuffer* indexBuffer) {
		ImGui::TextDisabled("id: %d", indexBuffer->id_);

	}


}