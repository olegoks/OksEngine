#pragma once

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	struct Behaviour : public ECSComponent<Behaviour> {

		Behaviour(const std::string& scriptName, const std::string& objectName);

		std::string scriptName_;
		std::string objectName_;
	};

	template<>
	void Edit<Behaviour>(Behaviour* behaviour);

	template<>
	void Add<Behaviour>(ECS::World* world, ECS::Entity::Id id);

}