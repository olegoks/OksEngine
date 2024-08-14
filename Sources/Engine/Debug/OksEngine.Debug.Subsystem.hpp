#pragma once 

#include <Common/OksEngine.Subsystem.hpp>
#include <Debug/OksEngine.MainMenuBar.hpp>
#include <Common/OksEngine.FramesCounter.hpp>

namespace OksEngine {

	class DebugSubsystem : public Subsystem {
	public:

		struct CreateInfo {
			Context& context_;
		};
		
		DebugSubsystem(const CreateInfo& createInfo) : Subsystem{ Subsystem::Type::Debug, createInfo.context_ } {
			
			const ECS::Entity::Id mainMenuBar = GetContext().GetECSWorld()->CreateEntity();

			GetContext().GetECSWorld()->CreateComponent<MainMenuBar>(mainMenuBar);
			GetContext().GetECSWorld()->CreateComponent<EnginePerformance>(mainMenuBar);
			GetContext().GetECSWorld()->CreateComponent<ECSInspector>(mainMenuBar);
			GetContext().GetECSWorld()->CreateComponent<FramesCounter>(mainMenuBar);


			GetContext().GetECSWorld()->RegisterSystem<MainMenuBarSystem>(createInfo.context_);
			GetContext().GetECSWorld()->RegisterSystem<EnginePerformanceSystem>(createInfo.context_);
			GetContext().GetECSWorld()->RegisterSystem<ECSInspectorSystem>(createInfo.context_);

		} 

		virtual void Update() override {

		}

	};

}