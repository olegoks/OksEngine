#pragma once
#include <Resources\auto_OksEngine.ResourceSystem.hpp>

#include <Common/Async/auto_OksEngine.Async.hpp>

namespace OksEngine
{
	namespace Resource
	{
		namespace Manager
		{
			void Create::Update() {
				
				const ECS2::Entity::Id entityId = CreateEntity();
				CreateComponent<Manager::Tag>(entityId);
			
			};


			void ProcessRequest::Update(
				ECS2::Entity::Id entity0id,
				const OksEngine::Resource::Manager::Tag* manager__Tag0,

				ECS2::Entity::Id entity1id,
				const OksEngine::Resource::Manager::Request::Tag* request__Tag1) {

				const ECS2::ComponentsFilter requestComponentsFilter = GetComponentsFilter(entity1id);

				if (requestComponentsFilter.IsSet<Request::LoadFileInfo>()) {
					const Request::LoadFileInfo* loadFileInfo = GetComponent<Request::LoadFileInfo>(entity1id);
					const std::string filename = loadFileInfo->fileName_;
					const ECS2::Entity::Id asyncTaskEntityId = CreateEntity();
					CreateComponent<Async::Manager::Task::Tag>(asyncTaskEntityId);

					std::future<std::any> future = std::async(
						std::launch::async,
						[filename]() -> std::any {
						using namespace std::chrono_literals;
						std::this_thread::sleep_for(1s);
						return std::any(std::string("Hello, World!"));

						});

					//Async::Manager::Task::Info info{ future };

					CreateComponent<Async::Manager::Task::Info>(asyncTaskEntityId, std::move(future));
					CreateComponent<Async::Manager::Task::InProgress>(asyncTaskEntityId);

				}

			}

		}

	} // namespace Resource

} // namespace OksEngine