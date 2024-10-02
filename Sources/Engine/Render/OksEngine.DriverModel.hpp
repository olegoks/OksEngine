#pragma once 

#include <OksEngine.ECS.Component.hpp>

#include <RAL.Driver.hpp>

namespace OksEngine {





	struct DriverModel : public ECSComponent<DriverModel> {
	public:

		struct Mesh {
			Common::Id id_;
			std::vector<RAL::Driver::ShaderBinding::Data> shaderBindings_;
		};

		DriverModel() : ECSComponent{ nullptr } { }
		DriverModel(std::vector<Mesh>& driverMeshs) :
			ECSComponent{ nullptr },
			driverMeshs_{ driverMeshs } {}

		std::vector<Mesh> driverMeshs_;
	};




}