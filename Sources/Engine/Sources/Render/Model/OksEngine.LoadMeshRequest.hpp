#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {


struct LoadMeshRequest : public ECSComponent<LoadMeshRequest> {

	LoadMeshRequest() : ECSComponent{ nullptr } { }

	enum class Type {
		OBJ_MTL_TEXTURES,
		OBJ_MTL,
		OBJ,
		FBX,
		FBX_TEXTURES,
		Undefined
	};

	LoadMeshRequest(const std::string& name, std::map<std::string, ECS::Entity::Id> resourceEntityId, Type type)
		: ECSComponent{ nullptr },
		resourceEntityId_{ resourceEntityId },
		type_{ type }, name_{ name } {}

	std::map<std::string, ECS::Entity::Id> resourceEntityId_;
	Type type_ = Type::Undefined;
	std::string name_;
};
}