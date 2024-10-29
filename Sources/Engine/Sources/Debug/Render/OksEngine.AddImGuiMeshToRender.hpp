#pragma once 

#include <OksEngine.ECS.System.hpp>

namespace OksEngine {



	class AddImGuiMeshToRender : public ECSSystem {
	public:

		AddImGuiMeshToRender(Context& context) noexcept : ECSSystem{ context } { }

	public:

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override;

		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override;
		[[nodiscard]]
		std::string GetName() const noexcept override {
			return "AddImGuiMeshToRender";
		}
		virtual Common::TypeId GetTypeId() const noexcept override;
	};


}