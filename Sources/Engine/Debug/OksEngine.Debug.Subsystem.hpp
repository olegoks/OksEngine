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
			
			const ECS::Entity::Id imgui = GetContext().GetECSWorld()->CreateEntity();

			GetContext().GetECSWorld()->CreateComponent<ImGuiContext>(imgui);
			GetContext().GetECSWorld()->CreateComponent<MainMenuBar>(imgui);
			GetContext().GetECSWorld()->CreateComponent<EnginePerformance>(imgui);
			GetContext().GetECSWorld()->CreateComponent<ECSInspector>(imgui);
			GetContext().GetECSWorld()->CreateComponent<FramesCounter>(imgui);

			GetContext().GetECSWorld()->RegisterSystem<ImGuiSystem>(createInfo.context_);
			GetContext().GetECSWorld()->RegisterSystem<MainMenuBarSystem>(createInfo.context_);
			GetContext().GetECSWorld()->RegisterSystem<EnginePerformanceSystem>(createInfo.context_);
			GetContext().GetECSWorld()->RegisterSystem<CollectEntitiesInfo>(createInfo.context_);
			GetContext().GetECSWorld()->RegisterSystem<ECSInspectorSystem>(createInfo.context_);

		} 

		virtual void Update() override {

		}

	};

}