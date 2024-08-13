#pragma once 

#include <OksEngine.ECS.Component.hpp>
#include <OksEngine.ECS.System.hpp>
#include <Resources/OksEngine.Resource.Subsystem.hpp>

namespace OksEngine {

	class LoadResourceRequest : public ECSComponent<LoadResourceRequest> {
	public:

		LoadResourceRequest(const std::string& resourceName) noexcept 
			: ECSComponent{ nullptr }, 
			resourceName_{ resourceName } {

		}

		AsyncResourceSubsystem::Task::Id taskId_ = 0;
		std::string resourceName_;
	private:
	};


	class LoadResourceSystem : public ECSSystem {
	public:

		LoadResourceSystem(Context& context) noexcept : ECSSystem{ context } {

		}

	public:

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId) override;

	private:

		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<LoadResourceSystem>().GetId();
		}
	};


}