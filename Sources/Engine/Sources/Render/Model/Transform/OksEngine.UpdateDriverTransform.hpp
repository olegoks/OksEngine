#pragma once 

#include <OksEngine.ECS.System.hpp>

namespace OksEngine {

	class UpdateDriverTransform3D : public ECSSystem {
	public:

		UpdateDriverTransform3D(Context& context) noexcept : ECSSystem{ context } { }

	public:
		[[deprecated]]
		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override;
		[[nodiscard]]
		[[deprecated]]
		std::string GetName() const noexcept override {
			return "UpdateDriverTransform3D";
		}
		[[deprecated]]
		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override;

	private:
		[[deprecated]]
		virtual Common::TypeId GetTypeId() const noexcept override;
	};

	class UpdateModelDriverTransform : public ECSSystem {
	public:

		UpdateModelDriverTransform(Context& context) noexcept : ECSSystem{ context } { }

	public:
		[[deprecated]]
		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override;
		[[nodiscard]]
		[[deprecated]]
		std::string GetName() const noexcept override {
			return "UpdateModelDriverTransform";
		}
		[[deprecated]]
		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override;

	private:
		[[deprecated]]
		virtual Common::TypeId GetTypeId() const noexcept override;
	};

}