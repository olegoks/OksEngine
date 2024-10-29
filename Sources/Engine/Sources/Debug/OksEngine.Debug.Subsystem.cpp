#include <Debug/OksEngine.Debug.Subsystem.hpp>

#include <Debug/OksEngine.Debug.Components.hpp>
#include <Debug/OksEngine.Debug.Systems.hpp>

namespace OksEngine {

	DebugSubsystem::DebugSubsystem(const CreateInfo& createInfo) : Subsystem{ Subsystem::Type::Debug, createInfo.context_ } {

		const ECS::Entity::Id imgui = GetContext().GetECSWorld()->CreateEntity();

		GetContext().GetECSWorld()->CreateComponent<ImGuiState>(imgui);
		GetContext().GetECSWorld()->CreateComponent<MainMenuBar>(imgui);
		GetContext().GetECSWorld()->CreateComponent<EnginePerformance>(imgui);
		GetContext().GetECSWorld()->CreateComponent<ECSInspector>(imgui);
		GetContext().GetECSWorld()->CreateComponent<FramesCounter>(imgui);
		GetContext().GetECSWorld()->RegisterSystem < CreateImGUIModel>(createInfo.context_);
		GetContext().GetECSWorld()->RegisterSystem < UpdateImGUIRenderData>(createInfo.context_);
		GetContext().GetECSWorld()->RegisterSystem < AddImGuiMeshToRender>(createInfo.context_);
		GetContext().GetECSWorld()->RegisterSystem<CreateImGuiTexture>(createInfo.context_);
		GetContext().GetECSWorld()->RegisterSystem<RenderImGuiUI>(createInfo.context_);
		GetContext().GetECSWorld()->RegisterSystem<UpdateMainMenuBar>(createInfo.context_);
		GetContext().GetECSWorld()->RegisterSystem<UpdateEnginePerformance>(createInfo.context_);
		GetContext().GetECSWorld()->RegisterSystem<CollectEntitiesInfo>(createInfo.context_);
		GetContext().GetECSWorld()->RegisterSystem<AddECSInspectorMainMenuItem>(createInfo.context_);

		GetContext().GetECSWorld()->RegisterSystem<CollectECSSystemsCallsInfo>(createInfo.context_);

	}

}