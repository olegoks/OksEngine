#include <Debug/OksEngine.Debug.Subsystem.hpp>

#include <Debug/OksEngine.Debug.Components.hpp>
#include <Debug/OksEngine.DebugUI.System.hpp>


namespace OksEngine {

	DebugSubsystem::DebugSubsystem(const CreateInfo& createInfo) : Subsystem{ Subsystem::Type::Debug, createInfo.context_ } {

		const ECS::Entity::Id imgui = GetContext().GetECSWorld()->CreateEntity();

		GetContext().GetECSWorld()->CreateComponent<ImGuiState>(imgui);
		GetContext().GetECSWorld()->CreateComponent<MainMenuBar>(imgui);
		GetContext().GetECSWorld()->CreateComponent<EnginePerformance>(imgui);
		GetContext().GetECSWorld()->CreateComponent<ECSInspector>(imgui);
		GetContext().GetECSWorld()->CreateComponent<FramesCounter>(imgui);

		GetContext().GetECSWorld()->RegisterSystem<ImGuiSystem>(createInfo.context_);
		GetContext().GetECSWorld()->RegisterSystem<ImGuiRenderSystem>(createInfo.context_);
		GetContext().GetECSWorld()->RegisterSystem<MainMenuBarSystem>(createInfo.context_);
		GetContext().GetECSWorld()->RegisterSystem<EnginePerformanceSystem>(createInfo.context_);
		GetContext().GetECSWorld()->RegisterSystem<CollectEntitiesInfo>(createInfo.context_);
		GetContext().GetECSWorld()->RegisterSystem<ECSInspectorSystem>(createInfo.context_);
		GetContext().GetECSWorld()->RegisterSystem<FramesCounterSystem>(createInfo.context_);

	}

}