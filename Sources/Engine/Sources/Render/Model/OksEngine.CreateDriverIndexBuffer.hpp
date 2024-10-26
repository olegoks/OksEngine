#pragma once 

#include <OksEngine.ECS.System.hpp>

namespace OksEngine {

	class CreateDriverIndexBuffer : public ECSSystem {
	public:

		CreateDriverIndexBuffer(Context& context) noexcept : ECSSystem{ context } { }

	public:
		[[deprecated]]
		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override;
		[[nodiscard]]
		[[deprecated]]
		std::string GetName() const noexcept override {
			return "CreateDriverIndexBuffer";
		}
		[[deprecated]]
		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override;

	private:
		[[deprecated]]
		virtual Common::TypeId GetTypeId() const noexcept override;
	};

}