#pragma once 

#include <OksEngine.ECS.System.hpp>

namespace OksEngine {


	class CreateImGuiTexture : public ECSSystem {
	public:
		CreateImGuiTexture(Context& context) : ECSSystem{ context } { }
	public:

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override;

	private:
		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override;

		virtual Common::TypeId GetTypeId() const noexcept override;


	};

}