#pragma once 

#include <OksEngine.ECS.System.hpp>

namespace OksEngine {



	class CreateDriverModel : public ECSSystem {
	public:

		CreateDriverModel(Context& context) noexcept : ECSSystem{ context } { }

	public:

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override;

		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override;
		[[nodiscard]]
		std::string GetName() const noexcept override {
			return "CreateDriverModel";
		}
		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<CreateDriverModel>().GetId();
		}
	};


}