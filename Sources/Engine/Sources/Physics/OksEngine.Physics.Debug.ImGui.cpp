#pragma once
#include <Physics\auto_OksEngine.Physics.Debug.ImGui.hpp>

namespace OksEngine
{
	namespace Physics
	{
		namespace Debug
		{
			namespace ImGUI
			{
				void CreatePhysicsMenu::Update(ECS2::Entity::Id entity0id, const OksEngine::ImGUI::State* imGUI__State0,
					const OksEngine::Input::HandleKeyboardEvents* input__HandleKeyboardEvents0,
					const OksEngine::Input::KeyboardEvents* input__KeyboardEvents0) {


					CreateComponent<PhysicsMenu>(entity0id, false);

				};


			}

		} // namespace Debug

	} // namespace Physics

	namespace Physics
	{
		namespace Debug
		{
			namespace ImGUI
			{
				void CreateDebugRenderMenuItem::Update(ECS2::Entity::Id entity0id, const OksEngine::ImGUI::State* imGUI__State0,
					const OksEngine::MainMenuBar* mainMenuBar0,
					const OksEngine::Physics::Debug::ImGUI::PhysicsMenu* physicsMenu0) {
					CreateComponent<DebugRenderMenuItem>(entity0id);

				};

			}

		} // namespace Debug

	} // namespace Physics

	namespace Physics
	{
		namespace Debug
		{
			namespace ImGUI
			{
				void BeginPhysicsMenu::Update(ECS2::Entity::Id entity0id, const OksEngine::ImGUI::State* imGUI__State0,
					const OksEngine::MainMenuBar* mainMenuBar0,
					OksEngine::Physics::Debug::ImGUI::PhysicsMenu* physicsMenu0) {


					physicsMenu0->show_ = false;
					if (ImGui::BeginMenu("Physics")) {
						physicsMenu0->show_ = true;
					}

				};

			}

		} // namespace Debug

	} // namespace Physics

	namespace Physics
	{
		namespace Debug
		{
			namespace ImGUI
			{
				void ProcessDebugRenderMenuItem::Update(
					ECS2::Entity::Id entity0id, const OksEngine::MainMenuBar* mainMenuBar0,
					const OksEngine::ImGUI::State* imGUI__State0, const OksEngine::Physics::Debug::ImGUI::PhysicsMenu* physicsMenu0,
					const OksEngine::Physics::Debug::ImGUI::DebugRenderMenuItem* debugRenderMenuItem0) {


					if (physicsMenu0->show_) {
						if (ImGui::MenuItem("DebugRender", "Ctrl+E")) {
							if (!IsComponentExist<DebugRenderWindow>(entity0id)) {
								CreateComponent<DebugRenderWindow>(entity0id);
								ECS2::Entity::Id debuggerEntityId = CreateEntity();
								CreateComponent<Debugger>(debuggerEntityId);
								CreateComponent<CommonParameters>(debuggerEntityId,
									false, false, false,
									false, false, false,
									false, false, false, false);
								CreateComponent<ContactPointsParameters>(debuggerEntityId,
									false,
									false);
								CreateComponent<ConstraintsParameters>(debuggerEntityId,
									false,
									false,
									false);
							}
						}
					}


				};

			}

		} // namespace Debug

	} // namespace Physics

	namespace Physics
	{
		namespace Debug
		{
			namespace ImGUI
			{
				void BeginDebugRenderWindow::Update(ECS2::Entity::Id entity0id, const OksEngine::ImGUI::State* imGUI__State0,
					const OksEngine::MainMenuBar* mainMenuBar0,
					const OksEngine::Physics::Debug::ImGUI::PhysicsMenu* physicsMenu0,
					const OksEngine::Physics::Debug::ImGUI::DebugRenderMenuItem* debugRenderMenuItem0,
					OksEngine::Physics::Debug::ImGUI::DebugRenderWindow* debugRenderWindow0, ECS2::Entity::Id entity1id,
					const OksEngine::Physics::Debug::Debugger* debugger1) {


					bool isOpen = true;

					ImGui::Begin("Physics Debug Render",
						&isOpen,
						ImGuiWindowFlags_AlwaysVerticalScrollbar);

					if (!isOpen) {
						RemoveComponent<DebugRenderWindow>(entity0id);
						DestroyEntity(entity1id);
					}

				};

			}

		} // namespace Debug

	} // namespace Physics

	namespace Physics::Debug::ImGUI {
		void ShowParameters::Update(
			ECS2::Entity::Id entity0id, const OksEngine::ImGUI::State* imGUI__State0,
			const OksEngine::MainMenuBar* mainMenuBar0,
			const OksEngine::Physics::Debug::ImGUI::PhysicsMenu* physicsMenu0,
			const OksEngine::Physics::Debug::ImGUI::DebugRenderMenuItem* debugRenderMenuItem0,
			const OksEngine::Physics::Debug::ImGUI::DebugRenderWindow* debugRenderWindow0,
			ECS2::Entity::Id entity1id,
			const OksEngine::Physics::Engine* engine1,
			const OksEngine::Physics::SimulationGranularity* simulationGranularity1,
			
			ECS2::Entity::Id entity2id,
			const OksEngine::Physics::Debug::Debugger* debugger2,
			OksEngine::Physics::Debug::CommonParameters* commonParameters2,
			OksEngine::Physics::Debug::ContactPointsParameters* contactPointsParameters2,
			OksEngine::Physics::Debug::ConstraintsParameters* constraintsParameters2) {

			const ECS2::ComponentsFilter debuggerFilter = GetComponentsFilter(entity2id);
			const ECS2::ComponentsFilter engineFilter = GetComponentsFilter(entity1id);

			ImGui::SeparatorText("Common:");


			ImGui::Checkbox("One frame simulation mode", &commonParameters2->oneFrameSimulateMode_);
			if (commonParameters2->oneFrameSimulateMode_) {
				if (ImGui::Button("One step")) {
					if (!engineFilter.IsSet<EnableSimulation>()) {
						CreateComponent<EnableSimulation>(entity1id);
					}
				}
				if (engineFilter.IsSet<EnableSimulation>()) {
					RemoveComponent<EnableSimulation>(entity1id);
				}
			}

			if (engineFilter.IsSet<EnableSimulation>()) {
				if (ImGui::Button("Disable simulation")) {
					RemoveComponent<EnableSimulation>(entity1id);
				}
			}
			else {
				if (ImGui::Button("Enable simulation")) {
					CreateComponent<EnableSimulation>(entity1id);
				}
			}

			ImGui::TextDisabled("Granularity %d ms", simulationGranularity1->ms_);

			if (debuggerFilter.IsSet<EnableDebugRender>()) {
				if (ImGui::Button("Disable render")) {
					RemoveComponent<EnableDebugRender>(entity2id);
				}
			}
			else {
				if (ImGui::Button("Enable render")) {
					CreateComponent<EnableDebugRender>(entity2id);
				}
			}

			ImGui::Checkbox("Draw normals", &commonParameters2->drawNormals_);
			ImGui::Checkbox("Draw AABB", &commonParameters2->drawAABB_);
			ImGui::Checkbox("Draw edges", &commonParameters2->drawEdges_);
			ImGui::Checkbox("Draw body axes", &commonParameters2->drawBodyAxes_);
			ImGui::Checkbox("Draw actor axes", &commonParameters2->drawActorAxes_);
			ImGui::Checkbox("Draw world axes", &commonParameters2->drawWorldAxes_);
			ImGui::Checkbox("Draw collision static", &commonParameters2->drawCollisionStatic_);
			ImGui::Checkbox("Draw collision dynamic", &commonParameters2->drawCollisionDynamic_);

			ImGui::SeparatorText("Constraints:");
			ImGui::Checkbox("Enable constraints debug", &constraintsParameters2->enable_);
			ImGui::Checkbox("Draw joint local frames", &constraintsParameters2->drawJointLocalFrames_);
			ImGui::Checkbox("Draw joint limits", &constraintsParameters2->drawJointLimits_);

			ImGui::SeparatorText("Contact points:");
			ImGui::Checkbox("Draw contact points", &contactPointsParameters2->drawContactPoints_);
			ImGui::Checkbox("Draw contact normals", &contactPointsParameters2->drawContactNormal_);

		}
		
	} // namespace Physics

	namespace Physics
	{
		namespace Debug
		{
			namespace ImGUI
			{
				void EndDebugRenderWindow::Update(ECS2::Entity::Id entity0id, const OksEngine::ImGUI::State* imGUI__State0,
					const OksEngine::MainMenuBar* mainMenuBar0,
					const OksEngine::Physics::Debug::ImGUI::PhysicsMenu* physicsMenu0,
					const OksEngine::Physics::Debug::ImGUI::DebugRenderMenuItem* debugRenderMenuItem0,
					const OksEngine::Physics::Debug::ImGUI::DebugRenderWindow* debugRenderWindow0) {

					ImGui::End();

				};

			}

		} // namespace Debug

	} // namespace Physics

	namespace Physics
	{
		namespace Debug
		{
			namespace ImGUI
			{
				void EndPhysicsMenu::Update(ECS2::Entity::Id entity0id, const OksEngine::ImGUI::State* imGUI__State0,
					const OksEngine::MainMenuBar* mainMenuBar0,
					const OksEngine::Physics::Debug::ImGUI::PhysicsMenu* physicsMenu0) {

					if (physicsMenu0->show_) {
						ImGui::EndMenu();

					}
				};

			}

		} // namespace Debug

	} // namespace Physics

} // namespace OksEngine