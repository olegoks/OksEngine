#include <Resources/OksEngine.CheckResourceLoaded.hpp>

#include <Resources/OksEngine.LoadResourceRequest.hpp>
#include <Resources/OksEngine.Resource.hpp>

namespace OksEngine {

	//void CheckResourceLoaded::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

	//	auto* loadResourceRequest1 = world->GetComponent<LoadResourceRequest>(entityId);
	//	auto* resource1 = world->GetComponent<Resource>(entityId);

	//	auto* loadResourceRequest2 = world->GetComponent<LoadResourceRequest>(secondEntityId);

	//	if (loadResourceRequest1->resourceName_== loadResourceRequest2->resourceName_) {
	//		world->CreateComponent<Resource>(secondEntityId, loadResourceRequest2->resourceName_, resource1->resourceData_);
	//	}

	//}

	//std::string CheckResourceLoaded::GetName() const noexcept
	//{
	//	return "CheckResourceLoaded";
	//}

	//std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CheckResourceLoaded::GetFilter() const noexcept {
	//	return {
	//		ECS::Entity::Filter{}
	//		.Include<LoadResourceRequest>()
	//		.Include<Resource>(),
	//		ECS::Entity::Filter{}
	//		.Include<LoadResourceRequest>()
	//		.Exclude<Resource>() };
	//}

	//Common::TypeId CheckResourceLoaded::GetTypeId() const noexcept {
	//	return Common::TypeInfo<CheckResourceLoaded>().GetId();
	//}

	//CheckResourceLoaded::CheckResourceLoaded(Context& context) noexcept :
	//	ECSSystem{ context } { }

}