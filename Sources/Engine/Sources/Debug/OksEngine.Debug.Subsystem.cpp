#include <Debug/OksEngine.Debug.Subsystem.hpp>

//#include <Debug/auto_OksEngine.Debug.Components.hpp>
//#include <Debug/OksEngine.Debug.Systems.hpp>

namespace OksEngine {

	DebugSubsystem::DebugSubsystem(const CreateInfo& createInfo) : Subsystem{ Subsystem::Type::Debug, createInfo.context_ } {

		const ECS2::Entity::Id entity = GetContext().GetECS2World()->CreateEntity();

		std::shared_ptr<ECS2::World> world = GetContext().GetECS2World();

		//world->CreateComponent<ImGuiState>(imgui);
		//world->CreateComponent<MainMenuBar>(imgui);
		//world->CreateComponent<EnginePerformance>(imgui);
		//world->CreateComponent<ECSInspector>(imgui);
		//world->CreateComponent<FramesCounter>(imgui);
		//GetContext().GetECSWorld()->RegisterSystem < CreateImGUIModel>(createInfo.context_);
		//GetContext().GetECSWorld()->RegisterSystem < UpdateImGUIRenderData>(createInfo.context_);
		//GetContext().GetECSWorld()->RegisterSystem < AddImGuiMeshToRender>(createInfo.context_);
		//GetContext().GetECSWorld()->RegisterSystem<CreateImGuiTexture>(createInfo.context_);
		//GetContext().GetECSWorld()->RegisterSystem<RenderImGuiUI>(createInfo.context_);
		//GetContext().GetECSWorld()->RegisterSystem<UpdateMainMenuBar>(createInfo.context_);
		//GetContext().GetECSWorld()->RegisterSystem<UpdateEnginePerformance>(createInfo.context_);
		//GetContext().GetECSWorld()->RegisterSystem<CollectEntitiesInfo>(createInfo.context_);
		//GetContext().GetECSWorld()->RegisterSystem<AddECSInspectorMainMenuItem>(createInfo.context_);

		//GetContext().GetECSWorld()->RegisterSystem<CollectECSSystemsCallsInfo>(createInfo.context_);

	}

}