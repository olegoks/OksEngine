#pragma once 

#include <OksEngine.ECS.Component.hpp>
#include <OksEngine.ECS.System.hpp>
#include <Math.Matrix.hpp>
#include <RAL.Driver.hpp>

namespace OksEngine {

	struct Mtl : public ECSComponent<Mtl> {

		Mtl() :
			ECSComponent{ nullptr } { }

		Mtl(const std::string& name, const std::string& data) :
			ECSComponent{ nullptr },
			name_{ name },
			data_{ data } { }

		std::string name_;
		std::string data_;
	};

	template<>
	inline void Edit<Mtl>(Mtl* mtl) {
		ImGui::TextDisabled("Name: %s", mtl->name_.c_str());
		ImGui::TextDisabled("Data: %s", mtl->data_.c_str());

	}


}