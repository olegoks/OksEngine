#pragma once
#include <Common\Async\auto_OksEngine.Async.hpp>

namespace OksEngine
{

	namespace Async
	{
		namespace Manager
		{
			void Create::Update() {
			
				const ECS2::Entity::Id entityId = CreateEntity();
				CreateComponent<Manager::Tag>(entityId);
			};

			void ChangeTaskState::Update(
				ECS2::Entity::Id entity0id, const OksEngine::Async::Manager::Task::Tag* task__Tag0,
				const OksEngine::Async::Manager::Task::Info* task__Info0) {
				
				const ECS2::ComponentsFilter taskCF = GetComponentsFilter(entity0id);

				ASSERT_FMSG(
					taskCF.IsSetOnlyOneOf<ASYNC__MANAGER__TASK__STATES>(),
					"Task must contain one of state components.");

				if (taskCF.IsSet<Task::State::InProgress>()) {
					if (task__Info0->future_._Is_ready()) {
						RemoveComponent<Task::State::InProgress>(entity0id);
						CreateComponent<Task::State::Finished>(entity0id);
					}
				} else if (taskCF.IsSet<Task::State::Pending>()) {
					RemoveComponent<Task::State::Pending>(entity0id);
					CreateComponent<Task::State::InProgress>(entity0id);
				}
			
			};

		}

	} // namespace Async

} // namespace OksEngine