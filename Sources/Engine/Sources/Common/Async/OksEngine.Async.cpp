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

			void CheckTaskFinished::Update(
				ECS2::Entity::Id entity0id, const OksEngine::Async::Manager::Task::Tag* task__Tag0,
				const OksEngine::Async::Manager::Task::InProgress* task__InProgress0,
				const OksEngine::Async::Manager::Task::Info* task__Info0) {
			
				Common::BreakPointLine();
			
			
			};

		}

	} // namespace Async

} // namespace OksEngine