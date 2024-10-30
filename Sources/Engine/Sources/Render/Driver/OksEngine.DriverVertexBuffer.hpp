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


	struct MutableDriverVertexBuffer : public ECSComponent<MutableDriverVertexBuffer> {
	public:

		MutableDriverVertexBuffer() : ECSComponent{ nullptr } { }
		MutableDriverVertexBuffer(RAL::Driver::VertexBuffer::Id id) :
			ECSComponent{ nullptr },
			id_{ id } {}

		RAL::Driver::VertexBuffer::Id id_;
	};

	template<>
	inline void Edit<MutableDriverVertexBuffer>(MutableDriverVertexBuffer* vertexBuffer) {
		ImGui::TextDisabled("id: %d", vertexBuffer->id_);

	}

	struct ConstDriverVertexBuffer : public ECSComponent<ConstDriverVertexBuffer> {
	public:

		ConstDriverVertexBuffer() : ECSComponent{ nullptr } { }
		ConstDriverVertexBuffer(RAL::Driver::VertexBuffer::Id id) :
			ECSComponent{ nullptr },
			id_{ id } {}

		RAL::Driver::VertexBuffer::Id id_;
	};

	template<>
	inline void Edit<ConstDriverVertexBuffer>(ConstDriverVertexBuffer* vertexBuffer) {
		ImGui::TextDisabled("id: %d", vertexBuffer->id_);

	}


}