#pragma once 

#include <OksEngine.ECS.Component.hpp>

#include <RAL.Driver.hpp>

namespace OksEngine {

	struct DriverVertexBuffer : public ECSComponent<DriverVertexBuffer> {
	public:

		DriverVertexBuffer() : ECSComponent{ nullptr } { }
		DriverVertexBuffer(RAL::Driver::VertexBuffer::Id id) :
			ECSComponent{ nullptr },
			id_{ id } {}

		RAL::Driver::VertexBuffer::Id id_;
	};

	template<>
	inline void Edit<DriverVertexBuffer>(DriverVertexBuffer* vertexBuffer) {
		ImGui::TextDisabled("id: %d", vertexBuffer->id_);

	}


}