#include <Resources/auto_OksEngine.WaitForResourceLoading.hpp>

#include <Resources/OksEngine.Subsystem.hpp>


namespace OksEngine {

	void WaitForResourceLoading::Update(
		ECS2::Entity::Id entity1Id,
		const LoadResourceRequest* loadResourceRequest1,
		const Name* name1,
		const WaitingForResourceLoading* waitingForResourceLoading1,
		ECS2::Entity::Id entity2Id,
		const Resource* resource2,
		const BinaryData* binaryData2) {
		//auto* resourceName = world->GetComponent<Name>(entityId);
		//if (resourceName->value_ == world->GetComponent<Name>(secondEntityId)->value_) {
		//	world->CreateComponent<ResourceEntity>(entityId, secondEntityId);
		//	world->CreateComponent<Completed>(entityId);
		//}
	}

	// std::string WaitForResourceLoading::GetName() const noexcept
	// {
	// 	return "WaitForResourceLoading";
	// }

	//std::pair<ECS::Entity::Filter, ECS::Entity::Filter> WaitForResourceLoading::GetFilter() const noexcept {
	//	static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter =
	//	{
	//	ECS::Entity::Filter{}
	//	.Include<LoadResourceRequest>()
	//	.Include<Name>()
	//	.Include<WaitingForResourceLoading>()
	//	.Exclude<ResourceEntity>(),
	//	ECS::Entity::Filter{}
	//.Include<Name>()
	//	.Include<Resource>()
	//.Include<BinaryData>() };
	//	return filter;
	//}

	// Common::TypeId WaitForResourceLoading::GetTypeId() const noexcept {
	// 	return Common::TypeInfo<WaitForResourceLoading>().GetId();
	// }

	// WaitForResourceLoading::WaitForResourceLoading(Context& context) noexcept :
	// 	ECSSystem{ context } { }

}