#pragma once 

#include <OksEngine.ECS.Component.hpp>
#include <OksEngine.ECS.System.hpp>
#include <Math.Matrix.hpp>
#include <RAL.Driver.hpp>

namespace OksEngine {

	struct Obj : public ECSComponent<Obj> {

		Obj() :
			ECSComponent{ nullptr } { }

		Obj(const std::string& name, const std::string& data) :
			ECSComponent{ nullptr },
			name_{ name },
			data_{ data } { }

		std::string name_;
		std::string data_;
	};

	template<>
	inline void Edit<Obj>(Obj* obj) {
		ImGui::TextDisabled("Name: %s", obj->name_.c_str());
		ImGui::TextDisabled("Data: %s", obj->data_.c_str());

	}


}