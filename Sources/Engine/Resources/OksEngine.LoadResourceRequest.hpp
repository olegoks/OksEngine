#pragma once 

#include <OksEngine.ECS.Component.hpp>
#include <OksEngine.ECS.System.hpp>
#include <Resources/OksEngine.Resource.Subsystem.hpp>
#include <Resources/OksEngine.Resource.hpp>

namespace OksEngine {

	class LoadResourceRequest : public ECSComponent<LoadResourceRequest> {
	public:

		LoadResourceRequest(const std::string& resourceName) noexcept 
			: ECSComponent{ nullptr }, 
			resourceName_{ resourceName } {

		}

		AsyncResourceSubsystem::Task::Id taskId_ = Common::Limits<Common::Index>::Max();
		std::string resourceName_;
	private:
	};


	template<>
	inline void Edit<LoadResourceRequest>(LoadResourceRequest* request) {
		ImGui::TextDisabled("Resource name: %s", request->resourceName_.c_str());
	}

	class LoadResourceSystem : public ECSSystem {
	public:

		LoadResourceSystem(Context& context) noexcept : ECSSystem{ context } {

		}

	public:

		virtual void Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) override;

		virtual std::pair<ECS::Entity::Filter, ECS::Entity::Filter> GetFilter() const noexcept override {
			return { ECS::Entity::Filter{}.Include<LoadResourceRequest>(), ECS::Entity::Filter{}.Exclude<Resource>() };
		}

	private:

		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<LoadResourceSystem>().GetId();
		}
	};


}