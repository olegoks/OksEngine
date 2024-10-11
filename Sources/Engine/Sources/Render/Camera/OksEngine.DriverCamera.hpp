#pragma once 

#include <OksEngine.ECS.Component.hpp>

#include <RAL.Driver.hpp>

namespace OksEngine {
	struct DriverCamera : public ECSComponent<DriverCamera> {
		struct Matrices {
			alignas(16) glm::mat4 view_;
			alignas(16) glm::mat4 proj_;
		};

		DriverCamera(
			/*const Matrices& matrices, */RAL::Driver::UniformBuffer::Id UBId) :
			ECSComponent{ nullptr },
			//matrices_{ matrices },
			matricesBuffer_{ UBId } { }
		//Matrices matrices_;

		RAL::Driver::UniformBuffer::Id matricesBuffer_;
	};

}