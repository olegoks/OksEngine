#pragma once 

#include <OksEngine.ECS.System.hpp>

namespace OksEngine {
	class UpdateCameraTransformUniformBuffer : public ECSSystem {
	public:

		UpdateCameraTransformUniformBuffer(Context& context) noexcept;

	public:

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override;
		[[nodiscard]]
		std::string GetName() const noexcept override {
			return "UpdateCameraTransformUniformBuffer";
		}
		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override;
	private:

		virtual Common::TypeId GetTypeId() const noexcept override;
	};
}