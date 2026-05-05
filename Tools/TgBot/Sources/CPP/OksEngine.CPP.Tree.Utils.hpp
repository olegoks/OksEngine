#pragma once 


#define CPP__FILE__CREATE_INCLUDE_ENTITY(includePath)\
[this](const std::string& path)->ECS2::Entity::Id {\
	ECS2::Entity::Id includeEntityId = CreateEntity();\
	CreateComponent<CPP::File::Include::Tag>(includeEntityId);\
	CreateComponent<CPP::File::Include::Path>(includeEntityId, path);\
	return includeEntityId;\
	}(includePath)